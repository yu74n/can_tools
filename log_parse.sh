#!/bin/bash
grep -v "\*\*\*" $1 | cut -d' ' -f 4- | ./parse.py
