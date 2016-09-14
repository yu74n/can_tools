#!/usr/bin/env python
# -*- coding: utf-8 -*-

PARSER_STATUS = {"INIT": -1 , "SINGLE": 0, "FIRST": 1, "CONSECUTIVE": 2, "FLOW": 3, "DONE": 99}
OFFSET_IDENTIFIER = 0
OFFSET_TYPE = 3

OFFSET_SINGLE_SIZE = 3
OFFSET_SINGLE_SID = 4
OFFSET_SINGLE_SUBFUNC = 5
OFFSET_SINGLE_PAYLOAD = 6

OFFSET_FIRST_SIZE = 4
OFFSET_FIRST_SID = 5
OFFSET_FIRST_SUBFUNC = 6
OFFSET_FIRST_PAYLOAD = 7

OFFSET_CONSECUTIVE_SEQ = 3
OFFSET_CONSECUTIVE_PAYLOAD = 4

class SinglePayload():
    def __init__(self):
        self._id = ""
        self._sid = ""
        self._subfunc = ""
        self._size = 0
        self._payload = []

    def extract_id(self, frame):
        self._id = frame[OFFSET_IDENTIFIER]

    def extract_sid(self, frame):
        self._sid = frame[OFFSET_SINGLE_SID] 

    def extract_subfunc(self, frame):
        self._subfunc = frame[OFFSET_SINGLE_SUBFUNC]

    def extract_size(self, frame):
        self._size = int(frame[OFFSET_SINGLE_SIZE][1], 16) 

    def extract_payload(self, frame):
        self._payload.extend(frame[OFFSET_SINGLE_PAYLOAD:OFFSET_SINGLE_PAYLOAD+self._size-2])

    def get_id(self):
        return self._id

    def get_sid(self):
        return self._sid

    def get_subfunc(self):
        return self._subfunc

    def get_size(self):
        return self._size

    def get_payload(self):
        return self._payload

class SegmentedPayload():
    def __init__(self):
        self._id = ""
        self._sid = ""
        self._subfunc = ""
        self._size = 0
        self._position = 0
        self._payload = []
        self._segment = {}
        self.is_done = False

    def extract_id(self, frame):
        self._id = frame[OFFSET_IDENTIFIER]

    def extract_sid(self, frame):
        self._sid = frame[OFFSET_FIRST_SID] 

    def extract_subfunc(self, frame):
        self._subfunc = frame[OFFSET_FIRST_SUBFUNC]

    def extract_size(self, frame):
        self._size = int(frame[OFFSET_SINGLE_SIZE][1], 16) + int(frame[OFFSET_FIRST_SIZE], 16)

    def extract_first_payload(self, frame):
        s = 0
        if self._size > 6:
            s = 6-2
        else:
            s = self._size
            self.is_done = True

        self._payload.extend(frame[OFFSET_FIRST_PAYLOAD:OFFSET_FIRST_PAYLOAD+s])
        self._position += s

    def extract_consective_payload(self, frame):
        s = 0
        if self._size > self._position + 7:
            s = 7
        else:
            s = self._size - self._position
            self.is_done = True

        self._segment[int(frame[OFFSET_CONSECUTIVE_SEQ][1], 16)] = frame[OFFSET_CONSECUTIVE_PAYLOAD:OFFSET_CONSECUTIVE_PAYLOAD+s]
        self._position += s

    def get_id(self):
        return self._id

    def get_sid(self):
        return self._sid

    def get_subfunc(self):
        return self._subfunc

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
        return int(frame[OFFSET_TYPE][0])

    def get_result(self):
        return self._result

    def parse(self):
        for l in self._input:
            if len(l) == 0:
                continue
            self._status = self.get_frame_type(l)
            if self._status == PARSER_STATUS["SINGLE"]:
                s = SinglePayload()
                s.extract_id(l)
                s.extract_sid(l)
                s.extract_subfunc(l)
                s.extract_size(l)
                s.extract_payload(l)
                self._payload = s
                self._status = PARSER_STATUS["DONE"]
            elif self._status == PARSER_STATUS["FIRST"]:
                s = SegmentedPayload()
                s.extract_id(l)
                s.extract_sid(l)
                s.extract_subfunc(l)
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
                self._result.append({"id": self._payload.get_id(), "sid": self._payload.get_sid(), "subfunc": self._payload.get_subfunc(), "size": self._payload.get_size(), "payload": self._payload.get_payload()})
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
        sid = p["sid"]
        sub = p["subfunc"]
        if sid == "27":
            if sub == "02":
                key = p["payload"][0:size-2]
        if sid == "67":
            if sub == "01":
                seed = p["payload"][0:size-2]
        if sid == "67" and sub == "02":
            r = {}
            r["seed"] = seed
            r["key"] = key
            result.append(r)
    return result

i = list(get_input()) 
parser = Parser(i)
parser.parse()
print parser.get_result()
#print find_seed_and_key(parser.get_result())
