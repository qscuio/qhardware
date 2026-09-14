#!/usr/bin/env sh
set -eu

if [ "$#" -ne 2 ]; then
  echo "usage: $0 CHUNK_DIRECTORY OUTPUT_PACKAGE" >&2
  exit 2
fi

CHUNK_DIR=$1
OUTPUT=$2
test -d "$CHUNK_DIR"
test -f "$CHUNK_DIR/package.sha256"
if [ -e "$OUTPUT" ]; then
  echo "refusing to overwrite: $OUTPUT" >&2
  exit 1
fi

set -- "$CHUNK_DIR"/part-*
test -f "$1"
LC_ALL=C cat "$@" > "$OUTPUT"
EXPECTED=$(cut -d ' ' -f 1 "$CHUNK_DIR/package.sha256")
ACTUAL=$(sha256sum "$OUTPUT" | cut -d ' ' -f 1)
if [ "$EXPECTED" != "$ACTUAL" ]; then
  echo "SHA-256 mismatch" >&2
  rm -f "$OUTPUT"
  exit 1
fi
echo "SHA-256 verified: $ACTUAL"
