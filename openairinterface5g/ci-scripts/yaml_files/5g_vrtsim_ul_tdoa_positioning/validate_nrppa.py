import re
import sys
import argparse

def extract_lmf_k1(filepath):
    """
    Parses the OAI LMF log for K1 values.
    Matches lines like: measurement: gnbId: 0xe00, trpId: 1 insert key k1 = 492592
    """
    lmf_k1_map = {}
    lmf_regex = re.compile(r"measurement: gnbId:.*?, trpId: (\d+) insert key k1 = (\d+)")

    try:
        with open(filepath, 'r') as file:
            for line in file:
                match = lmf_regex.search(line)
                if match:
                    trp_id = int(match.group(1))
                    k1_val = int(match.group(2))
                    lmf_k1_map[trp_id] = k1_val
    except FileNotFoundError:
        print(f"Error: Could not find LMF log file at {filepath}")
        sys.exit(1)

    return lmf_k1_map

def extract_gnb_k1(filepath):
    """
    Parses the gNB log for K1 values.
    Matches lines like: [NR_MAC] TRP 1, ToA  81, mu 1, k value 492592
    """
    gnb_k1_map = {}
    gnb_regex = re.compile(r"TRP (\d+),.*?k value (\d+)")

    try:
        with open(filepath, 'r') as file:
            for line in file:
                match = gnb_regex.search(line)
                if match:
                    # Extract the index directly (no longer adding +1)
                    trp_id = int(match.group(1))
                    k1_val = int(match.group(2))
                    gnb_k1_map[trp_id] = k1_val
    except FileNotFoundError:
        print(f"Error: Could not find gNB log file at {filepath}")
        sys.exit(1)

    return gnb_k1_map

def main():
    parser = argparse.ArgumentParser(description="Compare K1 values between LMF and gNB logs.")
    parser.add_argument("--lmf-log", required=True, help="Path to the OAI LMF log file")
    parser.add_argument("--gnb-log", required=True, help="Path to the gNB MAC log file")
    args = parser.parse_args()

    print("--- Extracting K1 Values ---")
    lmf_data = extract_lmf_k1(args.lmf_log)
    gnb_data = extract_gnb_k1(args.gnb_log)

    if not lmf_data:
        print("FAIL: No K1 values found in LMF logs.")
        sys.exit(1)
    if not gnb_data:
        print("FAIL: No K1 values found in gNB logs.")
        sys.exit(1)

    print(f"Found {len(lmf_data)} TRP records in LMF log.")
    print(f"Found {len(gnb_data)} TRP records in gNB log.")

    print("\n--- Comparing K1 Values per TRP ---")

    all_passed = True
    # Ensure we test all TRPs found in either file to catch missing entries
    all_trps = sorted(set(lmf_data.keys()).union(set(gnb_data.keys())))

    for trp in all_trps:
        lmf_val = lmf_data.get(trp)
        gnb_val = gnb_data.get(trp)

        if lmf_val is None:
            print(f"[FAIL] TRP {trp}: Missing in LMF logs (gNB K1 = {gnb_val})")
            all_passed = False
        elif gnb_val is None:
            print(f"[FAIL] TRP {trp}: Missing in gNB logs (LMF K1 = {lmf_val})")
            all_passed = False
        elif lmf_val == gnb_val:
            print(f"[PASS] TRP {trp}: MATCH (K1 = {lmf_val})")
        else:
            print(f"[FAIL] TRP {trp}: MISMATCH (LMF K1 = {lmf_val} | gNB K1 = {gnb_val})")
            all_passed = False

    print("\n--- Summary ---")
    if all_passed:
        print("SUCCESS: All K1 values match across components.")
        sys.exit(0)
    else:
        print("FAILURE: K1 value mismatches or missing data detected.")
        sys.exit(1)

if __name__ == "__main__":
    main()
