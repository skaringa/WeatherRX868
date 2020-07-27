#!/bin/bash
g++ -o TestDecoder TestDecoder.cpp ../Decoder.cpp
for f in logs/*.log
do
  if ./TestDecoder "$f" ; then
    echo OK: $f
  else 
    echo Failed: $f
  fi
done
