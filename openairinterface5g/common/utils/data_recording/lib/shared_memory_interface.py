# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief Shared Memory Interface for Data Recording App

import sysv_ipc as ipc
import time
import struct
from lib import data_recording_messages_def
import numpy as np
from lib import common_utils

DEBUG_WIRELESS_RECORDED_DATA = True
DEBUG_BUFFER_READING = False

# initialize shared memory
def attach_shm(shm_path, project_id):
    key = ipc.ftok(shm_path, project_id)
    shm = ipc.SharedMemory(key, 0, 0)
    shm.attach(0, 0)
    return shm


def detach_shm(shm):
    try:
        shm.detach()
        print("Shared memory detached successfully.")
    except ipc.ExistentialError:
        print("Shared memory segment does not exist.")

def remove_shm(shm):
    try:
        shm.remove()
        print("Shared memory removed successfully.")
    except ipc.ExistentialError:
        print("Shared memory segment does not exist.")


# Write Tracer Control Message
def write_shm(shm, args):
    # Note: Big Endian >, Little Endian <
    # Note: unsigned char B, signed char b, short h, int I, long long q
    # Note: float f, double d, string s,  char c, bool ?
    # 1: config
    # 2: record
    # 3: quit
    print(f"  [Shared Memory] Write action: {args.action}")
    if args.action == "config":
        # Determine the length of the IP address
        ip_length = len(args.bytes_IPaddress) + 1  # String terminator
        # Construct the format string dynamically
        format_string = f"{ip_length}s"
        shm.write(
            # Config action
            struct.pack("<B", 1) +
            struct.pack("<B", ip_length) +
            struct.pack(format_string, args.bytes_IPaddress) +
            struct.pack("<h", int(args.port))
        )
        print(f"  [T-Tracer] Config: IP={args.bytes_IPaddress}, port={args.port}")
    elif args.action == "record":
        shm.write(
                # Record action
                struct.pack("<B", int(2)) +
                struct.pack("<B", args.num_requested_tracer_msgs) +
                struct.pack("<{}h".format(len(args.req_tracer_msgs_indices)),
                            *args.req_tracer_msgs_indices,) +
                struct.pack("<I", args.num_records) +
                struct.pack("<h", args.start_frame_number)
        )
        print(f"  [T-Tracer] Record: num_msgs={args.num_requested_tracer_msgs}, "
              f"msg_ids={args.req_tracer_msgs_indices}, "
              f"num_records={args.num_records}, "
              f"start_frame={args.start_frame_number}")
    elif args.action == "quit":
        shm.write(struct.pack("<B", int(3)))  # Quit action
        print("  [T-Tracer] Quit")
    else:
        print("Unknown action for data recording system!")


# write shared memory task
def write_shm_task(barrier, shm_id, args):
    # Wait for threads to be ready
    barrier.wait()
    # send request to related T-Tracer Application
    write_shm(shm_id, args)

# check data if avalible in the shared memory
def is_data_available_in_memory(shm, bufIdx, general_message_header_length, timeout=10):
    start_time = time.time()
    while True:
        buf = shm.read(bufIdx + general_message_header_length)
                # Only check bytes in the range [bufIdx, bufIdx + header_length),
        # not the entire buffer from 0 — earlier records would make sum > 0
        n_bytes = sum(buf[bufIdx:bufIdx + general_message_header_length])
        if n_bytes > 0:
            if DEBUG_BUFFER_READING:
                print("Data in memory: ", n_bytes, " bytes")
            return True
        if (time.time() - start_time) > timeout:
            break
        print("Data Recording App: Waiting for Measurements!")
        time.sleep(1)
    return False

# Read data from gNB T-tracer Application
def get_frame_slot_start(shm_reading, bufIdx, general_msg_header_list,
                         general_message_header_length):
    buf = shm_reading.read(bufIdx + general_message_header_length)
    msg_id = struct.unpack("<H", buf[bufIdx: bufIdx + general_msg_header_list.get("msg_id")])[0]
    bufIdx += general_msg_header_list.get("msg_id")
    frame = struct.unpack("<H", buf[bufIdx: bufIdx + general_msg_header_list.get("frame")])[0]
    bufIdx += general_msg_header_list.get("frame")
    slot = struct.unpack("B", buf[bufIdx: bufIdx + general_msg_header_list.get("slot")])[0]
    return frame, slot

