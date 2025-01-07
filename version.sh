#!/usr/bin/env bash

# File: version.sh
# Project: Darkroom Enlarger Timer
# File Created: Tuesday, 7th January 2025 1:35:06 pm
# Author: Andrei Grichine (andrei.grichine@gmail.com)
# -----
# Last Modified: Tuesday, 7th January 2025 1:50:25 pm
# Modified By: Andrei Grichine (andrei.grichine@gmail.com>)
# -----
# Copyright 2019 - 2025, Prime73 Inc. MIT License
#
# Copyright (c) 2025 Prime73 Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
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
git=$(which git)

# Get total commit count for the build number
appBuild=$("$git" rev-list --all --count)

# Check for debug configuration
if [ "$CONFIGURATION" = "Debug" ]; then
    branchName=$("$git" rev-parse --abbrev-ref HEAD)
    revisionNumber=$(echo "$appBuild-$branchName" | sed 's/^[ \t]*//')
else
    revisionNumber=$appBuild
fi

# Prepare updated content for constants.h
buildVersion="#define BUILD_VERSION $BUILD_VERSION"
revisionNumberDef="#define REVISION_NUMBER $revisionNumber"

# Update constants.h
if [ -f "$CONSTANTS_FILE" ]; then
    # Remove existing BUILD_VERSION and REVISION_NUMBER definitions
    sed -i.bak '/#define BUILD_VERSION/d' "$CONSTANTS_FILE"
    if [ $? -eq 0 ]; then
        sed -i.bak '/#define REVISION_NUMBER/d' "$CONSTANTS_FILE"
        if [ $? -eq 0 ]; then
            # Remove the backup file if the operations were successful
            rm -f constants.bak
            echo "Build and revision updated successfully. Backup file deleted."
        else
            echo "Error updating revision in constants.h."
        fi
    else
        echo "Error updating build in constants.h."
    fi

    # Add new definitions
    echo "$buildVersion" >>"$CONSTANTS_FILE"
    echo "$revisionNumberDef" >>"$CONSTANTS_FILE"

    echo "Updated $CONSTANTS_FILE with:"
    echo "  $buildVersion"
    echo "  $revisionNumberDef"
else
    echo "Error: $CONSTANTS_FILE not found!"
    exit 1
fi
