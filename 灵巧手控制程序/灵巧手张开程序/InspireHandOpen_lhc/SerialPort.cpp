#include "StdAfx.h"    
#include "SerialPort.h"    
#include <process.h>    
#include <iostream>    
#include "InspireHand.h"   
using  namespace std;
/** 线程退出标志 */
bool CSerialPort::s_bExit = false;
/** 当串口无数据时,sleep至下次查询间隔的时间,单位:秒 */
const UINT SLEEP_TIME_INTERVAL = 5;

CSerialPort::CSerialPort(void)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;
	InitializeCriticalSection(&m_csCommunicationSync);
}
CSerialPort::CSerialPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/)
	: m_hListenThread(INVALID_HANDLE_VALUE)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_hListenThread = INVALID_HANDLE_VALUE;
	InitializeCriticalSection(&m_csCommunicationSync);
	InitPort(portNo, baud);

}
CSerialPort::~CSerialPort(void)
{
	CloseListenTread();
	ClosePort();
	DeleteCriticalSection(&m_csCommunicationSync);
}
bool CSerialPort::InitPort(UINT portNo /*= 1*/, UINT baud /*= CBR_9600*/, char parity /*= 'N'*/,
	UINT databits /*= 8*/, UINT stopsbits /*= 1*/, DWORD dwCommEvents /*= EV_RXCHAR*/)
{

	/** 临时变量,将制定参数转化为字符串形式,以构造DCB结构 */
	char szDCBparam[50];
	sprintf_s(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopsbits);

	/** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
	if (!openPort(portNo))
	{
		return false;
	}
	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 是否有错误发生 */
	BOOL bIsSuccess = TRUE;

	/** 在此可以设置输入输出的缓冲区大小,如果不设置,则系统会设置默认值.
	*  自己设置缓冲区大小时,要注意设置稍大一些,避免缓冲区溢出
	*/
	if (bIsSuccess )
	{
	bIsSuccess = SetupComm(m_hComm,500,500);
	}

	/** 设置串口的超时时间,均设为0,表示不使用超时限制 */
	COMMTIMEOUTS  CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	if (bIsSuccess)
	{
		bIsSuccess = SetCommTimeouts(m_hComm, &CommTimeouts);
	}

	DCB  dcb;
	if (bIsSuccess)
	{
		// 将ANSI字符串转换为UNICODE字符串    
		DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, NULL, 0);
		wchar_t* pwText = new wchar_t[dwNum];
		if (!MultiByteToWideChar(CP_ACP, 0, szDCBparam, -1, pwText, dwNum))
		{
			bIsSuccess = TRUE;
		}

		/** 获取当前串口配置参数,并且构造串口DCB参数 */
		bIsSuccess = GetCommState(m_hComm, &dcb) && BuildCommDCB(pwText, &dcb);
		/** 开启RTS flow控制 */
		dcb.fRtsControl = RTS_CONTROL_ENABLE;

		/** 释放内存空间 */
		delete[] pwText;
	}

	if (bIsSuccess)
	{
		/** 使用DCB参数配置串口状态 */
		bIsSuccess = SetCommState(m_hComm, &dcb);
	}

	/**  清空串口缓冲区 */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** 离开临界段 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return bIsSuccess == TRUE;
}

bool CSerialPort::InitPort(UINT portNo, const LPDCB& plDCB)
{
	/** 打开指定串口,该函数内部已经有临界区保护,上面请不要加保护 */
	if (!openPort(portNo))
	{
		return false;
	}

	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 配置串口参数 */
	if (!SetCommState(m_hComm, plDCB))
	{
		return false;
	}

	/**  清空串口缓冲区 */
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	/** 离开临界段 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

void CSerialPort::ClosePort()
{
	/** 如果有串口被打开，关闭它 */
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
	}
}

