#!/bin/bash

set -xe

cc -o particles *.c -lraylib -lm
