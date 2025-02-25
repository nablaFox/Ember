#!/bin/sh

shaders_dir="shaders"
output_dir="build"
shaders_files=$(find $shaders_dir -type f -name "*.vert" -o -name "*.frag")

for shader in $shaders_files
do
	mkdir -p $output_dir/$(dirname $shader)
	glslc $shader -o $output_dir/$shader.spv
	echo glslc $shader -o $output_dir/$shader.spv
done

