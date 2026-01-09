# DeployAgent

DeployAgent packages and deploys project builds.

## Requirements

- Python 3.6+
- tar on PATH
- Deployment script or API setup (optional)

## Usage

```bash
./agents/DeployAgent/main.py [--build-dir DIR] [--target TARGET] [--version VERSION]
```

### Options

- `-d, --build-dir DIR`: Build directory (default: `build`).
- `-t, --target TARGET`: Deployment target (`staging` or `production`, default: `staging`).
- `-v, --version VERSION`: Version tag for deployment.

## Examples

Package and deploy to staging:
```bash
./agents/DeployAgent/main.py
```

Specify build directory and production target:
```bash
./agents/DeployAgent/main.py -d out -t production -v v1.0.0
```
