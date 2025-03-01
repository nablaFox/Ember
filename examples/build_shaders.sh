#!/bin/sh

set -e

output_dir="shaders"

compile_shaders() {
  search_dir="$1"
  out_subdir="$2"
  
  find "$search_dir" -type f \( -name "*.vert" -o -name "*.frag" \) | while IFS= read -r shader; do
    filename=$(basename "$shader")
    if [ -n "$out_subdir" ]; then
      dest="$output_dir/$out_subdir"
    else
      dest="$output_dir"
    fi
    mkdir -p "$dest"
    echo glslc "$shader" -o "$dest/$filename"
	glslc "$shader" -o "$dest/$filename".spv
  done
}

compile_shaders "../shaders" "ember"

compile_shaders "."
