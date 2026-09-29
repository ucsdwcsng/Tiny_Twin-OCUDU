# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief SigMF Interface to Write data and meta-data to files in SigMF format

import os
import sigmf
from sigmf import SigMFFile
from datetime import datetime, timezone
from sigmf import SigMFCollection
from .wireless_parameters_mapper import map_waveform_metadata_to_wireless_dic, derive_remaining_5gnr_metadata, STANDARDS
from .config_interface import get_node_by_type

DEBUG_WIRELESS_RECORDED_DATA = True

"""
SERIALIZATION_SCHEMES = {
    # gnb
    "rx-fd-data": ["subcarriers", "ofdm_symbols"],
    "tx-pilots-fd-data": ["subcarriers", "ofdm_symbols"],
    "raw-ce-fd-data": ["subcarriers", "ofdm_symbols"],
    "rx-payload-bits": ["bits", "subcarriers", "ofdm_symbols"],
    # ue
    "tx-scrambled-bits": ["bits", "subcarriers", "ofdm_symbols"],
    "tx-payload-bits": ["bits", "subcarriers", "ofdm_symbols"],
}
"""


def time_stamp_formating(unix_capture_ts_sec, unix_capture_ts_nsec, datetime_offset):
    # Convert Unix epoch (sec, nsec) to a datetime object (UTC).
    # Python datetime only supports µs, so we format the nanosecond
    # fractional part manually to preserve the full 9-digit resolution.
    dt_obj = datetime.fromtimestamp(unix_capture_ts_sec, tz=timezone.utc)

    # Format with nanosecond fractional seconds (9 digits)
    time_stamp_ns_iso = (
        dt_obj.strftime("%Y_%m_%dT%H:%M:%S")
        + f".{unix_capture_ts_nsec:09d}"
        + datetime_offset
    )
    time_stamp_ns_file_name = time_stamp_ns_iso.replace(":", "_").replace(".", "_")

    return time_stamp_ns_iso, time_stamp_ns_file_name


def _get_node_meta_data(config_meta_data, role):
    """Return the meta_data dict for the first node whose type matches the given type string."""
    node = get_node_by_type(config_meta_data["data_recording_config"]["nodes"], role)
    return node["meta_data"] if node else {}


def create_serialization_metadata(serialization_scheme, data_source: str,
                                  link_sim_parameters: dict) -> dict:
    """Creates dict that specifies the serialization metadata."""

    # retrieve parameter from LinkSimulator config
    # if serialization_scheme empty, return empty dict, It is a meta-data only message
    if not serialization_scheme:
        return {}

    if data_source == "5gnr_oai":
        num_ofdm_symbol = link_sim_parameters["nr_of_symbols"]
        num_subcarriers = link_sim_parameters["rb_size"] * 12
        num_bits_per_symbol = link_sim_parameters["qam_mod_order"]
    else:
        raise Exception(
            f"Invalid data source string '{data_source}'! Only '5gnr_oai' is valid!"
        )

    # create array with scheme dimensions
    if len(serialization_scheme) == 2:
        counts = [num_subcarriers, num_ofdm_symbol]
    elif len(serialization_scheme) == 3:
        counts = [num_bits_per_symbol, num_subcarriers, num_ofdm_symbol]
    else:
        raise Exception(
            f"Invalid number of elements in serialization scheme: {len(serialization_scheme)}")

    # create metadata dict
    serialization_dict = {
        "serialization:scheme": serialization_scheme,
        "serialization:counts": counts,
        }

    return serialization_dict

