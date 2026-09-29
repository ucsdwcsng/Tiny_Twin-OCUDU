# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief main application of synchronized real-time data recording

import atexit
import signal
import sys
import time
from datetime import datetime
import json
import concurrent.futures
from termcolor import colored
import threading
# import library functions
from lib import sigmf_interface
from lib import sync_service
from lib import data_recording_messages_def
from lib import common_utils
from lib import config_interface
from lib import shared_memory_interface as shm_interface

DEBUG_WIRELESS_RECORDED_DATA = True
DEBUG_BUFFER_READING = False 

# globally applicable metadata
global_info = {
    "author": "Abdo Gaber",
    "description": "Synchronized Real-Time Data Recording",
    "timestamp": 0,
    "collection_file_prefix": "data-collection",  # collection file name prefix
    "collection_file": "",  # Reserved to be created in the code: “data-collection_rec-0_TIME-STAMP”
    "datetime_offset": "",  # datetime offset between current location and UTC/Zulu timezone
    # Example: "+01:00" for Berlin, Germany
    "save_config_data_recording_app_json": True,
    "waveform_generator": "5gnr_oai",
    "extensions": {},
}

# Supported OAI Trace messages
# UL receiver messages
# gNB IQ Msgs: "GNB_PHY_UL_FD_PUSCH_IQ", "GNB_PHY_UL_FD_DMRS", "GNB_PHY_UL_FD_CHAN_EST_DMRS_POS", 
#               "GNB_PHY_UL_FD_CHAN_EST_DMRS_INTERPL"
# gNB BITS Msgs: "GNB_PHY_UL_PAYLOAD_RX_BITS"
# UE BITS Msgs: "UE_PHY_UL_SCRAMBLED_TX_BITS", "UE_PHY_UL_PAYLOAD_TX_BITS"

supported_oai_tracer_messages = {
    # gNB messages
    "GNB_PHY_UL_FD_PUSCH_IQ": {
        "file_name_prefix": "rx-fd-data",
        "scope": "gNB",
        "description": "Frequency-domain RX data",
        "serialization_scheme": ["subcarriers", "ofdm_symbols"],
    },
    "GNB_PHY_UL_FD_DMRS": {
        "file_name_prefix": "tx-pilots-fd-data",
        "scope": "gNB",
        "description": "Frequency-domain TX PUSCH DMRS data",
        "serialization_scheme": ["subcarriers", "ofdm_symbols"],
    },
    "GNB_PHY_UL_FD_CHAN_EST_DMRS_POS": {
        "file_name_prefix": "raw-ce-fd-data",
        "scope": "gNB",
        "description": "Frequency-domain raw channel estimates (at DMRS positions)",
        "serialization_scheme": ["subcarriers", "ofdm_symbols"],
    },
    "GNB_PHY_UL_FD_CHAN_EST_DMRS_INTERPL": {
        "file_name_prefix": "raw-inter-ce-fd-data",
        "scope": "gNB",
        "description": "Interpolcated Frequency-domain raw channel estimates",
        "serialization_scheme": ["subcarriers", "ofdm_symbols"],
    },
    "GNB_PHY_UL_PAYLOAD_RX_BITS": {
        "file_name_prefix": "rx-payload-bits",
        "scope": "gNB",
        "description": "Received PUSCH payload bits",
        "serialization_scheme": ["bits", "subcarriers", "ofdm_symbols"],
    },
    # UE messages
    "UE_PHY_UL_SCRAMBLED_TX_BITS": {
        "file_name_prefix": "tx-scrambled-bits",
        "scope": "UE",
        "description": "Transmitted scrambled PUSCH bits",
        "serialization_scheme": ["bits", "subcarriers", "ofdm_symbols"],
    },
    "UE_PHY_UL_PAYLOAD_TX_BITS": {
        "file_name_prefix": "tx-payload-bits",
        "scope": "UE",
        "description": "Transmitted PUSCH payload bits",
        "serialization_scheme": ["bits", "subcarriers", "ofdm_symbols"],
    },
}    
       