bool CSerialPort::openPort(UINT portNo)
{
	/** 进入临界段 */
	EnterCriticalSection(&m_csCommunicationSync);

	/** 把串口的编号转换为设备名 */
	char szPort[50];
	sprintf_s(szPort, "COM%d", portNo);

	/** 打开指定的串口 */
	m_hComm = CreateFileA(szPort,  /** 设备名,COM1,COM2等 */
		GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */
		0,                            /** 共享模式,0表示不共享 */
		NULL,                         /** 安全性设置,一般使用NULL */
		OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */
		0,                            /** 该参数0：同步方式，ReadFile和WriteFile函数为阻塞式 */
		NULL);

	/** 如果打开失败，释放资源并返回 */
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}

	/** 退出临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);

	return true;
}

bool CSerialPort::OpenListenThread()
{
	/** 检测线程是否已经开启了 */
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 线程已经开启 */
		return false;
	}

	s_bExit = false;
	/** 线程ID */
	UINT threadId;
	/** 开启串口数据监听线程 */
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!m_hListenThread)
	{
		return false;
	}
	/** 设置线程的优先级,高于普通线程 */
	if (!SetThreadPriority(m_hListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return false;
	}

	return true;
}

bool CSerialPort::CloseListenTread()
{
	if (m_hListenThread != INVALID_HANDLE_VALUE)
	{
		/** 通知线程退出 */
		s_bExit = true;

		/** 等待线程退出 */
		Sleep(10);

		/** 置线程句柄无效 */
		CloseHandle(m_hListenThread);
		m_hListenThread = INVALID_HANDLE_VALUE;
	}
	return true;
}
/** 获取串口缓冲区中尚未被读取字节数
*  @return: UINT  操作是否成功
*  @note:   当串口缓冲区中无数据时,返回0
*  @see:
*/
UINT CSerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;  /** 错误码 */
	COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */
	memset(&comstat, 0, sizeof(COMSTAT));
	UINT BytesInQue = 0;
	/** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志
	*	并获取通讯设备的状态
	*/
	if (ClearCommError(m_hComm, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** 获取在输入缓冲区中的字节数 */
	}
	return BytesInQue;
}