# map OAI config data to Wireless Dictionary Parameter Map - SigMF metadata
def create_system_components_metadata(captured_data, config_meta_data):
    """Creates system components (TX, channel, RX) metadata that will reside in the annotations."""

    # map OAI config data to Wireless Dictionary Parameter Map - SigMF metadata
    waveform_generator = config_meta_data["data_recording_config"]["global_info"][
        "waveform_generator"]
    parameter_map_file = config_meta_data["data_recording_config"]["parameter_map_file"]

    # map metadata of waveform generator to Wireless Dictionary Parameter Map - SigMF metadata
    signal_metadata, generator = map_waveform_metadata_to_wireless_dic(
        "tx", waveform_generator, parameter_map_file, captured_data)

    # derive remaining Signal metadata
    signal_metadata = derive_remaining_5gnr_metadata(captured_data, signal_metadata)

    tx_metadata = {
        "signal:detail": {
            "standard": STANDARDS[waveform_generator],
            "generator": generator,
            STANDARDS[waveform_generator]: signal_metadata,
        }
    }

    channel_metadata = {}   # will be filled later
    rx_metadata = {}        # will be filled later

    # get meta data from config file
    base_station_meta_data    = _get_node_meta_data(config_meta_data, "gnb")
    user_equipment_meta_data  = _get_node_meta_data(config_meta_data, "ue")

    # Set number of antennas based on link direction and captured side
    # Find number of antennas from global info on other side of the link
    if "UL" in captured_data["message_type"]:
        # On UE side, It is TX antennas
        # On gNB side, It is RX antennas
        if "UE" in captured_data["message_type"]:
            tx_metadata["signal:detail"][STANDARDS[waveform_generator]][
                "num_tx_antennas"] = captured_data["nb_antennas"]
            rx_metadata.update({"num_rx_antennas": base_station_meta_data["num_rx_antennas"]})
        else:
            tx_metadata["signal:detail"][STANDARDS[waveform_generator]][
                "num_tx_antennas"] = user_equipment_meta_data["num_tx_antennas"]
            rx_metadata.update({"num_rx_antennas": captured_data["nb_antennas"]})

    if "DL" in captured_data["message_type"]:
        tx_metadata["signal:detail"][STANDARDS[waveform_generator]][
            "link_direction"] = "downlink"
        # On UE side, It is RX antennas
        # On gNB side, It is TX antennasr
        if "UE" in captured_data["message_type"]:
            rx_metadata["signal:detail"][STANDARDS[waveform_generator]]["num_rx_antennas"] = \
                captured_data["nb_antennas"]  ## TO DO check key name: no DL message from UE yet
            tx_metadata.update({"num_tx_antennas": base_station_meta_data["num_tx_antennas"]})
        else:
            tx_metadata["signal:detail"][STANDARDS[waveform_generator]][
                "num_tx_antennas"] = captured_data["nrOfLayers"]  
            rx_metadata.update({"num_rx_antennas": user_equipment_meta_data["num_rx_antennas"]})
    return tx_metadata, channel_metadata, rx_metadata

