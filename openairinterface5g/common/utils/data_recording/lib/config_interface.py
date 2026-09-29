# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief Configuration Interface of Data Recording App

import json
import argparse

# Function to read the main configuration file in JSON format
def read_main_config_file_json(main_config_file: str) -> dict:
    """
    Reads main config file.
    """

    # read general parameter set from yaml config file
    with open(main_config_file, "r") as file:
        main_config = json.load(file)

    return main_config


# Function to parse the OAI T_messages file and get the index of a given string
def parse_message_file(file_path):
    with open(file_path, "r") as file:
        content = file.readlines()

    # Extract lines that start with 'ID' and remove the 'ID = ' prefix
    tracer_msgs_identities = [
        line.strip().replace("ID = ", "")
        for line in content
        if line.strip().startswith("ID")
    ]

    return tracer_msgs_identities


# Function to get the index of a given string in the list of ID lines
def get_index_of_id(tracer_msgs_identities, message_id):
    try:
        return tracer_msgs_identities.index(message_id)
    except ValueError:
        return -1  # Return -1 if the string is not found


# Function to get the indices of requested tracer messages
def get_requested_tracer_msgs_indices(requested_tracer_messages, tracer_msgs_identities):
    # get requested tracer messages indices
    req_tracer_msgs_indices = []
    for idx, value in enumerate(requested_tracer_messages):
        msg_index = get_index_of_id(tracer_msgs_identities, value)
        req_tracer_msgs_indices.append(msg_index)
    print("Requested Traces IDs: ", req_tracer_msgs_indices)
    return req_tracer_msgs_indices


# Helper: return the first node in the nodes list whose type matches the given type string
def get_node_by_type(nodes, node_type):
    for node in nodes:
        if node.get("type") == node_type:
            return node
    return None


# Function to get the data recording configuration.
# Returns (config_meta_data, node_args_dict) where node_args_dict is
# { node_id: Namespace } with one entry per node defined in the config.
def get_data_recording_config(config_meta_data):
    # get Tracer Messages IDs from the T-Tracer Messages file
    #   MSG list order should be similar to the T-Tracer Messages txt file
    #   Be sure that you are using the same T_messages file that is used in the OAI project
    #   OAI Path: .../common/utils/T/T_messages.txt

    # Parse the T_messages file
    tracer_msgs_identities = parse_message_file(
        config_meta_data["data_recording_config"]["t_tracer_message_definition_file"])
    config_meta_data["data_recording_config"]["tracer_msgs_identities"] = tracer_msgs_identities

    nodes = config_meta_data["data_recording_config"]["nodes"]

    # Build per-node args generically
    node_args_dict = {}
    for node in nodes:
        parser = argparse.ArgumentParser(description="request messages IDs")
        node_args = parser.parse_args()

        node_id = node.get("id")
        node_args.node_id = node_id
        node_args.type = node.get("type")
        node_args.requested_tracer_messages = node.get("requested_tracer_messages", [])

        if node_args.requested_tracer_messages:
            node["req_tracer_msgs_indices"] = get_requested_tracer_msgs_indices(
                node_args.requested_tracer_messages, tracer_msgs_identities)

            node_args.num_records = config_meta_data["data_recording_config"]["num_records"]
            node_args.start_frame_number = config_meta_data["data_recording_config"]["start_frame_number"]
            node_args.req_tracer_msgs_indices = node["req_tracer_msgs_indices"]
            node_args.num_requested_tracer_msgs = len(node_args.req_tracer_msgs_indices)
            # Split the string into IP and port
            tracer_addr = node.get("tracer_service_address", "")
            if tracer_addr:
                node_args.IPaddress, node_args.port = tracer_addr.split(":")
                node_args.bytes_IPaddress = bytes(node_args.IPaddress, "utf-8")

        node_args_dict[node_id] = node_args

    return config_meta_data, node_args_dict
