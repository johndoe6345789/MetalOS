#!/usr/bin/env python3

import argparse
import sys
from pathlib import Path

def main():
    parser = argparse.ArgumentParser(
        description="RoadmapAgent: view or update the project roadmap (docs/ROADMAP.md)"
    )
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument(
        '--view', '-v',
        action='store_true',
        help='Display the current roadmap'
    )
    group.add_argument(
        '--add', '-a',
        metavar='ITEM',
        help='Append a new item to the roadmap'
    )
    args = parser.parse_args()
    roadmap_path = Path(__file__).parent.parent / 'docs' / 'ROADMAP.md'

    if args.view:
        if not roadmap_path.exists():
            print(f"Roadmap file not found at {roadmap_path}", file=sys.stderr)
            sys.exit(1)
        print(roadmap_path.read_text())
        sys.exit(0)

    if args.add:
        # Append new roadmap item
        entry = f"- {args.add}\n"
        try:
            with open(roadmap_path, 'a') as f:
                f.write(entry)
            print(f"Added roadmap item: {args.add}")
            sys.exit(0)
        except Exception as e:
            print(f"Failed to update roadmap: {e}", file=sys.stderr)
            sys.exit(1)

if __name__ == '__main__':
    main()
