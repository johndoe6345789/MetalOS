#!/usr/bin/env python3

import argparse
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(
        description="LintAgent: run linters and formatters"
    )
    parser.add_argument(
        '--fix',
        action='store_true',
        help='Automatically fix formatting issues'
    )
    args = parser.parse_args()

    linters = [
        ['flake8', '.'],
        ['clang-tidy', '-p', 'build']
    ]

    for cmd in linters:
        print(f"Running {' '.join(cmd)}...")
        ret = subprocess.run(cmd)
        if ret.returncode != 0:
            print(f"Linting failed: {' '.join(cmd)}", file=sys.stderr)
            if not args.fix:
                sys.exit(ret.returncode)

    if args.fix:
        print("Auto-formatting with black...")
        ret = subprocess.run(['black', '.'])
        if ret.returncode != 0:
            print("Formatting failed.", file=sys.stderr)
            sys.exit(ret.returncode)

    print("Linting and formatting complete.")

if __name__ == '__main__':
    main()
