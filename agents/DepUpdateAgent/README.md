# DepUpdateAgent

DepUpdateAgent automates updating of project dependencies.

## Requirements

- Python 3.6+
- pip, conan, npm installed and on PATH

## Usage

```bash
./agents/DepUpdateAgent/main.py [--all]
```

### Options

- `-a, --all`: Run all update routines (pip, conan, npm).

## Examples

Update Python dependencies:
```bash
./agents/DepUpdateAgent/main.py -a
```
