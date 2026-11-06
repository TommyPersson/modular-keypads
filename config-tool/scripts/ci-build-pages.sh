#!/usr/bin/env bash

set -euxo pipefail

source "${BASH_SOURCE%/*}/script-setup.sh"

# Served from https://tommypersson.github.io/mkp-config-tool/
export CONFIG_TOOL_BASE_PATH="/mkp-config-tool/"

npm ci
npm run typecheck
npm run build

# GitHub Pages has no SPA fallback; it serves 404.html for unknown paths, so let it load the app
cp dist/index.html dist/404.html