def read_and_store_tracer_messages(shm_reading, bufIdx, num_messages, config_meta_data, 
                         global_info, update_timestamp=False):
    """
    Read tracer messages from shared memory while they belong to the same frame and slot.
    Continuously reads messages until a different frame/slot is encountered.
    
    Args:
        shm_reading: Shared memory reading handle
        bufIdx: Current buffer index
        num_messages: Maximum number of messages to read (used as safeguard)
        config_meta_data: Configuration metadata
        global_info: Global information dict (for timestamp on first message)
        update_timestamp: If True, update global_info timestamp from first message
    
    Returns:
        tuple: (updated_bufIdx, collected_metafiles, last_captured_data)
    """
    tracer_msgs_identities = config_meta_data["data_recording_config"]["tracer_msgs_identities"]
    collected_metafiles = []
    captured_data = None
    
    # Get sync header to check frame and slot
    sync_header_msg, sync_header_msg_length = data_recording_messages_def.get_sync_header_msg_list()
    
    # Read first message to establish the reference frame and slot
    timeout = 10  # 10 seconds timeout
    is_data_in_memory = shm_interface.is_data_available_in_memory(
        shm_reading, bufIdx, sync_header_msg_length, timeout)
    
    if not is_data_in_memory:
        if DEBUG_WIRELESS_RECORDED_DATA:
            print("Warning: No data available in shared memory")
        return bufIdx, collected_metafiles, global_info
    
    # Get reference frame and slot from first message
    ref_frame, ref_slot = shm_interface.get_frame_slot_start(
        shm_reading, bufIdx, sync_header_msg, sync_header_msg_length)
    
    if DEBUG_WIRELESS_RECORDED_DATA:
        print(f"\nRecord number: {global_info['record_idx']}, Reference Frame: {ref_frame}, Slot: {ref_slot}")
    
    idx = 0
    while idx < num_messages:
        time.sleep(0.0015)
        
        # Check if data is available
        is_data_in_memory = shm_interface.is_data_available_in_memory(
            shm_reading, bufIdx, sync_header_msg_length, timeout)
        
        if not is_data_in_memory:
            break
        
        # Get frame and slot of current message before reading
        current_frame, current_slot = shm_interface.get_frame_slot_start(
            shm_reading, bufIdx, sync_header_msg, sync_header_msg_length)
        
        # Check if we're still on the same frame and slot
        if current_frame != ref_frame or current_slot != ref_slot:
            if DEBUG_WIRELESS_RECORDED_DATA:
                print(f"Frame/Slot changed: ({ref_frame}, {ref_slot}) -> ({current_frame}, {current_slot}). Stopping at message {idx}")
            # Keep buffer index pointing to this new frame/slot message (don't increment)
            break
        
        if DEBUG_WIRELESS_RECORDED_DATA:
            print(f"Reading MSG {idx}: Frame={current_frame}, Slot={current_slot}")
        
        # Read the message
        captured_data, bufIdx = shm_interface.read_data_from_shm(
            shm_reading, bufIdx, tracer_msgs_identities)
        
        # Update timestamp from the first message if requested
        if idx == 0 and update_timestamp:
            time_stamp, time_stamp_file_name = (
                sigmf_interface.time_stamp_formating(
                    captured_data["unix_capture_ts_sec"],
                    captured_data["unix_capture_ts_nsec"],
                    global_info["datetime_offset"]))
            global_info["collection_file"] = (
                f"{global_info['collection_file_prefix']}-rec-{global_info['record_idx']}-{time_stamp_file_name}")
            global_info["timestamp"] = time_stamp
            global_info["frame"] = captured_data["frame"]
            global_info["slot"]= captured_data["slot"]
        
        # Write data into files with the given format
        if config_meta_data["data_recording_config"]["enable_saving_tracer_messages_sigmf"]:
            collected_metafiles.append(sigmf_interface.write_recorded_data_to_sigmf(
                                        captured_data, config_meta_data, global_info, global_info['record_idx']))
        
        idx += 1
    
    if DEBUG_WIRELESS_RECORDED_DATA:
        print(f"Total messages read: {idx} for Frame {ref_frame}, Slot {ref_slot}")
    
    return bufIdx, collected_metafiles, global_info

