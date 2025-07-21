#!/bin/bash
cmake --build build
for test in $(ls ./build/test_*); do
    echo "run test" ${test}
    ${test}
done
