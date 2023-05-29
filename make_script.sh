#!/bin/bash

build_test () {
    cd ./build
    cmake ..
    make 
    make test 
    if [ "$?" != 0 ]
    then 
        echo "build failed"
        exit 1
    fi

    echo "build_test was successful"
    cd ..
}

build_coverage_test() {
    cd ./build_coverage 
    cmake -DCMAKE_BUILD_TYPE=Coverage ..
    make coverage
    if [ "$?" != 0 ]
    then
        echo "build_coverage failed"
        exit 1
    fi

    echo "build_coverage_test was successful"
    cd ..
}

help_func() {
    printf " flags:\n -t: Runs the build make test\n -c: Runs the build_coverage make test\n -h: Returns information about flags\n"
}

while getopts 'tch' flag; do
    case "${flag}" in
        t) build_test ;;
        c) build_coverage_test ;;
        h) help_func ;;
    esac
done


exit 0