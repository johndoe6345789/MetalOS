#!/bin/bash
# Run commands in MetalOS Docker container

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
METALOS_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

IMAGE_NAME="metalos-builder"
IMAGE_TAG="latest"

# Check if Docker image exists
if ! docker image inspect "$IMAGE_NAME:$IMAGE_TAG" >/dev/null 2>&1; then
    echo "Error: Docker image $IMAGE_NAME:$IMAGE_TAG not found"
    echo "Build it first with: ./scripts/docker-build.sh"
    exit 1
fi

# Run command in container
# Mount the MetalOS source directory as a volume
docker run --rm -it \
    -v "$METALOS_ROOT:/metalos" \
    -w /metalos \
    "$IMAGE_NAME:$IMAGE_TAG" \
    "$@"