def write_recorded_data_to_sigmf(captured_data, config_meta_data, global_info, idx):
    """
    Compiles and saves provided data and metadata into SigMF file format.
    """
    # get meta data from config file
    base_station_meta_data    = _get_node_meta_data(config_meta_data, "gnb")
    user_equipment_meta_data  = _get_node_meta_data(config_meta_data, "ue")

    # Check the receive target path is valid, else create folder
    data_storage_path = config_meta_data["data_recording_config"]["data_storage_path"]
    if not os.path.isdir(data_storage_path):
        print("Create new folder for recorded data: " + str(data_storage_path))
        os.makedirs(data_storage_path)

    # Write recorded data to file
    # Get time stamp
    time_stamp, time_stamp_file_name = time_stamp_formating(
        captured_data["unix_capture_ts_sec"], captured_data["unix_capture_ts_nsec"],
        global_info["datetime_offset"])
    # Map OAI Message Name to SigMF Message Name
    file_name_prefix = config_meta_data["data_recording_config"][
        "supported_oai_tracer_messages"][captured_data["message_type"]]["file_name_prefix"]
    recorded_data_file_name = (
        file_name_prefix + "-rec-" + str(idx) + "-" + time_stamp_file_name)

    dataset_filename = recorded_data_file_name + ".sigmf-data"
    dataset_file_path = os.path.join(data_storage_path, dataset_filename)
    if DEBUG_WIRELESS_RECORDED_DATA:
        print(dataset_file_path)
    captured_data["recorded_data"].tofile(dataset_file_path)

    # ----------------------------------------------------
    # map OAI config data to Wireless Dictionary Parameter Map - SigMF metadata
    waveform_generator = config_meta_data["data_recording_config"]["global_info"][
        "waveform_generator"]
    tx_metadata, channel_metadata, rx_metadata = create_system_components_metadata(captured_data, config_meta_data)
    
    # ----------------------------------------------------
    # Read mean parameters
    freq = config_meta_data["environment_emulation"]["target_link_config"][
        "wireless_channel"]["carrierFreqValueList_hz"][0]
    bandwidth = config_meta_data["data_recording_config"]["common_meta_data"][
        "bandwidth"]
    sample_rate = config_meta_data["data_recording_config"]["common_meta_data"]["sample_rate"]

    # get signal emitter info
    signal_emitter = {
        "manufacturer": "NI",
        "seid": user_equipment_meta_data["seid"],  # Unique ID of the emitter
        "hw": user_equipment_meta_data["hw_type"],
        "hw_subtype": user_equipment_meta_data["hw_subtype"],
        "frequency": freq,
        "sample_rate": sample_rate,
        "bandwidth": bandwidth,
        "gain_tx": user_equipment_meta_data["tx_gain"],
        "clock_reference": config_meta_data["data_recording_config"]["common_meta_data"]["clock_reference"],
        }

    # ----------------------------------------------------
    # get channel metadata
    # ----------------------------------------------------
    # get uplink channel info
    ch_uplink_config = config_meta_data["environment_emulation"]["target_link_config"]["wireless_channel"]["uplink"]
    # Real Time RF Emulation
    if config_meta_data["test_config"]["test_mode"] == "rf_real_time":
        channel_metadata.update({"emulation_mode": "ni rf real time"})
        if ch_uplink_config["type"] == "statistical_predefined":
            channel_metadata.update(
                {
                    "channel_model": ch_uplink_config["statistical"]["predef_channel_profile"],
                    "snr_esn0_db": ch_uplink_config["statistical"]["snr_db"], 
                    }
            )

        elif ch_uplink_config["type"] == "statistical_var":
            channel_metadata.update(
                {
                    "channel_model": ch_uplink_config["statistical"]["var_power_delay_profile"],
                    "snr_esn0_db": ch_uplink_config["statistical"]["snr_db"],
                    "delay_spread": ch_uplink_config["statistical"]["delay_spread_ns"] * pow(10, -9),
                    "speed": ch_uplink_config["statistical"]["speed_mps"],
                    }
            )
        else:
            raise Exception("ERROR: channel type is not supported in SigMF formatting")
        
    # RF Simulation
    elif config_meta_data["test_config"]["test_mode"] == "rf_simulation":
        channel_metadata.update({"emulation_mode": "oai rf simulation"})
        channel_metadata.update(
            {
                "channel_model": ch_uplink_config["statistical"]["predef_channel_profile"],
                "snr_esn0_db": ch_uplink_config["statistical"]["snr_db"],
                "delay_spread": ch_uplink_config["statistical"]["delay_spread_ns"]
                * pow(10, -9), "speed": ch_uplink_config["statistical"]["speed_mps"],
            }
        )
    else:
        raise Exception(
            "ERROR: Channel Emulation mode is not supported in SigMF formatting")

    channel_metadata["carrier_frequency"] = config_meta_data["environment_emulation"][
        "target_link_config"]["wireless_channel"]["carrierFreqValueList_hz"][0]

    # ----------------------------------------------------
    # get rx metadata
    # ----------------------------------------------------
    rx_metadata.update(
        {
            "bandwidth": bandwidth,
            "gain": base_station_meta_data["rx_gain"],
            "manufacturer": "NI",
            "seid": base_station_meta_data["seid"],
            "hw_subtype": base_station_meta_data["hw_subtype"],
            "clock_reference": config_meta_data["data_recording_config"][
                "common_meta_data"
            ]["clock_reference"],
            "phy_freq_domain_receiver_type": config_meta_data["test_config"][
                "dut_type"
            ],
        }
    )
    # Create sigmf metadata
    # ----------------------
    # Add global parameters to SigMF metadata
    # ----------------------
    meta = SigMFFile(
        data_file=dataset_file_path,  # extension is optional
        global_info={
            SigMFFile.DATATYPE_KEY: captured_data[
                "sigmf_data_type"
            ],  # "cf32_le",  # get_data_type_str(rx_data) - 'cf64_le' is not supported yet
            SigMFFile.SAMPLE_RATE_KEY: sample_rate,
            SigMFFile.NUM_CHANNELS_KEY: 1,
            SigMFFile.AUTHOR_KEY: global_info["author"],
            SigMFFile.DESCRIPTION_KEY: config_meta_data["data_recording_config"][
                "supported_oai_tracer_messages"
            ][captured_data["message_type"]]["description"],
            SigMFFile.RECORDER_KEY: "NI Data Recording Application for OAI",
            SigMFFile.LICENSE_KEY: "MIT License",
            # Since we are focusing on 5G NR UL, the base station is the receiver
            SigMFFile.HW_KEY: base_station_meta_data["hw_type"],
            # Disable DATASET key to mitigate the warning when read SIGMF data although it is given in the spec.
            # It seems SIGMF still has bug here
            SigMFFile.DATASET_KEY: dataset_filename,
            SigMFFile.VERSION_KEY: sigmf.__version__,
            sigmf.SigMFFile.COLLECTION_KEY: global_info["collection_file"],
        },
    )

    # ----------------------
    # Add capture parameters to SigMF metadata
    # ----------------------
    serialization_scheme = config_meta_data["data_recording_config"][
        "supported_oai_tracer_messages"][captured_data["message_type"]]["serialization_scheme"]
    capture_metadata = {
        sigmf.SigMFFile.DATETIME_KEY: time_stamp,
        SigMFFile.FREQUENCY_KEY: freq,
        **create_serialization_metadata(
            serialization_scheme, waveform_generator, captured_data
        ),
    }
    meta.add_capture(start_index=0, metadata=capture_metadata)  # Sample Start

    # create annotations dict
    system_components_dict = {
        "system_components:transmitter": [
            {
                "transmitter_id": "tx_0",
                **tx_metadata,
                "signal:emitter": signal_emitter,
            }
        ]
    }

    system_components_dict["system_components:channel"] = [
        {"transmitter_id": "tx_0", **channel_metadata}]

    system_components_dict["system_components:receiver"] = rx_metadata
    # ----------------------
    # Add annotation parameters to SigMF metadata
    # ----------------------
    meta.add_annotation(
        start_index=0,  # Sample Start
        length=len(captured_data["recorded_data"]),  # Sample count
        metadata={
            # SigMFFile.FLO_KEY: freq - sample_rate/ 2,  # args.freq - args.rate / 2,
            # SigMFFile.FHI_KEY: freq + sample_rate/ 2,  # args.freq + args.rate / 2,
            # SigMFFile.LABEL_KEY: label,
            # SigMFFile.COMMENT_KEY: general_config["comment"],
            "num_transmitters": 1,
            **system_components_dict,
        },
    )

    # Write Meta Data to file
    dataset_meta_filename = recorded_data_file_name + ".sigmf-meta"
    dataset_meta_file_path = os.path.join(data_storage_path, dataset_meta_filename)
    meta.tofile(dataset_meta_file_path)  # extension is optional

    if DEBUG_WIRELESS_RECORDED_DATA:
        print(dataset_meta_file_path)

    return dataset_meta_file_path


