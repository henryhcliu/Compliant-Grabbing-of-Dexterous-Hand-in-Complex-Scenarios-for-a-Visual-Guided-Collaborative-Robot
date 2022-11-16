
#include "stdafx.h"
#include "IO.h"
#include "windows.h"
#include "time.h"
#include "InspireHand.h"
#include "SerialPort.h"
#pragma comment(lib, "winmm.lib ")
/********************************************************************
function:	ComR_Init
purpose :	使用windows的API函数实现串口操作
param   :	comport 串口号

return  :	true 成功 false 失败
*********************************************************************/

CInspireHand::CInspireHand(CSerialPort * para_p_SerialPort,int para_id)
{
	p_SerialPort = para_p_SerialPort;
	id = para_id;
	state = { NO_RESPONSE,
		0,
		{{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0},
		{0,0,0,0,0}}
	};
}
CInspireHand::~CInspireHand(void)
{
	;
}
void CInspireHand::Send_Frame_SetAngle(short Val0, short Val1, short Val2, short Val3, short Val4, short Val5)
{
	p_SerialPort->str_tx_buffer[0] = (char)FRAME_HEAD1;
	p_SerialPort->str_tx_buffer[1] = FRAME_HEAD2;
	p_SerialPort->str_tx_buffer[2] = id;//ID
	p_SerialPort->str_tx_buffer[5] = CMD_FINGER_ANGLE_SET_2B(0) & 0xFF;
	p_SerialPort->str_tx_buffer[6] = (CMD_FINGER_ANGLE_SET_2B(0) >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[3] = 12 + 2 + 1;
	p_SerialPort->str_tx_buffer[4] = CMD_HANDG3_WRITE;
	p_SerialPort->str_tx_buffer[7] = Val0 & 0xFF;
	p_SerialPort->str_tx_buffer[8] = (Val0 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[9] = Val1 & 0xFF;
	p_SerialPort->str_tx_buffer[10] = (Val1 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[11] = Val2 & 0xFF;
	p_SerialPort->str_tx_buffer[12] = (Val2 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[13] = Val3 & 0xFF;
	p_SerialPort->str_tx_buffer[14] = (Val3 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[15] = Val4 & 0xFF;
	p_SerialPort->str_tx_buffer[16] = (Val4 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[17] = Val5 & 0xFF;
	p_SerialPort->str_tx_buffer[18] = (Val5 >> 8) & 0xFF;
	UCHAR check_sum = 0;
	int SendL_Len = p_SerialPort->str_tx_buffer[3] + 5;
	for (int i = 2; i < (SendL_Len - 1); i++)
	{
		check_sum = check_sum + p_SerialPort->str_tx_buffer[i];
	}
	p_SerialPort->str_tx_buffer[SendL_Len - 1] = check_sum & 0xFF;
	p_SerialPort->WriteData(SendL_Len);
	Sleep(10);
}
bool CInspireHand::Send_Frame_SetAngle_block(short Val0,short Val1,short Val2,short Val3,short Val4,short Val5)
{
	p_SerialPort->str_tx_buffer[0] = FRAME_HEAD1;
	p_SerialPort->str_tx_buffer[1] = FRAME_HEAD2;
	p_SerialPort->str_tx_buffer[2] = id;//ID
	p_SerialPort->str_tx_buffer[5] = CMD_FINGER_ANGLE_SET_2B(0) & 0xFF;
	p_SerialPort->str_tx_buffer[6] = (CMD_FINGER_ANGLE_SET_2B(0)>>8) & 0xFF;
	p_SerialPort->str_tx_buffer[3] = 12+2+1;
	p_SerialPort->str_tx_buffer[4] = CMD_HANDG3_WRITE;
	p_SerialPort->str_tx_buffer[7] = Val0 & 0xFF;
	p_SerialPort->str_tx_buffer[8] = (Val0>>8) & 0xFF;
	p_SerialPort->str_tx_buffer[9] = Val1 & 0xFF;
	p_SerialPort->str_tx_buffer[10] = (Val1>>8) & 0xFF;
	p_SerialPort->str_tx_buffer[11] = Val2 & 0xFF;
	p_SerialPort->str_tx_buffer[12] = (Val2>>8) & 0xFF;
	p_SerialPort->str_tx_buffer[13] = Val3 & 0xFF;
	p_SerialPort->str_tx_buffer[14] = (Val3>>8) & 0xFF;
	p_SerialPort->str_tx_buffer[15] = Val4 & 0xFF;
	p_SerialPort->str_tx_buffer[16] = (Val4>>8) & 0xFF;
	p_SerialPort->str_tx_buffer[17] = Val5 & 0xFF;
	p_SerialPort->str_tx_buffer[18] = (Val5>>8) & 0xFF;
	UCHAR check_sum = 0;
	int SendL_Len = p_SerialPort->str_tx_buffer[3] + 5;
	for (int i = 2; i < (SendL_Len - 1); i++)
	{
		check_sum = check_sum + p_SerialPort->str_tx_buffer[i];
	}
	p_SerialPort->str_tx_buffer[SendL_Len - 1] = check_sum & 0xFF;
	p_SerialPort->WriteData(SendL_Len);
	DWORD delayTimeBegin;
	DWORD delayTimeEnd;
	delayTimeBegin = timeGetTime();
	while (true)
	{
		Sleep(2);
		delayTimeEnd = timeGetTime();
		if (delayTimeEnd - delayTimeBegin > 500)//延时超过1s，退出
		{
			return false;
		}
		if (p_SerialPort->m_hand.rcv_type == SET_ANGLE && p_SerialPort->m_hand.id == id)
		{

			return true;//成功
		}
	}
}
void CInspireHand::Send_Frame_SetSpeed(short Val0,short Val1,short Val2,short Val3,short Val4,short Val5)
{
	p_SerialPort->str_tx_buffer[0] = FRAME_HEAD1;
	p_SerialPort->str_tx_buffer[1] = FRAME_HEAD2;
	p_SerialPort->str_tx_buffer[2] = id;//ID
	p_SerialPort->str_tx_buffer[5] = CMD_FINGER_SPEED_SET_2B(0) & 0xFF;
	p_SerialPort->str_tx_buffer[6] = (CMD_FINGER_SPEED_SET_2B(0) >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[3] = 12 + 2 + 1;
	p_SerialPort->str_tx_buffer[4] = CMD_HANDG3_WRITE;
	p_SerialPort->str_tx_buffer[7] = Val0 & 0xFF;
	p_SerialPort->str_tx_buffer[8] = (Val0 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[9] = Val1 & 0xFF;
	p_SerialPort->str_tx_buffer[10] = (Val1 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[11] = Val2 & 0xFF;
	p_SerialPort->str_tx_buffer[12] = (Val2 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[13] = Val3 & 0xFF;
	p_SerialPort->str_tx_buffer[14] = (Val3 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[15] = Val4 & 0xFF;
	p_SerialPort->str_tx_buffer[16] = (Val4 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[17] = Val5 & 0xFF;
	p_SerialPort->str_tx_buffer[18] = (Val5 >> 8) & 0xFF;
	UCHAR check_sum = 0;
	int SendL_Len = p_SerialPort->str_tx_buffer[3] + 5;
	for (int i = 2; i < (SendL_Len - 1); i++)
	{
		check_sum = check_sum + p_SerialPort->str_tx_buffer[i];
	}
	p_SerialPort->str_tx_buffer[SendL_Len - 1] = check_sum & 0xFF;
	p_SerialPort->WriteData(SendL_Len);
}
void CInspireHand::Send_Frame_SetForce(short Val0,short Val1,short Val2,short Val3,short Val4,short Val5)
{
	p_SerialPort->str_tx_buffer[0] = FRAME_HEAD1;
	p_SerialPort->str_tx_buffer[1] = FRAME_HEAD2;
	p_SerialPort->str_tx_buffer[2] = id;//ID
	p_SerialPort->str_tx_buffer[5] = CMD_FINGER_FORCE_SET_2B(0) & 0xFF;
	p_SerialPort->str_tx_buffer[6] = (CMD_FINGER_FORCE_SET_2B(0) >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[3] = 12 + 2 + 1;
	p_SerialPort->str_tx_buffer[4] = CMD_HANDG3_WRITE;
	p_SerialPort->str_tx_buffer[7] = Val0 & 0xFF;
	p_SerialPort->str_tx_buffer[8] = (Val0 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[9] = Val1 & 0xFF;
	p_SerialPort->str_tx_buffer[10] = (Val1 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[11] = Val2 & 0xFF;
	p_SerialPort->str_tx_buffer[12] = (Val2 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[13] = Val3 & 0xFF;
	p_SerialPort->str_tx_buffer[14] = (Val3 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[15] = Val4 & 0xFF;
	p_SerialPort->str_tx_buffer[16] = (Val4 >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[17] = Val5 & 0xFF;
	p_SerialPort->str_tx_buffer[18] = (Val5 >> 8) & 0xFF;
	UCHAR check_sum = 0;
	int SendL_Len = p_SerialPort->str_tx_buffer[3] + 5;
	for (int i = 2; i < (SendL_Len - 1); i++)
	{
		check_sum = check_sum + p_SerialPort->str_tx_buffer[i];
	}
	p_SerialPort->str_tx_buffer[SendL_Len - 1] = check_sum & 0xFF;
	p_SerialPort->WriteData(SendL_Len);
}
void CInspireHand::Send_Frame_GetState(void)
{
	p_SerialPort->str_tx_buffer[0] = FRAME_HEAD1;
	p_SerialPort->str_tx_buffer[1] = FRAME_HEAD2;
	p_SerialPort->str_tx_buffer[2] = id;//ID
	p_SerialPort->str_tx_buffer[3] = 3+1;
	p_SerialPort->str_tx_buffer[4] = CMD_HANDG3_READ;
	p_SerialPort->str_tx_buffer[5] = CMD_FINGER_ANGLE_2B(0) & 0xFF;
	p_SerialPort->str_tx_buffer[6] = (CMD_FINGER_ANGLE_2B(0) >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[7] = 78;
	UCHAR check_sum = 0;
	int SendL_Len = p_SerialPort->str_tx_buffer[3] + 5;
	for (int i = 2; i < (SendL_Len - 1); i++)
	{
		check_sum = check_sum + p_SerialPort->str_tx_buffer[i];
	}
	p_SerialPort->str_tx_buffer[SendL_Len - 1] = check_sum & 0xFF;
	p_SerialPort->WriteData(SendL_Len);
}

bool CInspireHand::Send_Frame_GetState_block(Hand_State* para_state)
{
	p_SerialPort->str_tx_buffer[0] = FRAME_HEAD1;
	p_SerialPort->str_tx_buffer[1] = FRAME_HEAD2;
	p_SerialPort->str_tx_buffer[2] = id;//ID
	p_SerialPort->str_tx_buffer[3] = 3 + 1;
	p_SerialPort->str_tx_buffer[4] = CMD_HANDG3_READ;
	p_SerialPort->str_tx_buffer[5] = CMD_FINGER_ANGLE_2B(0) & 0xFF;
	p_SerialPort->str_tx_buffer[6] = (CMD_FINGER_ANGLE_2B(0) >> 8) & 0xFF;
	p_SerialPort->str_tx_buffer[7] = 78;
	UCHAR check_sum = 0;
	int SendL_Len = p_SerialPort->str_tx_buffer[3] + 5;
	for (int i = 2; i < (SendL_Len - 1); i++)
	{
		check_sum = check_sum + p_SerialPort->str_tx_buffer[i];
	}
	p_SerialPort->str_tx_buffer[SendL_Len - 1] = check_sum & 0xFF;
	p_SerialPort->WriteData(SendL_Len);
	DWORD delayTimeBegin;
	DWORD delayTimeEnd;
	delayTimeBegin = timeGetTime();
	while (true)
	{
		Sleep(2);
		delayTimeEnd = timeGetTime();
		if (delayTimeEnd - delayTimeBegin > 300)//延时超过1s，退出
		{
			return false;
		}
		if (p_SerialPort->m_hand.rcv_type == GET_STATE && p_SerialPort->m_hand.id == id)
		{
			memcpy(&state, &(p_SerialPort->m_hand), sizeof(state));
			*para_state = state;
			return true;//成功
		}
	}
}
void CInspireHand::Get_State(Hand_State* para_state)
{
	if (id == p_SerialPort->m_hand.id)
	{
		memcpy(para_state, &(p_SerialPort->m_hand), sizeof(state));
		memcpy(&state, &(p_SerialPort->m_hand), sizeof(state));
	}
	else
	{
		memset(para_state, 0, sizeof(para_state));
	}
}
