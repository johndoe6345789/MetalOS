#!/usr/bin/env python3

import argparse
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(
        description="TestAgent: run project test suite"
    )
    parser.add_argument(
        '--pattern', '-p',
        help='Test pattern to run',
        default=None
    )
    args = parser.parse_args()
    cmd = ['pytest']
    if args.pattern:
        cmd.extend(['-k', args.pattern])
    print(f"Running test suite{' with pattern ' + args.pattern if args.pattern else ''}...")
    ret = subprocess.run(cmd)
    sys.exit(ret.returncode)

if __name__ == '__main__':
    main()
