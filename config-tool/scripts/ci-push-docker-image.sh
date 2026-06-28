#!/usr/bin/env bash

set -euxo pipefail

source "${BASH_SOURCE%/*}/script-setup.sh"

docker tag $IMAGE_NAME $IMAGE_NAME:$VERSION
docker tag $IMAGE_NAME:$VERSION $IMAGE_NAME:latest

docker push $IMAGE_NAME:$VERSION
docker push $IMAGE_NAME:latest

