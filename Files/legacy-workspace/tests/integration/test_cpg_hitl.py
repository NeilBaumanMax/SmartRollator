#!/usr/bin/env python3
"""
test_cpg_hitl.py — CPG 控制器 HITL (Hardware-In-The-Loop) 集成测试模板

用法:
    python test_cpg_hitl.py --port /dev/tty.usbmodem12345 --baud 115200

测试流程:
    1. 连接串口，等待 MCU 就绪
    2. 发送 "t" 开启周期流
    3. 采集 N 秒数据
    4. 分析 CPG 输出是否在安全范围内
    5. 生成测试报告
"""

import argparse
import csv
import sys
import time
from dataclasses import dataclass, field
from typing import Optional

try:
    import serial
except ImportError:
    print("Error: pyserial not installed. Run: pip install pyserial")
    sys.exit(1)


@dataclass
class TestSample:
    ts: int = 0
    fx: float = 0.0
    fy: float = 0.0
    fz: float = 0.0
    cmd_mode: int = 0
    cmd_vel: int = 0
    cmd_tq: int = 0


@dataclass
class TestReport:
    name: str = ""
    passed: bool = False
    samples_count: int = 0
    duration_s: float = 0.0

    # CPG 安全范围检查
    vel_in_range: bool = True
    vel_min: int = 0
    vel_max: int = 0

    # NaN 检查
    no_nan_detected: bool = True

    failures: list = field(default_factory=list)


class CPGHITLTest:
    """CPG 控制器 HITL 测试器"""

    # 安全范围
    VEL_LIMIT_RPM = 400

    def __init__(self, port: str, baud: int = 115200, duration: float = 5.0):
        self.port = port
        self.baud = baud
        self.duration = duration
        self.ser: Optional[serial.Serial] = None
        self.samples: list[TestSample] = []
        self.report = TestReport(name="CPG HITL Test")

    def connect(self):
        print(f"[INFO] Connecting to {self.port} @ {self.baud}...")
        self.ser = serial.Serial(
            port=self.port,
            baudrate=self.baud,
            timeout=1.0,
        )
        # 等待 MCU 启动
        time.sleep(1.0)
        # 清空缓冲区
        self.ser.reset_input_buffer()
        print("[INFO] Connected.")

    def disconnect(self):
        if self.ser:
            self.ser.close()

    def send(self, cmd: str):
        if self.ser:
            self.ser.write((cmd + "\r").encode())
            time.sleep(0.1)

    def parse_line(self, line: str) -> Optional[TestSample]:
        """解析一行监控输出"""
        if "F[" not in line or "CMD[" not in line:
            return None

        try:
            sample = TestSample()
            parts = line.split("|")
            for p in parts:
                p = p.strip()
                if p.startswith("ts="):
                    sample.ts = int(p.split("=")[1])
                elif p.startswith("F["):
                    inner = p[2:-1]
                    for kv in inner.split(","):
                        k, v = kv.split("=")
                        if k == "fx":
                            sample.fx = float(v)
                        elif k == "fy":
                            sample.fy = float(v)
                        elif k == "fz":
                            sample.fz = float(v)
                elif p.startswith("CMD["):
                    inner = p[4:-1]
                    for kv in inner.split(","):
                        k, v = kv.split("=")
                        if k == "mode":
                            sample.cmd_mode = int(v)
                        elif k == "vel":
                            sample.cmd_vel = int(v)
                        elif k == "tq":
                            sample.cmd_tq = int(v)
            return sample
        except (ValueError, IndexError):
            return None

    def collect_data(self):
        """采集指定时长的数据"""
        print(f"[INFO] Collecting data for {self.duration}s...")
        print("       Toggle 't' on serial to enable streaming.")

        start = time.time()
        while (time.time() - start) < self.duration:
            if self.ser.in_waiting:
                try:
                    line = self.ser.readline().decode("utf-8", errors="replace").rstrip()
                    sample = self.parse_line(line)
                    if sample:
                        self.samples.append(sample)
                except Exception:
                    pass

        self.report.samples_count = len(self.samples)
        self.report.duration_s = time.time() - start
        print(f"[INFO] Collected {len(self.samples)} samples in {self.report.duration_s:.1f}s")

    def analyze(self):
        """分析数据，生成测试报告"""
        if not self.samples:
            self.report.failures.append("No samples collected")
            return

        # 检查速度范围
        velocities = [s.cmd_vel for s in self.samples]
        self.report.vel_min = min(velocities)
        self.report.vel_max = max(velocities)

        vel_abs_max = max(abs(v) for v in velocities)
        if vel_abs_max > self.VEL_LIMIT_RPM:
            self.report.vel_in_range = False
            self.report.failures.append(
                f"Velocity exceeded limit: max |v| = {vel_abs_max} > {self.VEL_LIMIT_RPM}"
            )

        # 检查模式
        modes = set(s.cmd_mode for s in self.samples)
        if 1 not in modes:
            self.report.failures.append("No samples with velocity mode (ctrl_mode=1)")

        # 最终判定
        self.report.passed = len(self.report.failures) == 0

    def print_report(self):
        r = self.report
        status = "PASS" if r.passed else "FAIL"
        print(f"""
╔══════════════════════════════════════════════════════╗
║           SmartRollator HITL Test Report            ║
╠══════════════════════════════════════════════════════╣
║  Test:       {r.name:<40s} ║
║  Result:     {status:<40s} ║
║  Samples:    {r.samples_count:<40d} ║
║  Duration:   {r.duration_s:<40.1f} ║
╠══════════════════════════════════════════════════════╣
║  Velocity Range: [{r.vel_min}, {r.vel_max}] rpm               ║
║  Vel in Limit:  {str(r.vel_in_range):<40s} ║
╚══════════════════════════════════════════════════════╝
""")
        if r.failures:
            print("Failures:")
            for f in r.failures:
                print(f"  - {f}")

        return 0 if r.passed else 1

    def run(self):
        try:
            self.connect()
            self.send("t")  # 开启周期流
            self.collect_data()
            self.analyze()
            return self.print_report()
        except Exception as e:
            print(f"[ERROR] {e}")
            return 1
        finally:
            self.disconnect()


def main():
    parser = argparse.ArgumentParser(description="SmartRollator CPG HITL Test")
    parser.add_argument("--port", "-p", required=True, help="Serial port")
    parser.add_argument("--baud", "-b", type=int, default=115200, help="Baud rate")
    parser.add_argument("--duration", "-d", type=float, default=5.0,
                        help="Test duration in seconds (default: 5.0)")
    args = parser.parse_args()

    test = CPGHITLTest(port=args.port, baud=args.baud, duration=args.duration)
    sys.exit(test.run())


if __name__ == "__main__":
    main()
