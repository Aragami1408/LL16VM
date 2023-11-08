#!/bin/sh
#
set -xe

CFLAGS="-Wall -Wextra"

clang $CFLAGS -g -o assembler assembler.c instruction_parser.c instruction_evaluator.c utils.c mpc.c -ledit
