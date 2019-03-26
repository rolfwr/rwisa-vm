#! /usr/bin/env bash
clang++-7 -g -O0 -std=c++17 -Wall -fsanitize=integer -fsanitize=address -fsanitize-address-use-after-scope -fsanitize=undefined -o vm vm.cpp
