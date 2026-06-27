#!/usr/bin/env bash

set -euxo pipefail

source "${BASH_SOURCE%/*}/script-setup.sh"

npm ci
npm run typecheck
npm run build
