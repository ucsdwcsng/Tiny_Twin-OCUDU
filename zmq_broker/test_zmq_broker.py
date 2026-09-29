#!/usr/bin/env python3
"""Tests for zmq_broker against fake gNB and UE endpoints that speak the ZMQ radio protocol.

Usage: ZMQ_BROKER=<path to zmq_broker> python3 test_zmq_broker.py   (needs pyzmq and numpy)
"""

import os
import signal
import subprocess
import tempfile
import threading
import time
import unittest
import warnings

import numpy as np
import zmq

BROKER = os.environ.get("ZMQ_BROKER", os.path.join(os.path.dirname(os.path.abspath(__file__)), "build", "zmq_broker"))
TIMEOUT_MS = 5000


def samples(rng, n):
    return (rng.standard_normal(n) + 1j * rng.standard_normal(n)).astype(np.complex64)


class Transmitter(threading.Thread):
    """REP side of a link (gNB TX or UE TX): answers each request with the next chunk, then stops answering."""

    def __init__(self, ctx, endpoint, chunks):
        super().__init__(daemon=True)
        self.sock = ctx.socket(zmq.REP)
        self.sock.setsockopt(zmq.LINGER, 0)
        self.sock.bind(endpoint)
        self.chunks = list(chunks)
        self.first_request_time = None
        self.stop = threading.Event()

    def run(self):
        for chunk in self.chunks:
            while not self.stop.is_set():
                if self.sock.poll(50):
                    break
            else:
                return
            self.sock.recv()
            if self.first_request_time is None:
                self.first_request_time = time.monotonic()
            self.sock.send(chunk.tobytes())


class Receiver(threading.Thread):
    """REQ side of a link (gNB RX or UE RX): requests until `total` samples have arrived."""

    def __init__(self, ctx, endpoint, total):
        super().__init__(daemon=True)
        self.sock = ctx.socket(zmq.REQ)
        self.sock.setsockopt(zmq.LINGER, 0)
        self.sock.setsockopt(zmq.RCVTIMEO, TIMEOUT_MS)
        self.sock.connect(endpoint)
        self.total = total
        self.parts = []
        self.reply_sizes = []
        self.error = None

    def run(self):
        received = 0
        try:
            while received < self.total:
                self.sock.send(b"\x00")
                part = np.frombuffer(self.sock.recv(), dtype=np.complex64)
                self.parts.append(part)
                self.reply_sizes.append(part.size)
                received += part.size
        except zmq.Again:
            self.error = f"timed out after {received} of {self.total} samples"

    def data(self):
        return np.concatenate(self.parts) if self.parts else np.zeros(0, np.complex64)


