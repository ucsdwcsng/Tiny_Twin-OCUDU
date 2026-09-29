# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief defination of captured data recording messages

# Get Common Sync Header - number of bytes
def get_sync_header_msg_list():
    """
    shared memory layout written from the app:
    =================================
    msg_id                  (uint8)  message type ID
    frame                   (uint16)
    slot                    (uint8)
    unix_capture_ts_sec       (uint32)  Unix epoch seconds
    unix_capture_ts_nsec      (uint32)  nanoseconds [0, 999999999]

    """
    # Get Common Sync Header - number of bytes
    sync_header_msg = {
        "msg_id": 2,
        "frame": 2,
        "slot": 1,
        "unix_capture_ts_sec": 4,
        "unix_capture_ts_nsec": 4,
    }
    # initial number of bytes to read to get data
    sync_header_msg_length = 0
    for key, value in sync_header_msg.items():
        sync_header_msg_length = sync_header_msg_length + value
    return sync_header_msg, sync_header_msg_length

# Data Collection Trace Messages - General message structure - number of bytes
def get_general_msg_header_list():
    """
    shared memory layout written from the app:
    =================================
    msg_id                  (uint8)  message type ID
    frame                   (uint16)
    slot                    (uint8)
    unix_capture_ts_sec     (uint32)  Unix epoch seconds
    unix_capture_ts_nsec    (uint32)  nanoseconds [0, 999999999]
    frame_type              (uint8)
    freq_range              (uint8)
    subcarrier_spacing      (uint8)
    cyclic_prefix           (uint8)
    symbols_per_slot        (uint8)
    Nid_cell                (uint16)
    rnti                    (uint16)
    rb_size                 (uint16)
    rb_start                (uint16)
    start_symbol_index      (uint8)
    nr_of_symbols           (uint8)
    qam_mod_order           (uint8)
    mcs_index               (uint8)
    mcs_table               (uint8)
    nrOfLayers              (uint8)
    transform_precoding     (uint8)
    dmrs_config_type        (uint8)
    ul_dmrs_symb_pos        (uint16)
    number_dmrs_symbols     (uint8)
    dmrs_port               (uint16)
    dmrs_scid               (uint16)
    nb_antennas             (uint8)
    number_of_bits          (uint32)
    length_bytes            (uint32)
    For IQ Data: IQ samples: I0, Q0, I1, Q1, ... I_x, Q_x (int16)
    For bit data: bits: b0, b1, b2, ... b_x (uint8)

    """
    # Data Collection Trace Messages - General message structure - number of bytes
    general_msg_header_list = {
        "msg_id": 2,
        "frame": 2,
        "slot": 1,
        "unix_capture_ts_sec": 4,
        "unix_capture_ts_nsec": 4,
        "frame_type": 1,
        "freq_range": 1,
        "subcarrier_spacing": 1,
        "cyclic_prefix": 1,
        "symbols_per_slot": 1,
        "Nid_cell": 2,
        "rnti": 2,
        "rb_size": 2,
        "rb_start": 2,
        "start_symbol_index": 1,
        "nr_of_symbols": 1,
        "qam_mod_order": 1,
        "mcs_index": 1,
        "mcs_table": 1,
        "nrOfLayers": 1,
        "transform_precoding": 1,
        "dmrs_config_type": 1,
        "ul_dmrs_symb_pos": 2,
        "number_dmrs_symbols": 1,
        "dmrs_port": 2,
        "dmrs_scid": 2,
        "nb_antennas": 1,  # for gNB or nb_antennas_tx for UE
        "number_of_bits": 4,
        "length_bytes": 4,
    }
    # initial number of bytes to read to get data
    general_message_header_length = 0
    for key, value in general_msg_header_list.items():
        general_message_header_length = general_message_header_length + value
    return general_msg_header_list, general_message_header_length
