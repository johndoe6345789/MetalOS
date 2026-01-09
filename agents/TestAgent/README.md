# TestAgent

TestAgent runs the MetalOS project test suite using pytest.

## Requirements

- Python 3.6+
- pytest installed

## Installation

No installation required. Ensure the script is executable:

```bash
chmod +x agents/TestAgent/main.py
```

## Usage

```bash
./agents/TestAgent/main.py [--pattern PATTERN]
```

### Options

- `-p, --pattern PATTERN`: Run tests matching the given pattern.

## Examples

Run all tests:
```bash
./agents/TestAgent/main.py
```

Run tests matching "core":
```bash
./agents/TestAgent/main.py -p core
```
