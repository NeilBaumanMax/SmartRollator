#!/usr/bin/env python3
"""
monitor.py — SmartRollator 串口监控与调参工具

用法:
    python monitor.py --port /dev/tty.usbmodem12345 --baud 115200
    python monitor.py --port COM3 --baud 115200 --log data.csv
    python monitor.py --port /dev/tty.usbmodem12345 --baud 115200 --cmd "t"

功能:
    - 实时显示 MCU 串口输出
    - 支持发送 CLI 命令 (h/p/t)
    - 可选 CSV 日志记录
    - 支持命令交互模式
"""

import argparse
import csv
import os
import signal
import sys
import time
from datetime import datetime

try:
    import serial
except ImportError:
    print("Error: pyserial not installed. Run: pip install pyserial")
    sys.exit(1)


class SmartRollatorMonitor:
    """SmartRollator 串口监控器"""

    # 解析模式: force 和 cmd 行
    @staticmethod
    def parse_line(line: str) -> dict | None:
        """尝试解析一行监控输出为字典"""
        record = {"raw": line, "timestamp": datetime.now().isoformat()}

        # 尝试解析 ts=... F[...] CMD[...] 格式
        if "F[" in line and "CMD[" in line:
            try:
                # ts=12345 | F[fx=1.2,fy=3.4,fz=5.6] | CMD[mode=1,vel=100,tq=0]
                parts = line.split("|")
                for p in parts:
                    p = p.strip()
                    if p.startswith("ts="):
                        record["ts"] = int(p.split("=")[1])
                    elif p.startswith("F["):
                        inner = p[2:-1]
                        for kv in inner.split(","):
                            k, v = kv.split("=")
                            record[f"force_{k}"] = float(v)
                    elif p.startswith("CMD["):
                        inner = p[4:-1]
                        for kv in inner.split(","):
                            k, v = kv.split("=")
                            if k == "mode":
                                record["cmd_mode"] = int(v)
                            elif k == "vel":
                                record["cmd_vel"] = int(v)
                            elif k == "tq":
                                record["cmd_tq"] = int(v)
                return record
            except (ValueError, IndexError):
                pass

        return record

    def __init__(self, port: str, baud: int = 115200, log_file: str | None = None):
        self.port = port
        self.baud = baud
        self.log_file = log_file
        self.ser: serial.Serial | None = None
        self.csv_writer = None
        self.csv_file = None
        self.running = True

    def open(self):
        """打开串口"""
        print(f"[INFO] Opening {self.port} @ {self.baud} baud...")
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baud,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=0.1,
            )
        except serial.SerialException as e:
            print(f"[ERROR] Cannot open serial port: {e}")
            print("Available ports:")
            import serial.tools.list_ports
            for p in serial.tools.list_ports.comports():
                print(f"  {p.device} - {p.description}")
            sys.exit(1)

        print(f"[INFO] Connected. Press Ctrl+C to exit, type 'h' for help.")

    def send_command(self, cmd: str):
        """发送 CLI 命令"""
        if self.ser and self.ser.is_open:
            self.ser.write((cmd + "\r").encode())
            print(f"[SEND] {cmd}")

    def start_logging(self):
        """启动 CSV 日志"""
        if self.log_file:
            self.csv_file = open(self.log_file, "w", newline="")
            fieldnames = [
                "timestamp", "ts",
                "force_fx", "force_fy", "force_fz",
                "cmd_mode", "cmd_vel", "cmd_tq",
                "raw",
            ]
            self.csv_writer = csv.DictWriter(self.csv_file, fieldnames=fieldnames)
            self.csv_writer.writeheader()
            print(f"[INFO] Logging to {self.log_file}")

    def stop_logging(self):
        """停止日志"""
        if self.csv_file:
            self.csv_file.close()
            print(f"[INFO] Log saved to {self.log_file}")

    def print_help(self):
        """打印帮助"""
        print("""
Commands (in monitor):
  h          - This help
  t          - Toggle periodic stream
  p          - Print once snapshot
  Ctrl+C     - Exit
""")

    def interactive_loop(self):
        """交互式主循环"""
        import select
        import threading

        def reader_thread():
            while self.running and self.ser and self.ser.is_open:
                try:
                    line_bytes = self.ser.readline()
                    if line_bytes:
                        try:
                            line = line_bytes.decode("utf-8", errors="replace").rstrip()
                        except UnicodeDecodeError:
                            line = line_bytes.decode("latin-1", errors="replace").rstrip()
                        if line:
                            print(f"\r[RX] {line}")
                            if self.csv_writer:
                                record = self.parse_line(line)
                                if record:
                                    self.csv_writer.writerow(record)
                                    self.csv_file.flush()
                except serial.SerialException:
                    break
                except Exception as e:
                    print(f"\r[ERR] {e}")
                    break

        thread = threading.Thread(target=reader_thread, daemon=True)
        thread.start()

        print("[INFO] Interactive mode. Type commands and press Enter.")
        self.print_help()

        while self.running:
            try:
                user_input = input("> ").strip()
                if user_input in ("h", "H"):
                    self.print_help()
                elif user_input in ("t", "T", "p", "P"):
                    self.send_command(user_input)
                elif user_input in ("q", "quit", "exit"):
                    self.running = False
                elif user_input:
                    self.send_command(user_input)
            except (EOFError, KeyboardInterrupt):
                self.running = False

    def run(self, initial_command: str | None = None):
        """主运行入口"""
        signal.signal(signal.SIGINT, lambda s, f: setattr(self, "running", False))

        self.open()
        self.start_logging()

        if initial_command:
            print(f"[INFO] Sending initial command: {initial_command}")
            time.sleep(0.5)  # 等待 MCU 就绪
            self.send_command(initial_command)

        self.interactive_loop()

        self.stop_logging()
        if self.ser:
            self.ser.close()
        print("\n[INFO] Monitor closed.")


def main():
    parser = argparse.ArgumentParser(
        description="SmartRollator Serial Monitor",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--port", "-p", required=True,
                        help="Serial port (e.g., /dev/tty.usbmodem12345, COM3)")
    parser.add_argument("--baud", "-b", type=int, default=115200,
                        help="Baud rate (default: 115200)")
    parser.add_argument("--log", "-l", default=None,
                        help="CSV log file path")
    parser.add_argument("--cmd", "-c", default=None,
                        help="Initial command to send (h/p/t)")
    args = parser.parse_args()

    monitor = SmartRollatorMonitor(
        port=args.port,
        baud=args.baud,
        log_file=args.log,
    )
    monitor.run(initial_command=args.cmd)


if __name__ == "__main__":
    main()
