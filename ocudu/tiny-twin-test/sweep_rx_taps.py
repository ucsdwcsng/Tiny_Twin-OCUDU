#!/usr/bin/env python3
"""Set each RX first-tap gain, collect UE measurements, and draw two plots."""

import argparse
import csv
from datetime import datetime
import math
from pathlib import Path
import re
import statistics
import subprocess
import time

ROOT = Path(__file__).resolve().parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rx-taps', nargs='+', type=float, required=True, help='gains, e.g. 1 0.75 0.5 0.25')
    parser.add_argument('--duration', type=int, default=60, help='seconds per gain (default: 60)')
    args = parser.parse_args()
    if args.duration <= 0 or any(not math.isfinite(tap) or tap < 0 for tap in args.rx_taps):
        parser.error('use nonnegative finite gains and a positive duration')

    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    config = ROOT / 'configs/oaiue_zmq.conf'
    original = config.read_text()
    setting = re.compile(r'^([ \t]*rx_taps\s*=\s*)"[^"\n]*"', re.MULTILINE)
    if len(setting.findall(original)) != 1:
        parser.error('config must contain exactly one rx_taps setting')
    output = ROOT / 'logs' / datetime.now().strftime('tap_sweep_%Y%m%d_%H%M%S_%f')
    output.mkdir(parents=True)
    compose = ['docker', 'compose', '-f', str(ROOT / 'docker-compose.yml')]
    metric = re.compile(r'DL Chan:.*?SINR\s+([-+\d.eE]+)\s+dB\s+RSRP\s+([-+\d.eE]+)\s+dBm')
    results = []

    with (output / 'results.csv').open('w', newline='') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow(['rx_tap', 'samples', 'mean_sinr_db', 'mean_rsrp_dbm'])
        for index, tap in enumerate(args.rx_taps, 1):
            print(f'RX tap = {tap:g}; collecting for {args.duration} seconds', flush=True)
            config.write_text(setting.sub(lambda m: f'{m[1]}"{tap:g},0"', original))
            subprocess.run(compose + ['up', '-d', '--no-deps', '--no-build', '--pull', 'never',
                                     '--force-recreate', 'gnb', 'ue'], cwd=ROOT, check=True, timeout=180)
            container = subprocess.check_output(compose + ['ps', '-a', '-q', 'ue'], cwd=ROOT, text=True).strip()
            if not container or '\n' in container:
                raise RuntimeError('Expected one UE container')
            time.sleep(args.duration)
            log = subprocess.check_output(['docker', 'logs', '--timestamps', container],
                                          stderr=subprocess.STDOUT, text=True, timeout=30)
            log = re.sub(r'\x1b\[[0-?]*[ -/]*[@-~]', '', log)
            lines = [line for line in log.splitlines() if metric.search(line)]
            (output / f'{index:02d}_rx_{tap:g}.log').write_text(
                f'# rx_taps = "{tap:g},0"\n' + '\n'.join(lines) + '\n')
            pairs = [tuple(map(float, metric.search(line).groups())) for line in lines]
            if pairs:
                sinr = statistics.mean(pair[0] for pair in pairs)
                rsrp = statistics.mean(pair[1] for pair in pairs)
                writer.writerow([tap, len(pairs), sinr, rsrp])
                results.append((tap, sinr, rsrp))
                print(f'  Mean SINR: {sinr:.2f} dB; RSRP: {rsrp:.2f} dBm', flush=True)
            else:
                writer.writerow([tap, 0, '', ''])
                print('  No SINR/RSRP measurements; skipping this point in the plots.', flush=True)
            csv_file.flush()

    results.sort()
    for column, ylabel, filename in [(1, 'Mean SINR (dB)', 'tap_vs_sinr.png'),
                                      (2, 'Mean RSRP (dBm)', 'tap_vs_rsrp.png')]:
        fig, ax = plt.subplots()
        ax.plot([row[0] for row in results], [row[column] for row in results], 'o-')
        ax.set(xlabel='RX first-tap amplitude', ylabel=ylabel)
        ax.grid(True)
        fig.tight_layout()
        fig.savefig(output / filename, dpi=150)
        plt.close(fig)
    print(f'Saved logs, results.csv, and two plots in {output}')
    print('The final tap setting remains active.')


if __name__ == '__main__':
    main()