# get MSG header Info
def get_msg_header(shm_reading, bufIdx, sync_header_msg, sync_header_length):
    """Read the sync header from shared memory and return msg_id, frame, slot, and timestamp.

    Args:
        shm_reading: Shared memory handle.
        bufIdx: Current buffer index.
        sync_header_msg: Dict with field names and byte sizes (from get_sync_header_msg_list).
        sync_header_length: Total byte length of the sync header.

    Returns:
        tuple: (msg_id, frame, slot, unix_capture_ts_sec, unix_capture_ts_nsec)
    """
    buf = shm_reading.read(bufIdx + sync_header_length)
    offset = bufIdx
    msg_id    = struct.unpack('<H', buf[offset:offset + sync_header_msg["msg_id"]])[0]
    offset += sync_header_msg["msg_id"]
    frame     = struct.unpack('<H', buf[offset:offset + sync_header_msg["frame"]])[0]
    offset += sync_header_msg["frame"]
    slot      = struct.unpack('B',  buf[offset:offset + sync_header_msg["slot"]])[0]
    offset += sync_header_msg["slot"]
    unix_capture_ts_sec  = struct.unpack('<I', buf[offset:offset + sync_header_msg["unix_capture_ts_sec"]])[0]
    offset += sync_header_msg["unix_capture_ts_sec"]
    unix_capture_ts_nsec = struct.unpack('<I', buf[offset:offset + sync_header_msg["unix_capture_ts_nsec"]])[0]
    return msg_id, frame, slot, unix_capture_ts_sec, unix_capture_ts_nsec


# Get MSG ID from Shared memory
def get_msg_id_from_shm(shm_reading, bufIdx, msg_id_length):
    buf = shm_reading.read(bufIdx + msg_id_length)
    msg_id = struct.unpack("<H", buf[bufIdx: bufIdx + msg_id_length])[0]
    return msg_id

