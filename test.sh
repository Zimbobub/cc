#!/bin/bash
find tests/ -name *.c -exec make test TEST="{}" \;