class BrokerTest(unittest.TestCase):
    def setUp(self):
        # ctx.destroy() closes the endpoint sockets; pyzmq still warns about them when they are collected.
        warnings.simplefilter("ignore", ResourceWarning)
        self.tmp = tempfile.TemporaryDirectory()
        self.ctx = zmq.Context()
        self.proc = None

    def tearDown(self):
        if self.proc is not None:
            self.proc.terminate()
            try:
                self.proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.proc.kill()
        self.ctx.destroy(linger=0)
        self.tmp.cleanup()

    def ep(self, name):
        return f"ipc://{self.tmp.name}/{name}"

    def start_broker(self, n_ues, *extra, capture=False):
        args = [BROKER, "--gnb-tx", self.ep("gnb_tx"), "--gnb-rx", self.ep("gnb_rx"), "--stats-period", "0"]
        for i in range(n_ues):
            args += ["--ue", f"{self.ep(f'ue{i}_rx')},{self.ep(f'ue{i}_tx')}"]
        out = subprocess.PIPE if capture else subprocess.DEVNULL
        self.proc = subprocess.Popen([*args, *extra], stdout=out, text=True)

    def wait_for_line(self, prefix, timeout=5.0):
        """Next broker stdout line starting with prefix (needs start_broker(capture=True))."""
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            line = self.proc.stdout.readline()
            if line.startswith(prefix):
                return line
        self.fail(f"broker printed no '{prefix}' line")

    def join(self, *threads):
        for t in threads:
            t.join(TIMEOUT_MS / 1000 * 2)
            self.assertFalse(t.is_alive(), "endpoint thread did not finish")
            self.assertIsNone(getattr(t, "error", None))

    def test_dl_waits_for_all_ues(self):
        rng = np.random.default_rng(1)
        gnb_tx = Transmitter(self.ctx, self.ep("gnb_tx"), [samples(rng, 100)])
        gnb_tx.start()
        self.start_broker(2)
        ue0 = Receiver(self.ctx, self.ep("ue0_rx"), 100)
        ue0.start()
        time.sleep(0.5)
        self.assertIsNone(gnb_tx.first_request_time, "broker pulled DL before every UE connected")
        ue1_start = time.monotonic()
        ue1 = Receiver(self.ctx, self.ep("ue1_rx"), 100)
        ue1.start()
        self.join(ue0, ue1, gnb_tx)
        self.assertGreaterEqual(gnb_tx.first_request_time, ue1_start)

    def test_dl_is_copied_to_every_ue_and_split_to_reply_limit(self):
        rng = np.random.default_rng(2)
        chunks = [samples(rng, n) for n in (1000, 7, 2500, 64, 3001)]
        expected = np.concatenate(chunks)
        gnb_tx = Transmitter(self.ctx, self.ep("gnb_tx"), chunks)
        gnb_tx.start()
        self.start_broker(3, "--ue-max-reply", "1000")
        ues = [Receiver(self.ctx, self.ep(f"ue{i}_rx"), expected.size) for i in range(3)]
        for ue in ues:
            ue.start()
        self.join(*ues, gnb_tx)
        for ue in ues:
            np.testing.assert_array_equal(ue.data(), expected)
            self.assertLessEqual(max(ue.reply_sizes), 1000)

    def test_ul_is_summed_sample_aligned_across_different_chunking(self):
        rng = np.random.default_rng(3)
        total = 10000
        streams = [samples(rng, total) for _ in range(3)]
        cuts = [[1000, 4000, 9999], [10, 20, 5000, 5001], [7000]]
        ue_tx = []
        for i, (stream, cut) in enumerate(zip(streams, cuts)):
            t = Transmitter(self.ctx, self.ep(f"ue{i}_tx"), np.split(stream, cut))
            t.start()
            ue_tx.append(t)
        self.start_broker(3, "--gnb-max-reply", "1500")
        gnb_rx = Receiver(self.ctx, self.ep("gnb_rx"), total)
        gnb_rx.start()
        self.join(gnb_rx, *ue_tx)
        np.testing.assert_allclose(gnb_rx.data(), streams[0] + streams[1] + streams[2], rtol=1e-6, atol=1e-6)
        self.assertLessEqual(max(gnb_rx.reply_sizes), 1500)

    def test_single_ue_ul_is_passed_through_unchanged(self):
        rng = np.random.default_rng(4)
        chunks = [samples(rng, n) for n in (500, 1, 2000)]
        expected = np.concatenate(chunks)
        ue_tx = Transmitter(self.ctx, self.ep("ue0_tx"), chunks)
        ue_tx.start()
        self.start_broker(1)
        gnb_rx = Receiver(self.ctx, self.ep("gnb_rx"), expected.size)
        gnb_rx.start()
        self.join(gnb_rx, ue_tx)
        np.testing.assert_array_equal(gnb_rx.data(), expected)

    def test_ul_waits_for_the_slowest_ue(self):
        rng = np.random.default_rng(5)
        fast = Transmitter(self.ctx, self.ep("ue0_tx"), [samples(rng, 100)])
        fast.start()
        self.start_broker(2)
        gnb_rx = Receiver(self.ctx, self.ep("gnb_rx"), 100)
        gnb_rx.start()
        time.sleep(0.5)
        self.assertEqual(gnb_rx.reply_sizes, [], "broker answered the gNB before every UE had UL samples")
        slow = Transmitter(self.ctx, self.ep("ue1_tx"), [samples(rng, 100)])
        slow.start()
        self.join(gnb_rx, fast, slow)

    def test_bidirectional_loop(self):
        """Full DL + UL through the broker with two UEs, like the testbed."""
        rng = np.random.default_rng(6)
        dl = [samples(rng, 11520) for _ in range(20)]
        ul = [[samples(rng, 11520) for _ in range(20)] for _ in range(2)]
        gnb_tx = Transmitter(self.ctx, self.ep("gnb_tx"), dl)
        ue_tx = [Transmitter(self.ctx, self.ep(f"ue{i}_tx"), ul[i]) for i in range(2)]
        for t in (gnb_tx, *ue_tx):
            t.start()
        self.start_broker(2)
        ue_rx = [Receiver(self.ctx, self.ep(f"ue{i}_rx"), 11520 * 20) for i in range(2)]
        gnb_rx = Receiver(self.ctx, self.ep("gnb_rx"), 11520 * 20)
        for t in (*ue_rx, gnb_rx):
            t.start()
        self.join(gnb_rx, *ue_rx, gnb_tx, *ue_tx)
        for ue in ue_rx:
            np.testing.assert_array_equal(ue.data(), np.concatenate(dl))
        np.testing.assert_allclose(gnb_rx.data(), np.concatenate(ul[0]) + np.concatenate(ul[1]), rtol=1e-6, atol=1e-6)


    def test_profiling_window_counts_every_reply(self):
        """SIGUSR1/SIGUSR2 window: throughput and service-time counts match what the fake endpoints exchanged."""
        rng = np.random.default_rng(7)
        blocks, size = 10, 11520
        gnb_tx = Transmitter(self.ctx, self.ep("gnb_tx"), [samples(rng, size) for _ in range(blocks)])
        ue_tx = [Transmitter(self.ctx, self.ep(f"ue{i}_tx"), [samples(rng, size) for _ in range(blocks)]) for i in range(2)]
        for t in (gnb_tx, *ue_tx):
            t.start()
        self.start_broker(2, capture=True)
        self.wait_for_line("zmq_broker: UE 1 rx")
        self.proc.send_signal(signal.SIGUSR1)
        self.wait_for_line("zmq_broker: measurement window started")
        ue_rx = [Receiver(self.ctx, self.ep(f"ue{i}_rx"), size * blocks) for i in range(2)]
        gnb_rx = Receiver(self.ctx, self.ep("gnb_rx"), size * blocks)
        for t in (*ue_rx, gnb_rx):
            t.start()
        self.join(gnb_rx, *ue_rx, gnb_tx, *ue_tx)
        self.proc.send_signal(signal.SIGUSR2)
        summary = self.wait_for_line("zmq_broker summary:")
        f = dict(kv.split("=") for kv in summary.split(":", 1)[1].split())
        self.assertEqual(int(f["ues"]), 2)
        self.assertEqual(int(f["dl_n"]), sum(len(ue.reply_sizes) for ue in ue_rx))
        self.assertEqual(int(f["ul_n"]), len(gnb_rx.reply_sizes))
        # Every gNB DL sample goes to both UEs; every gNB UL sample is the sum of one sample from each UE.
        self.assertEqual(int(f["gnb_dl_samples"]), size * blocks)
        self.assertEqual(int(f["ue_dl_samples"]), 2 * size * blocks)
        self.assertEqual(int(f["gnb_ul_samples"]), size * blocks)
        self.assertEqual(int(f["ue_ul_samples"]), 2 * size * blocks)
        self.assertGreater(float(f["gnb_dl_msps"]), 0)
        for d in ("dl", "ul"):
            self.assertLessEqual(float(f[f"{d}_p50_us"]), float(f[f"{d}_p99_us"]))
            self.assertLessEqual(float(f[f"{d}_p99_us"]), float(f[f"{d}_max_us"]) + 1e-9)
            self.assertGreater(float(f[f"{d}_max_us"]), 0)
        self.assertTrue(0 <= float(f["busy_pct"]) <= 100)
        hist = self.wait_for_line("zmq_broker hist dl:")
        self.assertEqual(sum(int(b.split(":")[1]) for b in hist.split(":", 1)[1].split()), int(f["dl_n"]))


if __name__ == "__main__":
    unittest.main()
