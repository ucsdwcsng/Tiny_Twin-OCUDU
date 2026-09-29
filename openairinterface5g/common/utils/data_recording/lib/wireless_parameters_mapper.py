# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief Wireless parameters mapper to map captured parameters from waveform generator to API and SigMF format based on a mapping specification in a YAML file.
#  It also includes functions to derive other parameters that are not directly captured but can be derived from the captured parameters.
import os
import numpy as np
import yaml

STANDARDS = {"5gnr_oai": "5gnr"}


def map_waveform_metadata_to_wireless_dic(scope, waveform_generator, parameter_map_file, captured_data):
    """
    Maps metadata from Waveform creator to API and SigMF format.
    The used parameters and the mapping pairs are specified in a separate YAML file
    that must be provided as well.
    """
    # read waveform parameter map from yaml file
    dir_path = os.path.dirname(__file__)
    src_path = os.path.split(dir_path)[0]

    with open(os.path.join(src_path, parameter_map_file), "r") as file:
        parameter_map_dic = yaml.load(file, Loader=yaml.Loader)
    # preallocate target dict
    signal_metadata = {}

    #  get standard and name of generator
    standard_key = parameter_map_dic["waveform_generator"][waveform_generator]
    generator = standard_key["generator"]

    if scope == "tx":
        parameter_map_dic = parameter_map_dic["transmitter"][
            STANDARDS[waveform_generator]
        ]
    elif scope == "channel":
        parameter_map_dic = parameter_map_dic["channel"]
    elif scope == "rx":
        parameter_map_dic = parameter_map_dic["receiver"]
    else:
        raise Exception(
            f"Invalid mapping scope '{scope}'! Only 'tx', 'channel' and 'rx' are valid!"
        )

    # check if standard key is given
    if parameter_map_dic is None:
        raise Exception(
            "Invalid standard key: "
            "Name should be corrected or added to wireless_link_parameter_map.yaml, given: "
            f"{waveform_generator}"
        )

    for parameter_pair in parameter_map_dic:
        # check if key for chosen simulator even exists
        if waveform_generator + "_parameter" in parameter_pair.keys():
            # only continue with mapping from file if direct equivalent exists
            if parameter_pair[waveform_generator + "_parameter"]["name"]:
                # It is not necessary to get all parameters from wireless_link_parameter_map.yaml 
                # in captured_data since some parameters related to DL or UL only
                if (parameter_pair[waveform_generator + "_parameter"]["name"] in captured_data.keys()):
                    # extract value from waveform config source
                    value = captured_data[
                        parameter_pair[waveform_generator + "_parameter"]["name"]]
                    # additional mapping if parameter values should come from a discrete set of values
                    if ("value_map" in parameter_pair[waveform_generator + "_parameter"].keys()):
                        value = parameter_pair[waveform_generator + "_parameter"]["value_map"][value]
                    # write to target dictionary for SigMF
                    signal_metadata[parameter_pair["sigmf_parameter_name"]] = value
            else:
                raise Exception(
                    f"Incomplete specification in field '{waveform_generator}_parameter'!")
    if not signal_metadata:
        raise Exception(
            """ERROR: Check captured meta-data or provided config and meta data """)

    # check for non-JSON-serializable data types
    def isfloat(NumberString):
        try:
            float(NumberString)
            return True
        except ValueError:
            return False

    for key, value in signal_metadata.items():
        if isinstance(value, np.integer):
            signal_metadata[key] = int(value)
        elif isinstance(value, (np.float16, np.float32, np.float64)):
            signal_metadata[key] = np.format_float_positional(value, trim="-")
        elif isinstance(value, int):
            signal_metadata[key] = int(value)
        elif isinstance(value, float):
            # store value in decimal and not in scientific notation
            signal_metadata[key] = float(value)
        elif isinstance(value, str) and key != "standard":
            # convert string to lower case
            signal_metadata[key] = value.lower()
            if isfloat(value):
                if value.isdigit():
                    signal_metadata[key] = int(float(value))
                elif value.replace(".", "", 1).isdigit() and value.count(".") < 2:
                    signal_metadata[key] = float(value)

    return signal_metadata, generator