def find_sync_buffer_index(shm_reading, sync_info, tracer_msgs_identities, timeout_sec=None):
    """
    Find buffer index that matches sync frame and slot.
    
    Args:
        shm_reading: Shared memory reading handle
        sync_info: Dictionary with 'frame' and 'slot' keys
        tracer_msgs_identities: Tracer message identities
        timeout_sec: Timeout in seconds (None for no timeout)
    
    Returns:
        bufIdx: Buffer index where sync match was found (points to the matching message)
    
    Raises:
        Exception if timeout occurs before sync is found
    """
    bufIdx = 0
    timeout_sync = time.time() + timeout_sec if timeout_sec else None
    
    while True:
        time.sleep(0.0035)  # 2.3 ms = latency of T tracer to capture data from the RAN
        saved_bufIdx = bufIdx  # Save index BEFORE read
        captured_data, bufIdx = shm_interface.read_data_from_shm(
            shm_reading, bufIdx, tracer_msgs_identities)
        print(f"  [NR Data Sync] NR Data (Frame:{captured_data['frame']}, Slot:{captured_data['slot']}) | "
              f"Target (Frame:{sync_info['frame']}, Slot:{sync_info['slot']})")
        
        if (captured_data["frame"] == sync_info["frame"] and 
            captured_data["slot"] == sync_info["slot"]):
            print("  [NR Data Sync] Sync pass")
            return saved_bufIdx  # Return index pointing to the matching message
        
        if timeout_sync and time.time() > timeout_sync:
            raise Exception(
                "ERROR: Data Recording NO Sync Found, check Tracer Services if they are connected!")

def send_5g_nr_msgs_request(barrier, shm_writing, shm_reading, args):
    # send Tracer Control Message request to T-Tracers Apps
    # -------------------------------------------
    #   It consists of the following fields:
    #       Record action
    #       Number of requested Tracer Messages
    #       Requested Tracer Messages ID 1, …, ID N
    #       Number of records to be recorded in slots
    #       Start SFN: Frame Index to start data collection from it, but not yet used
    shm_interface.write_shm_task(barrier,shm_writing, args)

def generic_data_conversion_service(node_shm, config_meta_data, tracer_nodes, sync_info=None):
    """Generic data conversion service for N tracer nodes.

    Args:
        node_shm: dict {node_id: {"reading": shm, "writing": shm}}
        config_meta_data: Config metadata dict
        tracer_nodes: dict {node_id: args} for nodes with tracer messages
        sync_info: Optional sync info dict with 'frame' and 'slot'. If provided,
                   each node's buffer index is synced to this frame/slot.
    """
    record_idx = 0
    prev_frame = -1
    prev_slot = -1

    tracer_msgs_identities = config_meta_data["data_recording_config"]["tracer_msgs_identities"]
    global_info = config_meta_data["data_recording_config"]["global_info"]

    # Find sync buffer indices for each node if sync_info provided
    node_buf_idx = {}
    for node_id in tracer_nodes:
        if sync_info:
            print(f"  [NR Data Sync] Finding memory index for node {node_id}")
            node_buf_idx[node_id] = find_sync_buffer_index(
                node_shm[node_id]["reading"], sync_info, tracer_msgs_identities, timeout_sec=5)
        else:
            node_buf_idx[node_id] = 0

    # Determine num_records from first tracer node
    num_records = next(iter(tracer_nodes.values())).num_records

    print("\n--- Data Conversion: Reading data from T-tracer Applications ---")
    for node_id, args in tracer_nodes.items():
        print(f"  [Data Conversion] {node_id}: Requested tracer messages: {args.num_requested_tracer_msgs}")

    # Read data from all tracer nodes
    while True:
        if DEBUG_WIRELESS_RECORDED_DATA:
            print("\nRecord number: ", record_idx)
        if DEBUG_BUFFER_READING:
            for nid in tracer_nodes:
                print(f"  Buffer Index {nid}: {node_buf_idx[nid]}")
        time.sleep(0.0035)

        global_info["record_idx"] = record_idx
        all_metafiles = []

        for i, (node_id, args) in enumerate(tracer_nodes.items()):
            buf_idx, metafiles, global_info = read_and_store_tracer_messages(
                node_shm[node_id]["reading"], node_buf_idx[node_id],
                args.num_requested_tracer_msgs, config_meta_data, global_info,
                update_timestamp=(i == 0))
            node_buf_idx[node_id] = buf_idx
            all_metafiles.extend(metafiles)

        # Generate SigMF collection file
        if config_meta_data["data_recording_config"]["enable_saving_tracer_messages_sigmf"]:
            data_storage_path = config_meta_data["data_recording_config"]["data_storage_path"]
            description = global_info["description"]
            sigmf_interface.save_sigmf_collection(
                all_metafiles, global_info, description, data_storage_path)

        frame = global_info["frame"]
        slot = global_info["slot"]

        if frame != prev_frame or slot != prev_slot:
            record_idx += 1
            if record_idx >= num_records:
                break
        prev_frame = frame
        prev_slot = slot

