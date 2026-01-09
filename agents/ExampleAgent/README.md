# ExampleAgent

ExampleAgent is a sample micro agent demonstrating best practices for development and integration within MetalOS.

## Requirements

- Python 3.6+

## Installation

No installation required. Ensure the script is executable:

```bash
chmod +x agents/ExampleAgent/main.py
```

## Usage

```bash
./agents/ExampleAgent/main.py [--message MESSAGE]
```

### Options

- `-m, --message MESSAGE`: Custom message to print. Default: "Hello from ExampleAgent!"

## Tests

Run pytest from the project root:

```bash
pytest agents/ExampleAgent/tests
```
