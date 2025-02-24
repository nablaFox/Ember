#!/bin/sh

shaders_dir="shaders"
shaders_files=$(ls $shaders_dir)
output_dir="build/$shaders_dir"

shaders_files=$(echo $shaders_files | tr " " "\n" | grep -E ".(vert|frag)")

mkdir -p $output_dir

for shader in $shaders_files
do
	glslc $shaders_dir/$shader -o $output_dir/$shader.spv
done

