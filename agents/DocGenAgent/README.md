# DocGenAgent

DocGenAgent generates and optionally validates project documentation using Sphinx.

## Requirements

- Python 3.6+
- Sphinx installed

## Usage

```bash
./agents/DocGenAgent/main.py [--output-dir DIR]
```

### Options

- `-o, --output-dir DIR`: Output directory for generated docs (default: `docs/_build`).

## Examples

Build docs:
```bash
./agents/DocGenAgent/main.py
```
