#!/usr/bin/env python3

import argparse
import subprocess
import sys
import os
import shutil

def main():
    parser = argparse.ArgumentParser(
        description="BuildAgent: run CMake build for the project"
    )
    parser.add_argument(
        '--build-dir', '-d',
        default='build',
        help='Build directory'
    )
    parser.add_argument(
        '--config', '-c',
        help='CMake build configuration (e.g., Debug, Release)'
    )
    parser.add_argument(
        '--clean',
        action='store_true',
        help='Clean the build directory before building'
    )
    args = parser.parse_args()
    build_dir = args.build_dir

    if args.clean and os.path.isdir(build_dir):
        print(f"Cleaning build directory: {build_dir}")
        shutil.rmtree(build_dir)

    # Configure step
    if not os.path.isdir(build_dir):
        print(f"Configuring project in {build_dir}...")
        ret = subprocess.run(['cmake', '-S', '.', '-B', build_dir])
        if ret.returncode != 0:
            print("Configuration failed.", file=sys.stderr)
            sys.exit(ret.returncode)

    # Build step
    build_cmd = ['cmake', '--build', build_dir]
    if args.config:
        build_cmd.extend(['--config', args.config])
    print("Building project...")
    ret = subprocess.run(build_cmd)
    if ret.returncode != 0:
        print("Build failed.", file=sys.stderr)
        sys.exit(ret.returncode)

    print("Build succeeded.")
    sys.exit(0)

if __name__ == '__main__':
    main()
