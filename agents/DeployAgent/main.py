#!/usr/bin/env python3

import argparse
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(
        description="DeployAgent: package and deploy builds"
    )
    parser.add_argument(
        '--build-dir', '-d',
        default='build',
        help='Build directory to package'
    )
    parser.add_argument(
        '--target', '-t',
        choices=['staging', 'production'],
        default='staging',
        help='Deployment target'
    )
    parser.add_argument(
        '--version', '-v',
        help='Version tag for deployment',
        default=None
    )
    args = parser.parse_args()

    # Example: create tarball and call deployment script
    tarball = f"{args.build_dir}.tar.gz"
    print(f"Packaging {args.build_dir} into {tarball}...")
    ret = subprocess.run(['tar', '-czf', tarball, '-C', args.build_dir, '.'])
    if ret.returncode != 0:
        print("Packaging failed.", file=sys.stderr)
        sys.exit(ret.returncode)

    print(f"Deploying {tarball} to {args.target}...")
    # Placeholder: call actual deploy script or API
    # ret = subprocess.run(['./scripts/deploy.sh', tarball, args.target, args.version])
    ret = 0

    if ret != 0:
        print("Deployment failed.", file=sys.stderr)
        sys.exit(ret)

    print("Deployment succeeded.")
    sys.exit(0)

if __name__ == '__main__':
    main()
