#!/usr/bin/env python
# -*- coding: utf-8 -*-

PARSER_STATUS = {"INIT": -1 , "SINGLE": 0, "FIRST": 1, "CONSECUTIVE": 2, "FLOW": 3, "DONE": 99}

class SinglePayload():
    def __init__(self):
        self._size = 0
        self._payload = []

    def extract_size(self, frame):
        self._size = int(frame[0][1], 16) 

    def extract_payload(self, frame):
        self._payload.extend(frame[1:1+self._size])

    def get_size(self):
        return self._size

    def get_payload(self):
        return self._payload

class SegmentedPayload():
    def __init__(self):
        self._size = 0
        self._position = 0
        self._payload = []
        self._segment = {}
        self.is_done = False

    def extract_size(self, frame):
        self._size = int(frame[0][1], 16) + int(frame[1], 16)

    def extract_first_payload(self, frame):
        s = 0
        if self._size > 6:
            s = 6
        else:
            s = self._size
            self.is_done = True

        self._payload.extend(frame[2:2+s])
        self._position += s

    def extract_consective_payload(self, frame):
        s = 0
        if self._size > self._position + 7:
            s = 7
        else:
            s = self._size - self._position
            self.is_done = True

        self._segment[int(frame[0][1], 16)] = frame[1:1+s]
        self._position += s

    def get_size(self):
        return self._size

    def get_payload(self):
        l = len(self._segment)
        if l == 0:
            return
        for i in range(1, l+1):
            self._payload.extend(self._segment[i])

        return self._payload

class Parser():
    def __init__(self, input):
        self._status = PARSER_STATUS["INIT"]
        self._input = input
        self._payload = None
        self._result = []

    def get_frame_type(self, frame):
        return int(frame[0][0])

    def set_uds_size(self, frame):
        self._size = int(frame[0][1], 16)

    def set_payload(self, frame):
        self._payload.extend(frame[1:1+self._size])

    def get_result(self):
        return self._result

    def parse(self):
        for l in self._input:
            if len(l) == 0:
                continue
            self._status = self.get_frame_type(l)
            if self._status == PARSER_STATUS["SINGLE"]:
                s = SinglePayload()
                s.extract_size(l)
                s.extract_payload(l)
                self._payload = s
                self._status = PARSER_STATUS["DONE"]
            elif self._status == PARSER_STATUS["FIRST"]:
                s = SegmentedPayload()
                s.extract_size(l)
                s.extract_first_payload(l)
                self._payload = s
                if s.is_done:
                    self._status = PARSER_STATUS["DONE"]
            elif self._status == PARSER_STATUS["CONSECUTIVE"]:
                self._payload.extract_consective_payload(l)
                if s.is_done:
                    self._status = PARSER_STATUS["DONE"]

            if self._status == PARSER_STATUS["DONE"]:
                self._result.append({"size": self._payload.get_size(), "payload": self._payload.get_payload()})
                self.payload = None
                self._status = PARSER_STATUS["INIT"]

def get_input():
    while True:
        try:
            yield raw_input().split()
        except EOFError:
            break

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

i = list(get_input()) 
parser = Parser(i)
parser.parse()
#print parser.get_result()
print find_seed_and_key(parser.get_result())
