
Copy code
#!/bin/bash
# Works only in format:
# /*
# Commentary here
# */


# Check if a file name was provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <source_file.c>"
    exit 1
fi

# Input C file
input_file="$1"

# Output file (uncommented version)
output_file="${input_file%.c}_uncommented.c"

# Use sed to remove comments that span multiple lines
# and then remove any trailing comments on lines.
# This simple approach might not handle all cases, especially
# comments within string literals or comments spread in a complex way.
sed '/\/\*/,/\*\//d' "$input_file" | sed 's/\/\*.*\*\///g' > "$output_file"

echo "Comments removed. Output is in: $output_file"