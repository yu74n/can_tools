/* This file is generated by BUSMASTER */
/* VERSION [1.2] */
/* BUSMASTER VERSION [3.0.0] */
/* PROTOCOL [CAN] */

/* Start BUSMASTER include header */
#include <Windows.h>
#include <CANIncludes.h>
#include <map>
typedef unsigned int ID;
/* End BUSMASTER include header */


/* Start BUSMASTER global variable */
using namespace std;

typedef struct _data_info {
	unsigned long time;
	unsigned char data[64];
	unsigned char data_len;
	struct _data_info* next;
} data_info;

typedef struct {
	unsigned int size;
	data_info* data;
} data_array;
map<ID, data_array> m;
/* End BUSMASTER global variable */


/* Start BUSMASTER Function Prototype  */
GCC_EXTERN void GCC_EXPORT OnMsg_All(STCAN_MSG RxMsg);
void find(unsigned char* data, unsigned char* sub, int len, unsigned int id);
/* End BUSMASTER Function Prototype  */

/* Start BUSMASTER Function Wrapper Prototype  */
/* End BUSMASTER Function Wrapper Prototype  */


/* Start BUSMASTER generated function - OnMsg_All */
void OnMsg_All(STCAN_MSG RxMsg)
{
	if (m.find(RxMsg.id) == m.end()) {
		Trace("%x is not found", RxMsg.id);
		data_array* d = (data_array*) malloc(sizeof(data_array));
		d->data = (data_info*) malloc(sizeof(data_info));
		memcpy(d->data->data, RxMsg.data, RxMsg.dlc);
		d->data->data_len = RxMsg.dlc;
		d->data->next = NULL;
		d->data->time = RxMsg.timeStamp;
		d->size = 1;
		m.insert(make_pair(RxMsg.id, *d));
	} else {
		data_array d = m[RxMsg.id];
		data_info* cur = (data_info*) malloc(sizeof(data_info));
		memcpy(cur->data, RxMsg.data, RxMsg.dlc);
		cur->data_len = RxMsg.dlc;
		cur->next = NULL;
		cur->time = RxMsg.timeStamp;
		data_info* c = d.data;
		while (c->next != NULL) {
			c = c->next;
		}
		c->next = cur;
		d.size++;
		m[RxMsg.id] = d;
	}
	unsigned char old[8] = {};
	unsigned char sub[8] = {};
	unsigned long old_time = 0;
	unsigned int n = 0;
	data_array d = m[RxMsg.id];
	data_info* cur = d.data;
	while (cur->next != NULL) {
		int j = 1;
		char* s = NULL;
		char* t = NULL;
		unsigned long sub_time = cur->time - old_time;
		old_time = cur->time;
		if (cur->data_len < 1) {
			goto next;
		}
		s = (char*) malloc(cur->data_len * 4);
		t = (char*) malloc(cur->data_len * 4);
		sprintf(s, "%02X", cur->data[0]);
		sub[0] = cur->data[0] - old[0];
		sprintf(t, "%02X", sub[0]);
		old[0] = cur->data[0];
		
		while (j < cur->data_len) {
			unsigned char c = cur->data[j];
			sub[j] = c - old[j];
			sprintf(s, "%s %02X", s, c);
			sprintf(t, "%s %02X", t, sub[j]);
			old[j] = c;
			j++;
		}
		Trace("id: %X data: %s sub: %s time: %u", RxMsg.id, s, t, sub_time);
		if (1 < n && n < 4) {
			find(cur->data, sub, cur->data_len, RxMsg.id);
		}
		free(s);
		free(t);
next:
		cur = cur->next;
		n++;
	}
	
}/* End BUSMASTER generated function - OnMsg_All */

void find(unsigned char* data, unsigned char* sub, int len, unsigned int id) {
	map<unsigned char, int> m;
	for (int i = 0; i < len; i++) {
		if (sub[i] != 0) {
			map<unsigned char, int>::iterator it;
			if ((it = m.find(sub[i])) != m.end()) {
				Trace("id:%X %dth byte(%02X) or %dth byte(%02X) may be checksum", id, it->second, data[it->second], i, data[i]);
			}
			m.insert(make_pair(sub[i], i));
		}
		if (sub[i] == 1) {
			Trace("id:%X %dth byte(%02X) may be counter", id, i, data[i]);
		}
	}
}
