# SPDX-License-Identifier: LicenseRef-CSSL-1.0

# ---------------------------------------------------------------------
# brief Common utilities of Data Recording App

import os
import json

def real_to_complex(real_vector):
    # Ensure the length of the real vector is even
    if len(real_vector) % 2 != 0:
        raise ValueError("The length of the real vector must be even.")

    # Split the real vector into real and imaginary parts
    real_part = real_vector[::2]
    imag_part = real_vector[1::2]

    # Combine the real and imaginary parts to form a complex vector
    complex_vector = real_part + 1j * imag_part

    return complex_vector


def write_config_data_recording_app_json(config_meta_data):
    if config_meta_data["data_recording_config"]["global_info"][
        "save_config_data_recording_app_json"
    ]:
        try:
            json.dumps(config_meta_data)
        except (TypeError, ValueError) as e:
            print(f"data_recording_config_meta_json is not JSON serializable: {e}")

        # Specify the file name
        output_file = (
            config_meta_data["data_recording_config"]["data_storage_path"]
            + "config_data_recording_app_extended.json"
        )
        # Ensure the directory exists
        os.makedirs(os.path.dirname(output_file), exist_ok=True)

        # Write the JSON data to the file
        with open(output_file, "w") as file:
            try:
                json.dump(config_meta_data, file, indent=4)
                print(f"  [Config] JSON file saved: {output_file}")
            except Exception as e:
                print(f"  [Config] Failed to create JSON file: {e}")   
