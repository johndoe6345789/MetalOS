#!/bin/bash
# Build MetalOS Docker image with all dependencies

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
METALOS_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

IMAGE_NAME="metalos-builder"
IMAGE_TAG="latest"

echo "=== Building MetalOS Docker Image ==="
echo "Image: $IMAGE_NAME:$IMAGE_TAG"
echo "Context: $METALOS_ROOT"
echo ""

cd "$METALOS_ROOT"

# Build the Docker image
docker build \
    -t "$IMAGE_NAME:$IMAGE_TAG" \
    -f Dockerfile \
    .

echo ""
echo "✓ Docker image built successfully: $IMAGE_NAME:$IMAGE_TAG"
echo ""
echo "Next steps:"
echo "  1. Run setup script: ./scripts/docker-run.sh setup-deps.sh"
echo "  2. Build MetalOS: ./scripts/docker-run.sh make all"
echo "  3. Test in QEMU: ./scripts/docker-run.sh make qemu"
