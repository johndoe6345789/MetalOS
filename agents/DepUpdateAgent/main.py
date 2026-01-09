#!/usr/bin/env python3

import argparse
import subprocess
import sys
import os


def update_requirements():
    if os.path.isfile('requirements.txt'):
        print("Updating Python dependencies from requirements.txt...")
        ret = subprocess.run([sys.executable, '-m', 'pip', 'install', '--upgrade', '-r', 'requirements.txt'])
        if ret.returncode != 0:
            return ret.returncode
    return 0


def update_conan():
    if os.path.isfile('conanfile.py') or os.path.isfile('conanfile.txt'):
        print("Updating Conan dependencies...")
        ret = subprocess.run(['conan', 'install', '.'])
        if ret.returncode != 0:
            return ret.returncode
    return 0


def update_npm():
    if os.path.isfile('package.json'):
        print("Updating npm dependencies...")
        ret = subprocess.run(['npm', 'update'])
        if ret.returncode != 0:
            return ret.returncode
    return 0


def main():
    parser = argparse.ArgumentParser(
        description="DepUpdateAgent: check for and update project dependencies"
    )
    parser.add_argument(
        '--all', '-a',
        action='store_true',
        help='Run all update routines'
    )
    args = parser.parse_args()

    # Run updates
    status = 0
    if args.all or os.path.isfile('requirements.txt'):
        status |= update_requirements()
    if args.all or os.path.isfile('conanfile.py') or os.path.isfile('conanfile.txt'):
        status |= update_conan()
    if args.all or os.path.isfile('package.json'):
        status |= update_npm()

    if status != 0:
        print("Dependency update encountered errors.", file=sys.stderr)
        sys.exit(status)

    print("Dependency update complete.")
    sys.exit(0)

if __name__ == '__main__':
    main()
