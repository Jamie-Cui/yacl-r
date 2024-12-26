#!/bin/bash
#
# Creates one static library from several.
#
# Usage: copy all your libs to a single directory and call this script.
#
if [[ $# -ne 3 ]]; then
  echo "Usage: unify-static-libs.sh <OUTLIB> <WORKING_DIR> <PATTERN>"
  exit 2
fi

# Inputs
OUTLIB=$1
WORKING_DIR=$2
PATTERN=$3

UNIFY_CMD="ar -crs $OUTLIB "
CLEAN_CMD="rm -rf "

cd $WORKING_DIR

for EACH_LIB in ${WORKING_DIR}/${PATTERN}
do
    echo "Extracting objects from $EACH_LIB to ${EACH_LIB%??}"
    mkdir -p ${EACH_LIB%??}
    ar -x $EACH_LIB && mv *.o ${EACH_LIB%??}
    UNIFY_CMD="$UNIFY_CMD ${EACH_LIB%??}/*.o"
    CLEAN_CMD="$CLEAN_CMD ${EACH_LIB%??}"
done

# Link objects into a single lib
echo "Creating $OUTLIB from objects..."
$UNIFY_CMD

echo "Cleaning with $CLEAN_CMD"
$CLEAN_CMD

