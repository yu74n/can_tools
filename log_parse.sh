#!/bin/bash
grep -v "\*\*\*" $1 | cut -d' ' -f 7- | ./parse.py
