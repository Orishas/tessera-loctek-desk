#!/bin/sh
# Host tests of the model (no LVGL, no Tessera): sh tests/run.sh
set -e
cd "$(dirname "$0")/.."
c++ -std=c++17 -Wall -Werror -I components/loctek_desk tests/test_model.cpp components/loctek_desk/model.cpp -o tests/run
tests/run
