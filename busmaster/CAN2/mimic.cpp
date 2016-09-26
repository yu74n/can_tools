/* This file is generated by BUSMASTER */
/* VERSION [1.2] */
/* BUSMASTER VERSION [3.0.0] */
/* PROTOCOL [CAN] */

/* Start BUSMASTER include header */
#include <Windows.h>
#include <CANIncludes.h>
#include <stdio.h>
#include <time.h>
/* End BUSMASTER include header */


/* Start BUSMASTER global variable */
typedef struct {
    unsigned int id;
    unsigned int size;
    // unsigned char data[8];
    unsigned char *data;
} frame;

frame reprogramed[100];
int cur;
/* End BUSMASTER global variable */


/* Start BUSMASTER Function Prototype  */
GCC_EXTERN void GCC_EXPORT OnBus_Connect();
GCC_EXTERN void GCC_EXPORT OnMsg_All(STCAN_MSG RxMsg);
void Utils_generate_msg(STCAN_MSG *msg);
GCC_EXTERN void GCC_EXPORT OnMsgID_7e8(STCAN_MSG RxMsg);
/* End BUSMASTER Function Prototype  */

/* Start BUSMASTER Function Wrapper Prototype  */
/* End BUSMASTER Function Wrapper Prototype  */


/* Start BUSMASTER generated function - OnBus_Connect */
void OnBus_Connect()
{
    FILE *fp = fopen("pwn2.log", "r");
    if (fp == NULL) {
        Trace("Error: File open %d", errno);
        return;
    }

    char buf[64];
    int num = 0;
    while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
        // Trace("%s", buf);
        char *p = strtok(buf, " ");
        if (p == NULL) {
            break;
        }

        frame *f = (frame *)malloc(sizeof(frame));
        if (f == NULL)
        {
            return;
        }

        f->id = strtol(p, NULL, 16);
        // Trace("ID: 0x%lX", f->id);
        for (int i = 0; i < 2; i++) {
            // skip unused data
            strtok(NULL, " ");
        }
        // Parse message type and message size
        p = strtok(NULL, " ");
        if (p == NULL) {
            break;
        }
        unsigned char b = strtol(p, NULL, 16);
        unsigned char type = (b >> 4) & 0x0F;
        // Trace("Type: %02X", type);
        
        switch (type) {
            case 0:
            f->size = b & 0x0F;
            break;
            case 1:
            case 2:
            case 3:
            continue;
            default:
            f->size = 0;
        }
        // Trace("0x%lX Type: %02X Size: %02X", f->id, type, f->size);
        if (f->size == 0) {
            Trace("Data size is zero");
            return;
        }
        unsigned int size = f->size + 1;
        unsigned char* d = (unsigned char*) malloc(size);
        if (d == NULL) {
            return;
        }
        unsigned int count = 0;
        d[count++] = f->size & 0xFF;
        while ((p = strtok(NULL, " ")) != NULL && count < size) {
            unsigned long l = strtol(p, NULL, 16);
            d[count] = l & 0x000000FF;
            // Trace("%02X", d[count]);
            count++;
        }
        f->data = d;
        memcpy(&reprogramed[num], f, sizeof(frame));
        num++;
        free(f);
        f = NULL;
        // Trace("Data: %02X %02X %02X %02X %02X %02X %02X %02X", f->data[0], f->data[1], f->data[2], f->data[3], f->data[4], f->data[5], f->data[6], f->data[7]);
    }
    fclose(fp);

    STCAN_MSG msg;
    cur = 0;
    Utils_generate_msg(&msg);
    char sent[128];
    memset(sent, 0, sizeof(sent));
    for (int i = 0; i < msg.dlc + 1; i++) {
        sprintf(sent, "%s %02X", sent, msg.data[i]);
    }
    // Trace("sent 0x%lX Size: %02X %s", msg.id, msg.dlc, sent);
    // Trace("%02X %02X %02X", msg.data[0], msg.data[1], msg.data[2]);
    SendMsg(msg);
    // Trace("res %d", res);

}/* End BUSMASTER generated function - OnBus_Connect */
/* Start BUSMASTER generated function - OnMsg_All */
void OnMsg_All(STCAN_MSG RxMsg)
{
    Trace("0x%lX", RxMsg.id);
    if (RxMsg.id == 0x7E8) {
        Trace("Receive 0x7E8 packet");
        STCAN_MSG msg;
        Utils_generate_msg(&msg);
        SendMsg(msg);
    }
}/* End BUSMASTER generated function - OnMsg_All */
/* Start BUSMASTER generated function - Utils_a */
void Utils_generate_msg(STCAN_MSG *msg)
{
    msg->id = reprogramed[cur].id;
    msg->dlc = reprogramed[cur].size + 1;
    memcpy(msg->data, reprogramed[cur].data, reprogramed[cur].size + 1);
    msg->isExtended = FALSE;
    msg->isRtr= FALSE;
    msg->cluster = 1;
    // time_t timer;
    // time(&timer);
    // msg->timeStamp = timer;
    cur++;
}/* End BUSMASTER generated function - Utils_a */
/* Start BUSMASTER generated function - OnMsgID_7e8 */
void OnMsgID_7e8(STCAN_MSG RxMsg)
{
    Trace("Received %lX", RxMsg.id);
}/* End BUSMASTER generated function - OnMsgID_7e8 */