# Read data from Shared memory based Data Conversion Service message structure
def read_msg_data_from_shm(shm, bufIdx, tracer_msgs_identities):
    # read sync header to move buffer index
    # get general message header list
    general_msg_header_list, general_message_header_length = \
        data_recording_messages_def.get_general_msg_header_list()
    buf = shm.read(bufIdx + general_message_header_length)
    n_bytes = sum(buf)
    if n_bytes == 0:
        raise Exception('ERROR: No data available in memory')
    msg_id = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("msg_id")])[0]
    bufIdx += general_msg_header_list.get("msg_id")
    frame = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("frame")])[0]
    bufIdx += general_msg_header_list.get("frame")
    slot = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("slot")])[0]
    bufIdx += general_msg_header_list.get("slot")
    # get Unix capture timestamp (sec, nsec) — Unix epoch 1970
    unix_capture_ts_sec = \
        struct.unpack('<I', buf[bufIdx:bufIdx+general_msg_header_list.get("unix_capture_ts_sec")])[0]
    bufIdx += general_msg_header_list.get("unix_capture_ts_sec")
    unix_capture_ts_nsec = \
        struct.unpack('<I', buf[bufIdx:bufIdx+general_msg_header_list.get("unix_capture_ts_nsec")])[0]
    bufIdx += general_msg_header_list.get("unix_capture_ts_nsec")
    # Derive timestamp in seconds and nanoseconds (already in Unix epoch)
    timestamp_seconds = unix_capture_ts_sec
    timestamp_nseconds = unix_capture_ts_nsec
    # get frame type
    frame_type = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("frame_type")])[0]
    bufIdx += general_msg_header_list.get("frame_type")
    # get frequency range
    freq_range = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("freq_range")])[0]
    bufIdx += general_msg_header_list.get("freq_range")
    # get subcarrier spacing
    subcarrier_spacing = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("subcarrier_spacing")])[0]
    bufIdx += general_msg_header_list.get("subcarrier_spacing")
    # get cyclic prefix
    cyclic_prefix = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("cyclic_prefix")])[0]
    bufIdx += general_msg_header_list.get("cyclic_prefix")
    # get symbols per slot
    symbols_per_slot = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("symbols_per_slot")])[0]
    bufIdx += general_msg_header_list.get("symbols_per_slot")
    # get Nid cell
    Nid_cell = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("Nid_cell")])[0]
    bufIdx += general_msg_header_list.get("Nid_cell")
    # get rnti
    rnti = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("rnti")])[0]
    bufIdx += general_msg_header_list.get("rnti")
    # get rb size
    rb_size = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("rb_size")])[0]
    bufIdx += general_msg_header_list.get("rb_size")
    # get rb start
    rb_start = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("rb_start")])[0]
    bufIdx += general_msg_header_list.get("rb_start")
    # get start symbol index    
    start_symbol_index = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("start_symbol_index")])[0]
    bufIdx += general_msg_header_list.get("start_symbol_index")
    # get number of symbols
    nr_of_symbols = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("nr_of_symbols")])[0]
    bufIdx += general_msg_header_list.get("nr_of_symbols")
    # get qam modulation order
    qam_mod_order = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("qam_mod_order")])[0]
    bufIdx += general_msg_header_list.get("qam_mod_order")
    # get mcs index
    mcs_index = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("mcs_index")])[0]
    bufIdx += general_msg_header_list.get("mcs_index")
    # get mcs table
    mcs_table = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("mcs_table")])[0]
    bufIdx += general_msg_header_list.get("mcs_table")
    # get number of layers
    nrOfLayers = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("nrOfLayers")])[0]
    bufIdx += general_msg_header_list.get("nrOfLayers")
    # get transform precoding
    transform_precoding = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("transform_precoding")])[0]
    bufIdx += general_msg_header_list.get("transform_precoding")
    # get dmrs config type
    dmrs_config_type = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("dmrs_config_type")])[0]
    bufIdx += general_msg_header_list.get("dmrs_config_type")
    # get ul dmrs symb pos
    ul_dmrs_symb_pos = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("ul_dmrs_symb_pos")])[0]
    bufIdx += general_msg_header_list.get("ul_dmrs_symb_pos")
    # get number dmrs symbols
    number_dmrs_symbols = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("number_dmrs_symbols")])[0]
    bufIdx += general_msg_header_list.get("number_dmrs_symbols")
    # get dmrs port
    dmrs_port = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("dmrs_port")])[0]
    bufIdx += general_msg_header_list.get("dmrs_port")
    # get dmrs scid
    dmrs_scid = struct.unpack('<H', buf[bufIdx:bufIdx+general_msg_header_list.get("dmrs_scid")])[0]
    bufIdx += general_msg_header_list.get("dmrs_scid")
    # get nb antennas rx for gNB or nb antennas tx for UE
    nb_antennas = struct.unpack('B', buf[bufIdx:bufIdx+general_msg_header_list.get("nb_antennas")])[0]
    bufIdx += general_msg_header_list.get("nb_antennas")
    # get number of bits
    number_of_bits = struct.unpack('<I', buf[bufIdx:bufIdx+general_msg_header_list.get("number_of_bits")])[0]
    bufIdx += general_msg_header_list.get("number_of_bits")
    # get length of bytes
    length_bytes = struct.unpack('<I', buf[bufIdx:bufIdx+general_msg_header_list.get("length_bytes")])[0]
    bufIdx += general_msg_header_list.get("length_bytes")
    # print all captured data
    if DEBUG_WIRELESS_RECORDED_DATA:
        print("\n" + "="*80)
        print(f"UL CONFIG METADATA - {tracer_msgs_identities[msg_id]}")
        print(f"Unix TS: {unix_capture_ts_sec}.{unix_capture_ts_nsec:09d}  |  MSG ID: {msg_id}")
        print("-"*80)
        print(f"Frame: {frame:<5} Slot: {slot:<5} Nid Cell: {Nid_cell:<5} RNTI: {rnti:<5}")
        print(f"Frame Type: {frame_type}  Freq Range: {freq_range}  |  SCS: {subcarrier_spacing} Hz  Cyclic Prefix: {cyclic_prefix}  Symbols/Slot: {symbols_per_slot}")
        print("-"*80)
        print(f"RB: Start={rb_start:<4} Size={rb_size:<4}  |  Symbol Allocation: Start={start_symbol_index:<3} Count={nr_of_symbols:<3}")
        print(f"MCS: Index={mcs_index:<3} Table={mcs_table}  |  QAM Order: {qam_mod_order}  Layers: {nrOfLayers}  Transform Precoding: {transform_precoding}")
        print("-"*80)
        print(f"DMRS: Config Type={dmrs_config_type}  Symb Pos={ul_dmrs_symb_pos} ({number_dmrs_symbols} symbols)")
        print(f"      Port={dmrs_port}  SCID={dmrs_scid}  |  Antennas: {nb_antennas}")
        print("-"*80)
        print(f"Data: {number_of_bits} bits  ({length_bytes} bytes)")
        print("="*80)
    # raise exception if time stamp is zero, it means that the data is not recorded yet
    if unix_capture_ts_sec == 0 and unix_capture_ts_nsec == 0:
        raise Exception("ERROR: Time stamp is zero, data is not recorded yet or something wrong, check logs!")
    # get recorded data
    buf = shm.read(bufIdx + length_bytes)
    captured_data = {}
    # If message is bit message, store data in bytes
    # then the field number_of_bits should be not zero
    if "_BITS" in tracer_msgs_identities[msg_id]:
        recorded_data = struct.unpack("<" + int(length_bytes) * 'B', buf[bufIdx:bufIdx + length_bytes])
        bufIdx += length_bytes
        # convert data in bytes to bits
        bits_vector = []
        for byte in recorded_data:
            bits_vector.extend([int(bit) for bit in format(int(byte), '08b')])
        captured_data["sigmf_data_type"] = "ri8_le"
        # convert to uint8
        captured_data["recorded_data"] = np.asarray(bits_vector).astype(np.uint8) 
    else:
        recorded_data = struct.unpack("<" + int(length_bytes/2) * 'h', buf[bufIdx:bufIdx + length_bytes])
        bufIdx += length_bytes
        # Convert real data to complext data
        # converting list to array
        recorded_data = np.asarray(recorded_data)
        recorded_data_complex = common_utils.real_to_complex(recorded_data)
        captured_data["sigmf_data_type"] = "cf32_le"
        # convert to complex64
        captured_data["recorded_data"] = recorded_data_complex.astype(np.complex64)
    # store data in dictonary
    captured_data["message_id"] = msg_id
    captured_data["message_type"] = tracer_msgs_identities[msg_id]
    captured_data["frame"] = frame
    captured_data["slot"] = slot
    captured_data["unix_capture_ts_sec"] = unix_capture_ts_sec
    captured_data["unix_capture_ts_nsec"] = unix_capture_ts_nsec
    captured_data["timestamp_seconds"] = timestamp_seconds
    captured_data["timestamp_nseconds"] = timestamp_nseconds
    captured_data["frame_type"] = frame_type
    captured_data["freq_range"] = freq_range
    captured_data["subcarrier_spacing"] = subcarrier_spacing
    captured_data["cyclic_prefix"] = cyclic_prefix
    # captured_data["symbols_per_slot"] = symbols_per_slot  ... not used
    captured_data["Nid_cell"] = Nid_cell
    captured_data["rnti"] = rnti
    captured_data["rb_size"] = rb_size
    captured_data["rb_start"] = rb_start
    captured_data["start_symbol_index"] = start_symbol_index
    captured_data["nr_of_symbols"] = nr_of_symbols
    captured_data["qam_mod_order"] = qam_mod_order
    captured_data["mcs_index"] = mcs_index
    captured_data["mcs_table"] = mcs_table
    captured_data["nrOfLayers"] = nrOfLayers
    captured_data["transform_precoding"] = transform_precoding
    captured_data["dmrs_config_type"] = dmrs_config_type
    captured_data["ul_dmrs_symb_pos"] = ul_dmrs_symb_pos
    captured_data["number_dmrs_symbols"] = number_dmrs_symbols
    captured_data["dmrs_port"] = dmrs_port
    captured_data["dmrs_scid"] = dmrs_scid
    captured_data["nb_antennas"] = nb_antennas
    captured_data["number_of_bits"] = number_of_bits
    return captured_data, bufIdx

# Read data from Shared memory based Data Conversion Service message structure
def read_data_from_shm(shm, bufIdx, tracer_msgs_identities):
    # print buffer index
    if DEBUG_BUFFER_READING:
        print("Buffer Index: ", bufIdx)
    # Get MSG ID by reading Sync Header
    sync_header_msg_list, sync_header_message_length = \
        data_recording_messages_def.get_sync_header_msg_list()
    msg_id_length = sync_header_msg_list.get("msg_id")
    
    msg_id = get_msg_id_from_shm(shm, bufIdx, msg_id_length) 
    
    if DEBUG_WIRELESS_RECORDED_DATA:
        print(f"  [T-Tracer] Reading MSG ID: {msg_id} - {tracer_msgs_identities[msg_id]}")
        # Get MSG data based on Data Conversion Service message structure
        # UL IQ/bits messages (default UL data format)
        
    captured_data, bufIdx = read_msg_data_from_shm(shm, bufIdx, tracer_msgs_identities)
    
    return captured_data, bufIdx
    