# -------------------------------------------
# Cleanup — ensures shared memory and resources are released even on Ctrl+C
# -------------------------------------------
# Module-level dict populated at runtime with resources that need cleanup.
# Each key is optional; the cleanup function checks before acting.
_cleanup_state = {"done": False}


def cleanup_resources():
    """Release T-tracer shared memory resources."""
    if _cleanup_state["done"]:
        return
    _cleanup_state["done"] = True
    print("\nCleaning up resources...")

    tracer_nodes = _cleanup_state.get("tracer_nodes", {})
    node_shm = _cleanup_state.get("node_shm", {})
    for node_id in tracer_nodes:
        try:
            tracer_nodes[node_id].action = "quit"
            shm_interface.write_shm(node_shm[node_id]["writing"], tracer_nodes[node_id])
            time.sleep(0.5)
            shm_interface.detach_shm(node_shm[node_id]["reading"])
            shm_interface.detach_shm(node_shm[node_id]["writing"])
            shm_interface.remove_shm(node_shm[node_id]["reading"])
            shm_interface.remove_shm(node_shm[node_id]["writing"])
        except Exception as e:
            print(f"Warning: {node_id} shared memory cleanup error: {e}")


def register_cleanup():
    """Register cleanup_resources with atexit and signal handlers."""
    atexit.register(cleanup_resources)

    def _signal_handler(signum, frame):
        print(f"\nSignal {signum} received, shutting down...")
        cleanup_resources()
        sys.exit(130)

    signal.signal(signal.SIGINT, _signal_handler)
    signal.signal(signal.SIGTERM, _signal_handler)


