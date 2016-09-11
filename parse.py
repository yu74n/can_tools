#!/usr/bin/env python
# -*- coding: utf-8 -*-

def get_input():
    while True:
        try:
            yield raw_input().split()
        except EOFError:
            break


def parse(i):
    result = []
    for l in i:
        if len(l) == 0:
            continue
        frame = {}
        frame["type"] = l[0][0]
        if frame["type"] == '0':
            frame["size"] = int(l[0][1], 16)
            frame["payload"] = l[1:1+frame["size"]]
        else:
            size = int(l[0][1], 16) + int(l[1], 16)
        result.append(frame)
    return result

def find_seed_and_key(parsed):
    result = []
    for p in parsed:
        size = p["size"]
        sid = p["payload"][0]
        sub = p["payload"][1]
        if sid == "27":
            if sub == "02":
                key = p["payload"][2:2+size-2]
        if sid == "67":
            if sub == "01":
                seed = p["payload"][2:2+size-2]
        if sid == "67" and sub == "02":
            r = {}
            r["seed"] = seed
            r["key"] = key
            result.append(r)
    return result

parsed = parse(list(get_input()))
print find_seed_and_key(parsed)

