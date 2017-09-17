#!/bin/bash
clear
echo "Testing error code 1..."
for f in ./TestFiles/1/*.txt
do
  ./assemble $f output.obj
  echo "$?"
done
echo "Testing error code 2..."
for f in ./TestFiles/2/*.txt
do
  ./assemble $f output.obj
  echo "$?"
done
echo "Testing error code 3..."
for f in ./TestFiles/3/*.txt
do
  ./assemble $f output.obj
  echo "$?"
done
echo "Testing error code 4..."
for f in ./TestFiles/4/*.txt
do
  ./assemble $f output.obj
  echo "$?"
done
