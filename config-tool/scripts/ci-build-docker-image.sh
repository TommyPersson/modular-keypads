#!/usr/bin/env bash

set -euxo pipefail

source "${BASH_SOURCE%/*}/script-setup.sh"

docker build -t $IMAGE_NAME .