#!/bin/bash

# The active directory should always be '(...)/fa/changelog/snippets'

if ! ls *.md >/dev/null 2>&1; then
    echo "No Markdown files found to combine."
    exit 0
fi

# Combine the changelog snippets into one large changelog

output_file="combined.md"

echo "Writing output to: $output_file"

rm -f "$output_file"

cat "template/template.md" >> "$output_file"
echo "" >> "$output_file"

for file in *.md; do
    echo " - Processing: $file"
    cat "$file" >> "$output_file"
    echo "" >> "$output_file"
done

echo ""
echo "" >> "$output_file"

