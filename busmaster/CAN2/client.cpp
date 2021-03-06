/* This file is generated by BUSMASTER */
/* VERSION [1.2] */
/* BUSMASTER VERSION [3.0.0] */
/* PROTOCOL [CAN] */

/* Start BUSMASTER include header */
#include <Windows.h>
#include <CANIncludes.h>
/* End BUSMASTER include header */


/* Start BUSMASTER global variable */
STCAN_MSG TxMsg;
int seed_sent_flag=0;
int timeout_flag=0;
int seed = 0xFEEDBEEF;
int key=0;
unsigned int id = 0x169;
unsigned int counter = 0;
unsigned char data[] = { 0x02, 0xFF, 0x00, 0x29, 0x00, 0x00 };
/* End BUSMASTER global variable */


/* Start BUSMASTER Function Prototype  */
GCC_EXTERN void GCC_EXPORT OnTimer_send_message_10( );
GCC_EXTERN void GCC_EXPORT OnBus_Connect();
/* End BUSMASTER Function Prototype  */

/* Start BUSMASTER Function Wrapper Prototype  */
/* End BUSMASTER Function Wrapper Prototype  */


/* Start BUSMASTER generated function - OnTimer_send_message_10 */
void OnTimer_send_message_10( )
{
	// 02 FF 00 29 00 00 CTR CS
	unsigned int sum = 0;
	for (int i = 0; i < 6; i++) {
		sum += data[i];
	}
	TxMsg.data[0] = 0x02;
	TxMsg.data[1] = 0xFF;
	TxMsg.data[2] = 0x00;
	TxMsg.data[3] = 0x29;
	TxMsg.data[4] = 0x00;
	TxMsg.data[5] = 0x00;
	TxMsg.data[6] = counter++;
	TxMsg.data[7] = (unsigned char)(sum & 0x000000FF);
	SendMsg(TxMsg);
}/* End BUSMASTER generated function - OnTimer_send_message_10 */
/* Start BUSMASTER generated function - OnBus_Connect */
void OnBus_Connect()
{
	Trace("Bus connected");
	seed_sent_flag = 0;
	seed = 0xFEEDBEEF;
	timeout_flag=0;
	TxMsg.id = id; 
	TxMsg.isExtended = FALSE;
	TxMsg.isRtr = FALSE;
	TxMsg.dlc = 8;
	TxMsg.cluster = 1;
}/* End BUSMASTER generated function - OnBus_Connect */
