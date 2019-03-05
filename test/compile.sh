#!/bin/bash

if [[ "$OSTYPE" == "win32" ]]; then
      g++ main.cpp -I../ -o load
      g++ -shared -fPIC test.cpp -o libtest.dll
else
      g++ main.cpp -I../ -o load -ldl
      g++ -shared -fPIC test.cpp -o libtest.so
fi