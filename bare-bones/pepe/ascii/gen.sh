#!/bin/bash
output_file="pepe.h"

echo "#ifndef PEPE_H" >> "$output_file"
echo "#define PEPE_H" >> "$output_file"
echo "" >> "$output_file"

frames=()

for file in frame_*.txt; do
    var_name=$(basename "$file" .txt)
    frames+=("$var_name")

    echo "static const char ${var_name}[] = " >> "$output_file"

    while IFS= read -r line; do
        safe_line=${line//\"/\\\"}
        echo "    \"$safe_line\\n\"" >> "$output_file"
    done < "$file"

    echo ";" >> "$output_file"
    echo "" >> "$output_file"
done

echo "static const int FRAMES_COUNT = ${#frames[@]};" >> "$output_file"
echo "static const char* frames[] = {" >> "$output_file"
for f in "${frames[@]}"; do
    echo "    ${f}," >> "$output_file"
done

echo "};" >> "$output_file"
echo "" >> "$output_file"
echo "#endif // PEPE_H" >> "$output_file"
