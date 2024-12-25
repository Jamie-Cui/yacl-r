#!/bin/bash
#
# Creates one static library from several.
#
# Usage: copy all your libs to a single directory and call this script.
#
if [[ $# -ne 3 ]]; then
  echo "Usage: unify-static-libs.sh output-name path-to-libs pattern"
  exit 2
fi

# Inputs
LIBNAME=$1
LIBSDIR=$2
PATTERN=$3

# Tmp dir
OBJDIR=/tmp/unify-static-libs
mkdir -p ${OBJDIR}
cd ${OBJDIR}
# Extract .o
echo "Extracting objects to ${OBJDIR}..."
for i in ${LIBSDIR}/${PATTERN}
do
    echo $i
    ar -x $i
done
# Link objects into a single lib
echo "Creating $LIBNAME from objects..."
ar -crs $LIBNAME $OBJDIR/*.o

mv $LIBNAME ${LIBSDIR}

# Clean up
rm -rf ${OBJDIR}
rm -rf ${LIBSDIR}/${PATTERN}

echo "Done."