def save_sigmf_collection(streams: list, global_info: dict, description: str, storage_path: str):
    """Save metadata and links to SigMF data/metadata files to SigMF collection file."""
    # construct path
    storage_path = os.path.expanduser(storage_path)
    collection_filepath = os.path.join(storage_path, global_info["collection_file"])
    # statically configure Collection metadata
    collection = SigMFCollection(
        streams,
        metadata={
            SigMFCollection.COLLECTION_KEY: {
                SigMFCollection.VERSION_KEY: sigmf.__version__,
                SigMFCollection.DESCRIPTION_KEY: description,
                SigMFCollection.AUTHOR_KEY: global_info["author"],
                # SigMFCollection.LICENSE_KEY: "License",
                # SigMFCollection.EXTENSIONS_KEY: global_info["extensions"],
                SigMFCollection.STREAMS_KEY: [],
            }
        },
    )

    # save collection file
    collection.tofile(collection_filepath)
    if DEBUG_WIRELESS_RECORDED_DATA:
        print("")
        print(collection_filepath + ".sigmf-collection")


def load_sigmf(filename: str, storage_path: str, scope: str):
    """
    Loads metadata and data from SigMF file.
    """
    # construct path
    storage_path = os.path.expanduser(storage_path)
    filepath = os.path.join(storage_path, filename)
    # load SigMF file
    signal = sigmf.sigmffile.fromfile(filepath)

    # get metadata fields
    global_metadata = signal.get_global_info()
    annotations_metadata = signal.get_annotations()

    # we only consider single annotations metadata element per SigMF file
    annotation_start_idx = annotations_metadata[0][sigmf.SigMFFile.START_INDEX_KEY]
    annotation_length = annotations_metadata[0][sigmf.SigMFFile.LENGTH_INDEX_KEY]

    # get capture metadata
    #capture_metadata = signal.get_capture_info(annotation_start_idx)

    # from source code: sigmffile.py
    # "autoscale : bool, default True
    #   If dataset is in a fixed-point representation, scale samples from (min, max) to (-1.0, 1.0)
    # raw_components : bool, default False
    #   If True read and return the sample components (individual I & Q for complex, samples for real)
    #   with no conversions or interleaved channels.""
    # TODO: problem: raw_components flag is not used at all! changes in source code required!
    # TODO: data is always converted to float32 after being read

    # read actual data
    data = signal.read_samples(
        annotation_start_idx, annotation_length, autoscale=False, raw_components=True)

    return data, global_metadata, annotations_metadata