# RoadmapAgent

RoadmapAgent allows viewing and updating the project roadmap (`docs/ROADMAP.md`).

## Requirements

- Python 3.6+

## Installation

No installation required. Ensure the script is executable:

```bash
chmod +x agents/RoadmapAgent/main.py
```

## Usage

```bash
./agents/RoadmapAgent/main.py (--view | --add ITEM)
```

### Options

- `-v, --view`: Display the current roadmap from `docs/ROADMAP.md`.
- `-a, --add ITEM`: Append a new item to the roadmap.

## Examples

View the roadmap:
```bash
./agents/RoadmapAgent/main.py --view
```

Add a new roadmap entry:
```bash
./agents/RoadmapAgent/main.py --add "Support multi-arch builds"
```
