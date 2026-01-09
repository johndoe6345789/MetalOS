#!/usr/bin/env python3

import argparse
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(
        description="DocGenAgent: generate and validate documentation"
    )
    parser.add_argument(
        '--output-dir', '-o',
        default='docs/_build',
        help='Output directory for generated docs'
    )
    args = parser.parse_args()

    # Generate docs (e.g., Sphinx)
    print(f"Building docs in {args.output_dir}...")
    ret = subprocess.run(['sphinx-build', '-b', 'html', 'docs', args.output_dir])
    if ret.returncode != 0:
        print("Documentation build failed.", file=sys.stderr)
        sys.exit(ret.returncode)

    # Optionally validate
    # ret = subprocess.run(['sphinx-build', '-b', 'linkcheck', 'docs', args.output_dir])
    # if ret.returncode != 0:
    #     print("Documentation validation failed.", file=sys.stderr)
    #     sys.exit(ret.returncode)

    print("Documentation generation complete.")
    sys.exit(0)

if __name__ == '__main__':
    main()
