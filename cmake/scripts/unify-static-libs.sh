#!/bin/bash
#
# Copyright 2024 Jamie Cui and *Another Author*
#
# From Jamie: I can't recall the exact github repo link, so please do contact me
# if you found the original author.
#
# Creates one static library from several.
#
# Usage: copy all your libs to a single directory and call this script.
#

set -e

if [[ $# -ne 3 ]]; then
  echo "Usage: unify-static-libs.sh <OUTLIB> <WORKING_DIR> <PATTERN>"
  exit 2
fi

if [[ -d $3 ]]; then
  echo "Error: Path ${3} does not exit!"
  exit 2
fi

# Inputs
OUTLIB=$1
WORKING_DIR=$2
PATTERN=$3

# Define commands
UNIFY_CMD="ar -crs $OUTLIB "
CLEAN_CMD="rm -rf "

cd "${WORKING_DIR}" || exit 2

for EACH_LIB in ${WORKING_DIR}/${PATTERN}; do
  echo "Extracting objects from $EACH_LIB to ${EACH_LIB%??}"
  mkdir -p "${EACH_LIB%??}"
  ar -x "$EACH_LIB" && mv -- *.o "${EACH_LIB%??}"
  UNIFY_CMD="$UNIFY_CMD ${EACH_LIB%??}/*.o"
  CLEAN_CMD="$CLEAN_CMD ${EACH_LIB%??}"
done

# Link objects into a single lib
echo "Creating $OUTLIB from extracted objects..."
$UNIFY_CMD

echo "Cleaning with $CLEAN_CMD"
$CLEAN_CMD
