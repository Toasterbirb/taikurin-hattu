#!/bin/sh

# This script is meant to aid with creating exception json files
# that are used for PNG background removal when there are edge cases
#
# To use this script you need to have a valid exceptions.json file
# in your current directory. The json file should contain at-least
# an empty array like this: [ ]
#
# When you run the script, it'll ask your for a file path and x/y coords.
# The file path should be relative path from the asset root to the png file.
# The X and Y coordinates should point to a white pixel that should be
# flood filled with transparency

if [ ! -f ./exceptions.json ]
then
	echo "Couldn't find the exceptions file"
	exit 1
fi

read -e -p "File: " FILE
if [ ! -f "$FILE" ]
then
	echo "File doesn't exist"
	exit 1
fi

# Cleanup the file path
FILE="$(echo "$FILE" | sed 's|./output/||')"

read -p "x: " X_COORD
read -p "y: " Y_COORD

# Read the exceptions
EXCEPTIONS="$(cat ./exceptions.json)"

# Add the new value
echo "$EXCEPTIONS" | jq ". |= . + [{\"file\":\"$FILE\", \"x\": $X_COORD, \"y\": $Y_COORD}]" > ./exceptions.json
