# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief Sync service between captured Data from 5G NR gNB and UE

from lib import data_recording_messages_def
from lib import shared_memory_interface


# check if first frame ahead:
def is_frame_ahead(frame1, frame2, max_frame=1023):
    """
    Check if frame1 is ahead of frame2, considering wrap-around from max_frame to 0.

    Args:
        frame1 (int): The first frame number.
        frame2 (int): The second frame number.
        max_frame (int): The maximum frame number before wrap-around. Default is 1023.

    Returns:
        bool: True if frame1 is ahead of frame2, False otherwise.
    """
    # Calculate the difference considering wrap-around
    diff = (frame1 - frame2 + (max_frame + 1)) % (max_frame + 1)
    # If the difference is less than half the range, frame1 is ahead
    return diff < (max_frame + 1) // 2


# find the frame and slot start
def find_frame_slot_start(dataset1_start_info, dataset2_start_info, is_dataset2_pow_monitoring):
    """
    Function to find the frame and slot start for data sync
    Args:
    dataset1_start_info: Dictionary containing the start information of dataset1
    dataset2_start_info: Dictionary containing the start information of dataset2
    is_dataset2_pow_monitoring: Boolean indicating if dataset2 is pow monitoring
    Returns:
    sync_info: Dictionary containing the sync information
        sync_info["frame"] = frame_start
        sync_info["slot"]  = slot_start
    Note: if is_dataset2_pow_monitoring is true, then the slot start is taken from dataset1
    Since the pow monitoring is capturing all slots while the gNB and UE are capturing only data slots

    """
    sync_info = {}

    if dataset1_start_info["frame"] == dataset2_start_info["frame"] :
        frame_diff = 0
        frame_start = dataset1_start_info["frame"] 
        if is_dataset2_pow_monitoring:
            if dataset1_start_info["slot"] >= dataset2_start_info["slot"]:
                slot_start = dataset1_start_info["slot"]
            elif dataset1_start_info["slot"] < dataset2_start_info["slot"]:
                # Go to next frame and use data slot as reference
                frame_start = (frame_start + 1) % 1024
                # Use the slot from dataset1 as reference
                slot_start = dataset1_start_info["slot"]
        else:
            slot_start = max(dataset1_start_info["slot"], dataset2_start_info["slot"])

    elif is_frame_ahead(dataset1_start_info["frame"], dataset2_start_info["frame"]):
        frame_start = dataset1_start_info["frame"]
        slot_start = dataset1_start_info["slot"]
        frame_diff = (dataset1_start_info["frame"]- dataset2_start_info["frame"] + 1024) % 1024
    elif is_frame_ahead(dataset2_start_info["frame"], dataset1_start_info["frame"]):
        frame_start = dataset2_start_info["frame"]
        if is_dataset2_pow_monitoring:
            if dataset1_start_info["slot"] >= dataset2_start_info["slot"]:
                slot_start = dataset1_start_info["slot"]
            elif dataset1_start_info["slot"] < dataset2_start_info["slot"]:
                # Go to next frame and use data slot as reference
                frame_start = (frame_start + 1) % 1024
                # Use the slot from dataset1 as reference
                slot_start = dataset1_start_info["slot"]
        else:
            slot_start = dataset2_start_info["slot"]
        frame_diff = (dataset2_start_info["frame"] - dataset1_start_info["frame"] + 1024) % 1024
    # check first if the delta time between the two datasets is larger than expected offset time. 
    # So, the sync will not be applied
    # check during the calculation of the frame the ramp-around from 1023 to 0
    if abs(frame_diff) > 6:
        raise Exception(
            f"Frame difference between dataset1 and dataset2 is too large: {frame_diff}. "
            "This may indicate a problem with the data collection or synchronization."
        )

    # Determine the starting frame and slot for data sync
    sync_info["frame"] = frame_start
    sync_info["slot"] = slot_start
    return sync_info

# Sync data across N NR tracer nodes
def sync_multiple_nr_nodes(node_shm_readings):
    """Sync frame/slot across N NR tracer nodes.

    Args:
        node_shm_readings: dict {node_id: shm_reading}

    Returns:
        sync_info: dict with 'frame' and 'slot' of the common sync point
    """
    sync_header_msg, sync_header_msg_length = (
        data_recording_messages_def.get_sync_header_msg_list())

    # Get frame/slot from each node
    node_start_infos = {}
    for node_id, shm_reading in node_shm_readings.items():
        frame, slot = shared_memory_interface.get_frame_slot_start(
            shm_reading, 0, sync_header_msg, sync_header_msg_length)
        node_start_infos[node_id] = {"frame": frame, "slot": slot}
        print(f"  [NR Node Sync] {node_id} start: frame={frame}, slot={slot}")

    # Find the latest common frame/slot by pairwise comparison
    node_ids = list(node_start_infos.keys())
    sync_info = node_start_infos[node_ids[0]]
    for i in range(1, len(node_ids)):
        sync_info = find_frame_slot_start(sync_info, node_start_infos[node_ids[i]],
                                          is_dataset2_pow_monitoring=False)

    print(f"  [NR Node Sync] Multi-node sync point: frame={sync_info['frame']}, slot={sync_info['slot']}")
    return sync_info

