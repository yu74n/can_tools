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
enum frame_type {
    F_SINGLE,
    F_FIRST,
    F_CONSECUTIVE,
    F_DONE
};

typedef struct {
    unsigned int id;
    unsigned int size;
    // unsigned char data[8];
    unsigned char data[8];
} frame;

enum parser_status {
    SINGLE,
    FIRST,
    CONSECUTIVE,
    DONE
};

typedef struct {
    parser_status status;
} parser;

frame reprogramed[100];
int cur;
parser g_parser;
/* End BUSMASTER global variable */


/* Start BUSMASTER Function Prototype  */
GCC_EXTERN void GCC_EXPORT OnBus_Connect();
GCC_EXTERN void GCC_EXPORT OnMsg_All(STCAN_MSG RxMsg);
void Utils_generate_msg(STCAN_MSG *msg);
GCC_EXTERN void GCC_EXPORT OnMsgID_7e8(STCAN_MSG RxMsg);
void on_single();
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

    char buf[128];
    unsigned int num = 0;
    while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
        
        // Trace("%s", buf);
        char *p = strtok(buf, " ");
        if (p == NULL) {
            break;
        }
        unsigned int id = strtol(p, NULL, 16);
        if (id != 0x7E0) {
            continue;
        }

        frame *f = (frame *)malloc(sizeof(frame));
        if (f == NULL)
        {
            return;
        }
        f->id = id;
        f->size = 8;
        
        // Trace("ID: 0x%lX", f->id);
        for (int i = 0; i < 2; i++) {
            // skip unused data
            strtok(NULL, " ");
        }
        int c = 0;
        while (c < 8) {
            p = strtok(NULL, " ");
            unsigned int n = strtol(p, NULL, 16);
            f->data[c] = n;
            c++;
        }
        // Trace("ID: %X %DLC: %X Data: %02X %02X %02X %02X %02X %02X %02X %02X", f->id, f->size, f->data[0], f->data[1], f->data[2], f->data[3], f->data[4], f->data[5], f->data[6], f->data[7]);
        // unsigned char type = (b >> 4) & 0x0F;
        // unsigned char c;
        // // Trace("Type: %02X", type);
        
        // switch (type) {
        //     case 0:
        //     f->size = b & 0x0F;
        //     g_parser.status = SINGLE;
        //     break;
        //     case 1:
        //     c = strtol(p, NULL, 16);
        //     f->size = ((b & 0x0F) << 8) + (c & 0xFF);
        //     g_parser.status = FIRST;
        //     break;
        //     case 2:
        //     g_parser.status = CONSECUTIVE;
        //     break;
        //     case 3:
        //     continue;
        //     default:
        //     f->size = 0;
        // }
        // // Trace("0x%lX Type: %02X Size: %02X", f->id, type, f->size);
        // unsigned int size = f->size + 1;
        // unsigned char* d = (unsigned char*) malloc(size);
        // if (d == NULL) {
        //     return;
        // }
        // unsigned int count = 0;
        // d[count++] = f->size & 0x0F;
        // while ((p = strtok(NULL, " ")) != NULL && count < size) {
        //     unsigned long l = strtol(p, NULL, 16);
        //     d[count] = l & 0x000000FF;
        //     // Trace("%02X", d[count]);
        //     count++;
        // }
        // f->data = d;
        // if (f->id == 0x7E0 && g_parser.status == DONE) {
        //     memcpy(&reprogramed[num], f, sizeof(frame));
        //     num++;
        // }
        memcpy(&reprogramed[num], f, sizeof(frame));
        num++;
        free(f);
        f = NULL;
        
    }
    fclose(fp);

    STCAN_MSG msg;
    cur = 0;
    Utils_generate_msg(&msg);
    // char sent[128];
    // memset(sent, 0, sizeof(sent));
    // for (int i = 0; i < msg.dlc + 1; i++) {
    //     sprintf(sent, "%s %02X", sent, msg.data[i]);
    // }
    // Trace("sent 0x%lX Size: %02X %s", msg.id, msg.dlc, sent);
    // Trace("%02X %02X %02X", msg.data[0], msg.data[1], msg.data[2]);
    SendMsg(msg);
    // Trace("res %d", res);

}/* End BUSMASTER generated function - OnBus_Connect */
/* Start BUSMASTER generated function - OnMsg_All */
void OnMsg_All(STCAN_MSG RxMsg)
{
    // Trace("Received all 0x%lX", RxMsg.id);
    // if (RxMsg.id == 0x7E8) {
    //     Trace("Receive 0x7E8 packet");
    //     STCAN_MSG msg;
    //     Utils_generate_msg(&msg);
    //     SendMsg(msg);
    // }
}/* End BUSMASTER generated function - OnMsg_All */
/* Start BUSMASTER generated function - Utils_a */
void Utils_generate_msg(STCAN_MSG *msg)
{
    msg->id = reprogramed[cur].id;
    msg->dlc = reprogramed[cur].size;
    memcpy(msg->data, reprogramed[cur].data, reprogramed[cur].size);
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
    STCAN_MSG msg;
    if (RxMsg.byteAt(0) == 0x30) {
        if (RxMsg.byteAt(1) == 0) {
            while (reprogramed[cur].data[0] >> 4 == 0x2) {
                Utils_generate_msg(&msg);
                SendMsg(msg);
            }
        }
        int c = RxMsg.byteAt(1);
        int i = 0;
        while (i < c) {
            Utils_generate_msg(&msg);
                SendMsg(msg);
                i++;
        }
    } else {
        Utils_generate_msg(&msg);
        SendMsg(msg);
    }
}/* End BUSMASTER generated function - OnMsgID_7e8 */