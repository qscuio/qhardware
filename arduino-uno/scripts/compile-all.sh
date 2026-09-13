#!/usr/bin/env sh
set -eu

build_cache=${1:-/tmp/qhardware-arduino-cache}
fqbn=${FQBN:-arduino:avr:uno}
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
exercise_root=$(dirname "$script_dir")/exercises

command -v arduino-cli >/dev/null 2>&1 || {
    echo 'arduino-cli was not found on PATH' >&2
    exit 1
}

mkdir -p "$build_cache"
count=0
for sketch in "$exercise_root"/*/src/*.ino; do
    [ -f "$sketch" ] || continue
    source_dir=$(dirname "$sketch")
    exercise_dir=$(dirname "$source_dir")
    name=$(basename "$exercise_dir")
    staging="$build_cache/staging/$name"
    output="$build_cache/build/$name"
    mkdir -p "$staging" "$output"
    cp "$sketch" "$staging/$name.ino"
    echo "Compiling $name"
    arduino-cli compile --fqbn "$fqbn" --build-path "$output" "$staging"
    count=$((count + 1))
done

[ "$count" -gt 0 ] || {
    echo "No sketches found under $exercise_root" >&2
    exit 1
}
echo "Compiled $count sketches."
