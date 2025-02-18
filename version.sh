#!/usr/bin/env bash

# File: version.sh
# Project: Darkroom Enlarger Timer
# File Created: Monday, 17th February 2025 12:58:56 pm
# Author: Andrei Grichine (andrei.grichine@gmail.com)
# -----
# Last Modified: Tuesday, 18th February 2025 6:08:12 am
# Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
# -----
# Copyright: 2019 - 2025. Prime73 Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
# -----
# HISTORY:

# Ensure the script stops on errors
set -e

# Ensure CONFIGURATION has a default value (Release)
CONFIGURATION=${CONFIGURATION:-Release}

BUILD_VERSION=${BUILD_VERSION:-"2.0"}

# Paths
CONSTANTS_FILE="src/constants.h"

# Locate the git binary
# Specify git binary manually if not found
git=$(command -v git)

if [ -z "$git" ]; then
    git="/usr/bin/git" # Update this path if needed
fi

# Ensure git is working
if [ ! -x "$git" ]; then
    echo "Error: Git binary not found or not executable. We rely on git to properly handle project versioning. Make sure you have git installed."
    exit 1
fi

# Get total commit count for the build number
appBuild=$("$git" rev-list --all --count)

# Check for debug configuration
if [ "$CONFIGURATION" = "Debug" ]; then
    branchName=$("$git" rev-parse --abbrev-ref HEAD)
    REVISION_NUMBER=$(echo "$appBuild-$branchName" | sed 's/^[ \t]*//')
else
    REVISION_NUMBER=$appBuild
fi

# Prepare updated content for constants.h
buildVersion="const uint8_t BUILD_VERSION PROGMEM ="
revisionVersion="const uint8_t REVISION_NUMBER PROGMEM ="

if [ ! -f "$CONSTANTS_FILE" ]; then
    echo "Error: $CONSTANTS_FILE not found!"
    exit 1
fi
# Update constants.h

# Update existing BUILD_VERSION and REVISION_NUMBER definitions
sed -i.bak "s/\($buildVersion\)[^;]*;/\1$BUILD_VERSION;/" "$CONSTANTS_FILE"
if [ $? -ne 0 ]; then
    echo "Error: Failed to update BUILD_VERSION in $CONSTANTS_FILE"
    exit 1
fi
sed -i.bak "s/\($revisionVersion\)[^;]*;/\1$REVISION_NUMBER;/" "$CONSTANTS_FILE"
if [ $? -ne 0 ]; then
    echo "Error: Failed to update REVISION_NUMBER in $CONSTANTS_FILE"
    exit 1
fi

# Remove the backup file if the operations were successful
rm -f "$CONSTANTS_FILE.bak"
if [ $? -ne 0 ]; then
    echo "Error: Failed to remove backup file: $CONSTANTS_FILE.bak"
    exit 1
fi
echo "Build and revision updated successfully. Backup file deleted."

echo "Updated $CONSTANTS_FILE with:"
echo "  $buildVersion $BUILD_VERSION"
echo "  $revisionVersion $REVISION_NUMBER"
exit 0
