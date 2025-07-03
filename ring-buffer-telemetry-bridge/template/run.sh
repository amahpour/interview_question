#!/usr/bin/env bash
set -e
gcc -std=c11 -pthread ringbuf.c main.c -o app
python3 check_stream.py | grep -q '^PASS$'