if __name__ == "__main__":
    # -------------------------------------------
    # Data Control Service
    ## -------------------------------------------
    # ------------- Configuration --------------
    # ------------------------------------------
    # Data Recording Configuration
    data_recording_config_file = "config/config_data_recording.json"
    # -------------------------------------------
    # Configuration
    # -------------------------------------------
    # Read and parse the JSON file
    with open(data_recording_config_file, "r") as file:
        config_meta_data = json.load(file)
    # -------------------------------------------
    # Generic node-based configuration
    # -------------------------------------------
    config_meta_data, all_node_args = config_interface.get_data_recording_config(config_meta_data)
    _nodes_cfg = config_meta_data["data_recording_config"]["nodes"]

    # Build tracer_nodes: only nodes with requested tracer messages
    tracer_nodes = {nid: args for nid, args in all_node_args.items()
                    if args.requested_tracer_messages}
    
    # Validate: at least one node must have tracer messages
    any_tracer = bool(tracer_nodes)
    if not any_tracer:
        raise Exception("ERROR: No requested tracer messages IDs are provided")

    # -------------------------------------------
    # Attach shared memory for each tracer node
    # -------------------------------------------
    node_shm = {}  # {node_id: {"reading": shm, "writing": shm}}
    for node_id, args in tracer_nodes.items():
        node_cfg = next(n for n in _nodes_cfg if n["id"] == node_id)
        shm_cfg = node_cfg.get("shared_mem_config", {})
        read_path = shm_cfg.get("read_path")
        write_path = shm_cfg.get("write_path")
        proj_id = shm_cfg.get("project_id")
        if not read_path or not write_path or proj_id is None:
            raise Exception(
                f"ERROR: Node '{node_id}' has tracer messages but missing 'shared_mem_config' "
                f"(read_path, write_path, project_id) in config.")
        shm_writing = shm_interface.attach_shm(write_path, proj_id)
        shm_reading = shm_interface.attach_shm(read_path, proj_id)
        node_shm[node_id] = {"reading": shm_reading, "writing": shm_writing}

    # get general message header list
    general_msg_header_list, general_message_header_length = \
        data_recording_messages_def.get_general_msg_header_list()

    # Add supported OAI Tracer Messages
    config_meta_data["data_recording_config"]["supported_oai_tracer_messages"] = supported_oai_tracer_messages
    # Add global info
    config_meta_data["data_recording_config"]["global_info"] = global_info

    # -------------------------------------------
    # Initialization
    # -------------------------------------------  
    # send Tracer Control Message request to T-Tracers Apps
    #   It consists of the following fields:
    #       Config action
    #       IP address length
    #       IP address
    #       Port Number
    # Configure T-Tracers via shared memory for each tracer node
    for node_id, args in tracer_nodes.items():
        args.action = "config"
        shm_interface.write_shm(node_shm[node_id]["writing"], args)

    time.sleep(0.5)  # wait for the config to be applied

    # -------------------------------------------
    # Execution
    # -------------------------------------------
    # send Tracer Control Message request to T-Tracers Apps
    # -------------------------------------------
    #   It consists of the following fields:
    #       Record action
    #       Number of requested Tracer Messages
    #       Requested Tracer Messages ID 1, …, ID N
    #       Number of records to be recorded in slots
    #       Start SFN: Frame Index to start data collection from it, useful for future
    #       data sync between gNB and UE but not yet used
    # -------------------------------------------
    # -------- Data Collection Service ----------
    # -------------------------------------------
    print("Args:")
    for node_id, args in tracer_nodes.items():
        args.action = "record"
        print(f"  {node_id} args: ", args)
    
    # Pre-create thread pool if user would like to create a for loop for N iterations 
    # Outside the loop to avoid initialization overhead.
    # max_workers and barrier_parties are kept identical: every thread that
    # enters the pool must also call barrier.wait(), so pool size == barrier size.
    total_thread_count = len(tracer_nodes)
    thread_pool = concurrent.futures.ThreadPoolExecutor(max_workers=total_thread_count)
    barrier = threading.Barrier(total_thread_count) if total_thread_count > 0 else None
    
    # Populate cleanup state and register signal handlers
    _cleanup_state.update({
        "tracer_nodes": tracer_nodes,
        "node_shm": node_shm,
    })
    register_cleanup()

    # Start data recording iterations
    start_time = time.time()
    print("Send data logging request us:", datetime.now().strftime("%Y%m%d-%H%M%S%f"))

    # -------------------------------------------
    # Submit NR tracer threads for all tracer nodes
    # -------------------------------------------
    futures = []
    for node_id, args in tracer_nodes.items():
        shm = node_shm[node_id]
        tracer_thread = thread_pool.submit(
            send_5g_nr_msgs_request, barrier, shm["writing"], shm["reading"], args)
        futures.append(tracer_thread)

    # Wait for all threads to complete
    concurrent.futures.wait(futures)

    # -------------------------------------------
    # Check data availability for each tracer node
    # -------------------------------------------
    bufIdx = 0
    timeout = 10  # 10 seconds from now

    for node_id in tracer_nodes:
        is_data_in_memory = shm_interface.is_data_available_in_memory(
            node_shm[node_id]["reading"], bufIdx, general_message_header_length, timeout)

        if not is_data_in_memory:
            print(f"Error: {node_id}: Check T-Tracer APP, check IPs and Ports")
            print(f"Error: {node_id}: If IPs and Ports are correct, re-run the hanging app. "
                    "It seems the socket was not closed properly")
            raise Exception(
                f"ERROR: Time out, check if {node_id} T-Tracer APP connected to stack")

    # -------------------------------------------
    # Sync and convert data
    # -------------------------------------------
    common_utils.write_config_data_recording_app_json(config_meta_data)
    sync_info = {}

    # Step 1: Sync NR tracer nodes (find latest common frame/slot across all NR sources)
    if len(tracer_nodes) > 1:
        node_shm_readings = {nid: node_shm[nid]["reading"] for nid in tracer_nodes}
        sync_info = sync_service.sync_multiple_nr_nodes(node_shm_readings)
        print(f"\n--- NR Node Sync: Sync point: frame={sync_info['frame']}, slot={sync_info['slot']} ---")

    # Step 2: Read and store NR tracer data
    if tracer_nodes:
        do_sync = bool(sync_info)
        generic_data_conversion_service(
            node_shm, config_meta_data, tracer_nodes, sync_info if do_sync else None)

    # measure Elapsed time
    time_elapsed = time.time() - start_time
    time_elapsed_ms = int(time_elapsed * 1000)
    print(
        "Elapsed time of getting Requested Messages and writing data and meta data files:",
        colored(time_elapsed_ms, "yellow"),
        "ms",)

    # Shutdown thread pool after all iterations complete
    thread_pool.shutdown(wait=True)

    # Run cleanup (also registered with atexit and signal handlers for abnormal exits)
    cleanup_resources()
        
    print("End of the RF Data Recording API")
