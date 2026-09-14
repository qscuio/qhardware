#!/usr/bin/env sh
set -eu

if [ "$#" -ne 1 ]; then
  echo "usage: $0 /path/to/esp-idf" >&2
  exit 2
fi

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
IDF_DIR=$1
test -f "$IDF_DIR/export.sh"
. "$IDF_DIR/export.sh"
cd "$PROJECT_DIR"
idf.py set-target esp32s3
idf.py build