UINT WINAPI CSerialPort::ListenThread(void* pParam)
{
	/** 得到本类的指针 */
	CSerialPort* pSerialPort = reinterpret_cast<CSerialPort*>(pParam);
	// 线程循环,轮询方式读取串口数据    
	while (!pSerialPort->s_bExit)
	{
		UINT BytesInQue = pSerialPort->GetBytesInCOM();
		/** 如果串口输入缓冲区中无数据,则休息一会再查询 */
		if (BytesInQue == 0)
		{
			Sleep(SLEEP_TIME_INTERVAL);
			continue;
		}
		/** 读取输入缓冲区中的数据并输出显示 */
		char cRecved = 0x00;
		int count = 0;
		if(BytesInQue > 0)
		{
			if (pSerialPort->ReadChar(BytesInQue) == true)
			{
				pSerialPort->Uart_Rx_Pro(BytesInQue);
			}
		} 
	}
	return 0;
}
//对接受数据的协议解析
void CSerialPort::Uart_Rx_Pro(DWORD Bytes_int)
{
	UCHAR sum_check = 0;
	int length = str_rx_buffer[3] + 5;
	static int Status_Rec = 0;
	static int Index = 0;
	static int Frame_length = 0;
	for (int i = 0; i < (int)Bytes_int; i++)
	{
		switch (Status_Rec)
		{
		case 0:
			{
				if ((UCHAR)str_rx_buffer[i] == 0x90 )
				{					
					str_rx[Index] = str_rx_buffer[i];
					Index = 1;
					Status_Rec = 1;
				}
				break;
			}
		case 1:
		{
			if ((UCHAR)str_rx_buffer[i] == 0xeb)
			{
				str_rx[Index] = str_rx_buffer[i];
				Index = 2;
				Status_Rec = 2;
			}
			break;
		}
		case 2:  //ID
		{
			str_rx[Index] = str_rx_buffer[i];
			Index = 3;
			Status_Rec = 3;
			break;
		}
		case 3:  //长度
		{
			str_rx[Index] = str_rx_buffer[i];
			Frame_length = (str_rx[Index]&0x00FF) + 5;
			Index = 4;
			Status_Rec = 4;
			break;
		}
		case 4://
		{
			str_rx[Index] = str_rx_buffer[i];
			if (Index == Frame_length-1)//完整一帧已接收
			{
				for (int x = 0; x < (Frame_length - 3); x++)
				{
					sum_check = sum_check + str_rx[2 + x];
				}
				if (sum_check == (UCHAR)str_rx[Index])//校验正取
				{
					m_hand.id = str_rx[2];
					short Rcv_Prameter_Index = ((short)str_rx[6] << 8) + (0x00FF & (short)str_rx[5]);					
					if ((UCHAR)str_rx[4] == CMD_HANDG3_READ)
					{

						if (Rcv_Prameter_Index == CMD_FINGER_ANGLE_2B(0))
						{
							m_hand.rcv_type = GET_STATE;
							for (int m = 0; m < 6; m++)
							{
								m_hand.finger[m].Angle_Act = (str_rx[8 + 2 * m] << 8) + str_rx[7 + 2 * m];
								m_hand.finger[m].Force_Act = (str_rx[8 + 24 + 2 * m] << 8) + str_rx[7 + 24 + 2 * m];
								m_hand.finger[m].Current = (str_rx[8 + 36 + 2 * m] << 8) + str_rx[7 + 36 + 2 * m];
								m_hand.finger[m].ErrorCode = str_rx[8 + 36+24 +m];
							}
						}
					}
					if ((UCHAR)str_rx_buffer[4] == CMD_HANDG3_WRITE)
					{
						if (Rcv_Prameter_Index == CMD_FINGER_ANGLE_SET_2B(0))
						{
							m_hand.rcv_type = SET_ANGLE;
						}
						if (Rcv_Prameter_Index == CMD_FINGER_SPEED_SET_2B(0))
						{
							m_hand.rcv_type = SET_SPEED;
						}
						if (Rcv_Prameter_Index == CMD_FINGER_FORCE_SET_2B(0))
						{
							m_hand.rcv_type = SET_FORCE;
						}
					}
					Index = 0;
					Status_Rec = 0;
				}
				else
				{
					Index = 0;
					Status_Rec = 0;
				}
				Index = 0;
				Status_Rec = 0;
			}
			Index++;		
		}
		default:break;
		}
	}

}
bool CSerialPort::ReadChar(DWORD int_byte)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	/** 临界区保护(利用互斥保证线程安全) */
	EnterCriticalSection(&m_csCommunicationSync);
	/** 从缓冲区读取一个字节的数据 */
	bResult = ReadFile(m_hComm, str_rx_buffer, int_byte, &BytesRead, NULL);
	if ((!bResult))
	{
		/** 获取错误码,可以根据该错误码查出错误原因 */
		DWORD dwError = GetLastError();
		/** 清空串口缓冲区 */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);
		return false;
	}
	/** 离开临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);
	return true;
}
bool CSerialPort::WriteData(unsigned int length)
{
	BOOL   bResult = TRUE;
	DWORD  BytesToSend = 0;
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	/** 临界区保护 */
	EnterCriticalSection(&m_csCommunicationSync);
	/** 向缓冲区写入指定量的数据 */
	bResult = WriteFile(m_hComm, str_tx_buffer, length, &BytesToSend, NULL);
	if (!bResult)
	{
		DWORD dwError = GetLastError();
		/** 清空串口缓冲区 */
		PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);

		return false;
	}
	/** 离开临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);
	m_hand.rcv_type = NO_RESPONSE;
	return true;
}

