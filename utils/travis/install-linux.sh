#!/usr/bin/env bash

set -e -x

sudo apt-get install libqt4-dev libxtst-dev openbox

# Coveralls
pip install --user 'urllib3[secure]' cpp-coveralls
