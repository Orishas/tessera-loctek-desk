#!/bin/sh
# Host tests of the model and the icons' resampling (no LVGL, no Tessera): sh tests/run.sh
set -e
cd "$(dirname "$0")/.."
c++ -std=c++17 -Wall -Werror -I components/loctek_desk tests/test_model.cpp tests/test_icon.cpp \
  components/loctek_desk/model.cpp components/loctek_desk/icon.cpp -o tests/run
tests/run
