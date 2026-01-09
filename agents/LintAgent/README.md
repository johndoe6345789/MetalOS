# LintAgent

LintAgent runs static analysis tools and formatters on the MetalOS codebase.

## Requirements

- Python 3.6+
- flake8, clang-tidy, black installed and on PATH

## Usage

```bash
./agents/LintAgent/main.py [--fix]
```

### Options

- `--fix`: Automatically fix formatting issues with `black`.

## Examples

Run linters only:
```bash
./agents/LintAgent/main.py
```

Run linters and auto-format:
```bash
./agents/LintAgent/main.py --fix
```
