#!/usr/bin/env bash

set -euxo pipefail

export CI="true"
export IMAGE_NAME="ghcr.io/tommypersson/mkp-config-tool"

VERSION_STR=${1-v0}
export VERSION=${VERSION_STR#"v"}