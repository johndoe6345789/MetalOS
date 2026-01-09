#!/usr/bin/env python3

import argparse
import sys
from pathlib import Path
import re

def parse_roadmap(path):
    lines = path.read_text().splitlines(keepends=True)
    phases = []
    for i, line in enumerate(lines):
        m = re.match(r"^### Phase\s+(\d+):\s*(.+)", line)
        if m:
            phases.append((m.group(1), m.group(2).strip(), i))
    return lines, phases


def list_phases(phases):
    print("Available Phases:")
    for num, title, _ in phases:
        print(f"Phase {num}: {title}")


def view_phase(lines, phases, phase_num):
    for idx, (num, title, start) in enumerate(phases):
        if num == phase_num:
            end = phases[idx+1][2] if idx+1 < len(phases) else len(lines)
            print(''.join(lines[start:end]).rstrip())
            return
    print(f"Phase {phase_num} not found.", file=sys.stderr)
    sys.exit(1)


def add_item(lines, phases, phase_num, item):
    for idx, (num, title, start) in enumerate(phases):
        if num == phase_num:
            end = phases[idx+1][2] if idx+1 < len(phases) else len(lines)
            # find insertion point before end, after last task line
            insert = end
            for j in range(start+1, end):
                if lines[j].startswith('###'): break
                if re.match(r"^\s*- \[.\]", lines[j]):
                    insert = j+1
            lines.insert(insert, f"- [ ] {item}\n")
            return lines
    print(f"Phase {phase_num} not found.", file=sys.stderr)
    sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        description="RoadmapAgent: manage docs/ROADMAP.md"
    )
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--list-phases', action='store_true', help='List all roadmap phases')
    group.add_argument('--view-phase', metavar='NUM', help='View tasks for a specific phase')
    group.add_argument('--add-item', nargs=2, metavar=('NUM', 'ITEM'), help='Add a new incomplete task to a phase')
    args = parser.parse_args()

    roadmap_path = Path(__file__).resolve().parents[2] / 'docs' / 'ROADMAP.md'
    if not roadmap_path.exists():
        print(f"Roadmap file not found at {roadmap_path}", file=sys.stderr)
        sys.exit(1)

    lines, phases = parse_roadmap(roadmap_path)

    if args.list_phases:
        list_phases(phases)
        sys.exit(0)

    if args.view_phase:
        view_phase(lines, phases, args.view_phase)
        sys.exit(0)

    if args.add_item:
        num, item = args.add_item
        new_lines = add_item(lines, phases, num, item)
        try:
            roadmap_path.write_text(''.join(new_lines))
            print(f"Added item to Phase {num}: {item}")
            sys.exit(0)
        except Exception as e:
            print(f"Failed to update roadmap: {e}", file=sys.stderr)
            sys.exit(1)

if __name__ == '__main__':
    main()