# Derive other Signal metadata from captured metadata
def derive_remaining_5gnr_metadata(captured_data, signal_metadata):
    # Add other OAI metadata to Wireless Dictionary Parameter Map - SigMF metadata
    # ----------------------------------------------------
    # Set 1: Parameters needs to be derived from OAI message
    # ----------------------------------------------------
    # DMRS Info: 
    # Check if the message type contains "UL"
    if "UL" in captured_data["message_type"]:
        # PUSCH DMRS: OFDM symbol start index within slot
        get_pusch_dmrs_start_ofdm_symbol = 0
        signal_metadata["pusch_dmrs:ofdm_symbol_idx"] = []

        for symbol in range(captured_data["start_symbol_index"], 
                            captured_data["start_symbol_index"] + captured_data["nr_of_symbols"]):

            dmrs_symbol_flag = (captured_data["ul_dmrs_symb_pos"] >> symbol) & 0x01
            if dmrs_symbol_flag and not get_pusch_dmrs_start_ofdm_symbol:
                get_pusch_dmrs_start_ofdm_symbol = 1
                signal_metadata["pusch_dmrs:start_ofdm_symbol"] = symbol
            if dmrs_symbol_flag:
                signal_metadata["pusch_dmrs:ofdm_symbol_idx"].append(symbol)  # Append symbol to the list

        signal_metadata["pusch_dmrs:duration_num_ofdm_symbols"] = 1
        signal_metadata["pusch_dmrs:num_add_positions"] = (captured_data["number_dmrs_symbols"] - 1)

    if "DL" in captured_data["message_type"]:
        # PDSCH DMRS: OFDM symbol start index within slot
        get_pdsch_dmrs_start_ofdm_symbol = 0
        signal_metadata["pdsch_dmrs:ofdm_symbol_idx"] = []
        for symbol in range(captured_data["StartSymbolIndex"], 
                            captured_data["StartSymbolIndex"] + captured_data["NrOfSymbols"]):
            dmrs_symbol_flag = captured_data["dlDmrsSymbPos"] & (1 << symbol)
            if dmrs_symbol_flag:
                if dmrs_symbol_flag and not get_pdsch_dmrs_start_ofdm_symbol:
                    get_pdsch_dmrs_start_ofdm_symbol = 1
                    signal_metadata["pdsch_dmrs:start_ofdm_symbol"] = symbol
            if dmrs_symbol_flag:
                signal_metadata["pdsch_dmrs:ofdm_symbol_idx"].append(symbol)  # Append symbol to the list

        signal_metadata["pdsch_dmrs:duration_num_ofdm_symbols"] = 1
        signal_metadata["pdsch_dmrs:num_add_positions"] = (captured_data["numDmrsSymbols"] - 1)

    # If the message is a BIT message, add number of bits to the signal info
    # "UE_PHY_UL_SCRAMBLED_TX_BITS", "GNB_PHY_UL_PAYLOAD_RX_BITS", "UE_PHY_UL_PAYLOAD_TX_BITS"
    if "_BITS" in captured_data["message_type"]:
        # if "BIT" in captured_data["message_type"]:
        signal_metadata["num_bits"] = (captured_data["number_of_bits"])

    # Set Link direction and number of antennas based on link direction and captured side
    if "UL" in captured_data["message_type"]:
        signal_metadata["link_direction"] = "uplink"
        # On UE side, It is TX antennas
        # On gNB side, It is RX antennas
        if "UE" in captured_data["message_type"]:
            signal_metadata["num_tx_antennas"] = captured_data["nb_antennas"]
        else:
            signal_metadata["num_rx_antennas"] = captured_data["nb_antennas"]

    if "DL" in captured_data["message_type"]:
        signal_metadata["link_direction"] = "downlink"
        # On UE side, It is RX antennas
        # On gNB side, It is TX antennas
        if "UE" in captured_data["message_type"]:
            signal_metadata["num_rx_antennas"] = captured_data["nb_antennas"]  ## TO DO check key name: no DL message from UE yet
        else:
            signal_metadata["num_tx_antennas"] = captured_data["nrOfLayers"]  
    # ----------------------------------------------------
    # Set 2: Parameters that is hardcoded
    # ----------------------------------------------------
    # Note: The mapping type is hardcoded. It is fixed in OAI to mapping type A for DL and B for UL.
    # Look to: openair2/RRC/NR/nr_rrc_config.c
    if "UL" in captured_data["message_type"]:
        signal_metadata["pusch:mapping_type"] = "B"
        signal_metadata["pusch:content"] = "compliant"
        signal_metadata["pusch:payload_bit_pattern"] = "random"
    if "DL" in captured_data["message_type"]:
        signal_metadata["pdsch:mapping_type"] = "A"
        signal_metadata["pdsch:content"] = "compliant"
        signal_metadata["pdsch:payload_bit_pattern"] = "random"
        
    signal_metadata["num_slots"] = 1

    return signal_metadata
