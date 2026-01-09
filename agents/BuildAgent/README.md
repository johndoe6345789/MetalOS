# BuildAgent

BuildAgent automates the CMake-based build process for the MetalOS project.

## Requirements

- Python 3.6+
- CMake installed and available on PATH

## Installation

No installation required. Ensure the script is executable:

```bash
chmod +x agents/BuildAgent/main.py
```

## Usage

```bash
./agents/BuildAgent/main.py [options]
```

### Options

- `-d, --build-dir DIR`: Build directory (default: `build`)
- `-c, --config CONFIG`: CMake configuration (Debug, Release)
- `--clean`: Clean the build directory before building

## Examples

Configure and build in `build/`:
```bash
./agents/BuildAgent/main.py
```

Clean, configure, and build in `out/` with Release config:
```bash
./agents/BuildAgent/main.py -d out -c Release --clean
```

## Tests

No tests currently. Ensure you can run the script and invoke CMake successfully.
