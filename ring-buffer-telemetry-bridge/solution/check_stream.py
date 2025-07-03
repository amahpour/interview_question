#!/usr/bin/env python3
import subprocess
import sys

TARGET_BYTES = 1000
CMD = ["./app"]

def main():
    proc = subprocess.Popen(CMD, stdout=subprocess.PIPE, text=True)

    prev = None
    seen = 0

    for line in proc.stdout:
        try:
            val = int(line.strip())
        except ValueError:
            print("FAIL")
            proc.kill(); sys.exit(1)

        if prev is not None and val != (prev + 1) % 256:
            print("FAIL")
            proc.kill(); sys.exit(1)

        prev = val
        seen += 1
        if seen >= TARGET_BYTES:
            print("PASS")
            proc.terminate()
            proc.wait(timeout=1)
            sys.exit(0)

    print("FAIL")
    sys.exit(1)

if __name__ == "__main__":
    main()
