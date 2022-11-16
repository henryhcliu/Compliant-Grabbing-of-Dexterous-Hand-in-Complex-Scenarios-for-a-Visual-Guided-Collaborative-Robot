#include "GloveApp.h"
#include <QDebug>

GloveApp::GloveApp(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	//加载参数
	this->paraDataLoad();

	//1. 创建TCP对象
	m_pTcpServer = new QTcpServer(this);
    pTimer = new QTimer(this);

	//2. 新连接、错误信号
	connect(m_pTcpServer, &QTcpServer::newConnection, this, &GloveApp::tcpConnect);
	connect(pTimer, SIGNAL(timeout()), this, SLOT(timeEvent()));

	//3. 启动服务端
	if (!m_pTcpServer->listen(QHostAddress::Any, parameter.ipPort))
	{
		qDebug() << "m_pTcpServer->listen() error";
		assert(false);
	}

	//开启串口
	pSerialPort = new QSerialPort();
	pSerialPort->setPortName(QString::fromStdString(this->parameter.com));
	if (pSerialPort->open(QIODevice::ReadWrite))
	{
		qDebug() << "QSerialPort open error";
	}
	pSerialPort->setBaudRate(QSerialPort::Baud115200);
	pSerialPort->setDataBits(QSerialPort::Data8);
	pSerialPort->setParity(QSerialPort::NoParity);
	pSerialPort->setStopBits(QSerialPort::OneStop);
	pSerialPort->setFlowControl(QSerialPort::NoFlowControl);
}

void GloveApp::tcpConnect()
{
	//获取服务端数据
	pClientConnection = m_pTcpServer->nextPendingConnection();
	qDebug() << "get new connect";
	connect(pClientConnection, SIGNAL(readyRead()), this, SLOT(getData()));
	this->pTimer->start(30);
}

void GloveApp::timeEvent()
{
	//读取手套
	if(this->ui.checkBoxGlove->isChecked())
	{
		char sWriteData[] = { 0x47 };
		pClientConnection->write(sWriteData, 1);
		//qDebug() << "send Tcp Data";
	}
}

void GloveApp::dataShow()
{
	this->ui.lineEdit1->setText(QString::number(this->recData[0]));
	this->ui.lineEdit2->setText(QString::number(this->recData[1]));
	this->ui.lineEdit3->setText(QString::number(this->recData[2]));

	this->ui.lineEdit4->setText(QString::number(this->recData[3]));
	this->ui.lineEdit5->setText(QString::number(this->recData[4]));
	this->ui.lineEdit6->setText(QString::number(this->recData[5]));

	this->ui.lineEdit7->setText(QString::number(this->recData[6]));
	this->ui.lineEdit8->setText(QString::number(this->recData[7]));
	this->ui.lineEdit9->setText(QString::number(this->recData[8]));

	this->ui.lineEdit10->setText(QString::number(this->recData[9]));
	this->ui.lineEdit11->setText(QString::number(this->recData[10]));
	this->ui.lineEdit12->setText(QString::number(this->recData[11]));

	this->ui.lineEdit13->setText(QString::number(this->recData[12]));
	this->ui.lineEdit14->setText(QString::number(this->recData[13]));
	this->ui.lineEdit15->setText(QString::number(this->recData[14]));

	this->ui.lineEdit16->setText(QString::number(this->recData[15]));
	this->ui.lineEdit17->setText(QString::number(this->recData[16]));
	this->ui.lineEdit18->setText(QString::number(this->recData[17]));
}

void GloveApp::paraDataLoad()
{
	this->ui.spinBoxMin1->setValue(this->parameter.spinBoxMinValue[0]);
	this->ui.spinBoxMin2->setValue(this->parameter.spinBoxMinValue[1]);
	this->ui.spinBoxMin3->setValue(this->parameter.spinBoxMinValue[2]);
	this->ui.spinBoxMin4->setValue(this->parameter.spinBoxMinValue[3]);
	this->ui.spinBoxMin5->setValue(this->parameter.spinBoxMinValue[4]);
	this->ui.spinBoxMin6->setValue(this->parameter.spinBoxMinValue[5]);
	this->ui.spinBoxMin7->setValue(this->parameter.spinBoxMinValue[6]);
	this->ui.spinBoxMin8->setValue(this->parameter.spinBoxMinValue[7]);
	this->ui.spinBoxMin9->setValue(this->parameter.spinBoxMinValue[8]);

	this->ui.spinBoxMin10->setValue(this->parameter.spinBoxMinValue[9]);
	this->ui.spinBoxMin11->setValue(this->parameter.spinBoxMinValue[10]);
	this->ui.spinBoxMin12->setValue(this->parameter.spinBoxMinValue[11]);
	this->ui.spinBoxMin13->setValue(this->parameter.spinBoxMinValue[12]);
	this->ui.spinBoxMin14->setValue(this->parameter.spinBoxMinValue[13]);
	this->ui.spinBoxMin15->setValue(this->parameter.spinBoxMinValue[14]);
	this->ui.spinBoxMin16->setValue(this->parameter.spinBoxMinValue[15]);
	this->ui.spinBoxMin17->setValue(this->parameter.spinBoxMinValue[16]);
	this->ui.spinBoxMin18->setValue(this->parameter.spinBoxMinValue[17]);

	this->ui.spinBoxMax1->setValue(this->parameter.spinBoxMaxValue[0]);
	this->ui.spinBoxMax2->setValue(this->parameter.spinBoxMaxValue[1]);
	this->ui.spinBoxMax3->setValue(this->parameter.spinBoxMaxValue[2]);
	this->ui.spinBoxMax4->setValue(this->parameter.spinBoxMaxValue[3]);
	this->ui.spinBoxMax5->setValue(this->parameter.spinBoxMaxValue[4]);
	this->ui.spinBoxMax6->setValue(this->parameter.spinBoxMaxValue[5]);
	this->ui.spinBoxMax7->setValue(this->parameter.spinBoxMaxValue[6]);
	this->ui.spinBoxMax8->setValue(this->parameter.spinBoxMaxValue[7]);
	this->ui.spinBoxMax9->setValue(this->parameter.spinBoxMaxValue[8]);

	this->ui.spinBoxMax10->setValue(this->parameter.spinBoxMaxValue[9]);
	this->ui.spinBoxMax11->setValue(this->parameter.spinBoxMaxValue[10]);
	this->ui.spinBoxMax12->setValue(this->parameter.spinBoxMaxValue[11]);
	this->ui.spinBoxMax13->setValue(this->parameter.spinBoxMaxValue[12]);
	this->ui.spinBoxMax14->setValue(this->parameter.spinBoxMaxValue[13]);
	this->ui.spinBoxMax15->setValue(this->parameter.spinBoxMaxValue[14]);
	this->ui.spinBoxMax16->setValue(this->parameter.spinBoxMaxValue[15]);
	this->ui.spinBoxMax17->setValue(this->parameter.spinBoxMaxValue[16]);
	this->ui.spinBoxMax18->setValue(this->parameter.spinBoxMaxValue[17]);

	this->ui.checkBoxGlove->setChecked(this->parameter.check[0]);
	this->ui.checkBoxHand->setChecked(this->parameter.check[1]);
}

void GloveApp::paraDataSave()
{
	this->parameter.spinBoxMinValue[0] = this->ui.spinBoxMin1->value();
	this->parameter.spinBoxMinValue[1] = this->ui.spinBoxMin2->value();
	this->parameter.spinBoxMinValue[2] = this->ui.spinBoxMin3->value();
	this->parameter.spinBoxMinValue[3] = this->ui.spinBoxMin4->value();
	this->parameter.spinBoxMinValue[4] = this->ui.spinBoxMin5->value();
	this->parameter.spinBoxMinValue[5] = this->ui.spinBoxMin6->value();
	this->parameter.spinBoxMinValue[6] = this->ui.spinBoxMin7->value();
	this->parameter.spinBoxMinValue[7] = this->ui.spinBoxMin8->value();
	this->parameter.spinBoxMinValue[8] = this->ui.spinBoxMin9->value();

	this->parameter.spinBoxMinValue[9] = this->ui.spinBoxMin10->value();
	this->parameter.spinBoxMinValue[10] = this->ui.spinBoxMin11->value();
	this->parameter.spinBoxMinValue[11] = this->ui.spinBoxMin12->value();
	this->parameter.spinBoxMinValue[12] = this->ui.spinBoxMin13->value();
	this->parameter.spinBoxMinValue[13] = this->ui.spinBoxMin14->value();
	this->parameter.spinBoxMinValue[14] = this->ui.spinBoxMin15->value();
	this->parameter.spinBoxMinValue[15] = this->ui.spinBoxMin16->value();
	this->parameter.spinBoxMinValue[16] = this->ui.spinBoxMin17->value();
	this->parameter.spinBoxMinValue[17] = this->ui.spinBoxMin18->value();

	this->parameter.spinBoxMaxValue[0] = this->ui.spinBoxMax1->value();
	this->parameter.spinBoxMaxValue[1] = this->ui.spinBoxMax2->value();
	this->parameter.spinBoxMaxValue[2] = this->ui.spinBoxMax3->value();
	this->parameter.spinBoxMaxValue[3] = this->ui.spinBoxMax4->value();
	this->parameter.spinBoxMaxValue[4] = this->ui.spinBoxMax5->value();
	this->parameter.spinBoxMaxValue[5] = this->ui.spinBoxMax6->value();
	this->parameter.spinBoxMaxValue[6] = this->ui.spinBoxMax7->value();
	this->parameter.spinBoxMaxValue[7] = this->ui.spinBoxMax8->value();
	this->parameter.spinBoxMaxValue[8] = this->ui.spinBoxMax9->value();

	this->parameter.spinBoxMaxValue[9] = this->ui.spinBoxMax10->value();
	this->parameter.spinBoxMaxValue[10] = this->ui.spinBoxMax11->value();
	this->parameter.spinBoxMaxValue[11] = this->ui.spinBoxMax12->value();
	this->parameter.spinBoxMaxValue[12] = this->ui.spinBoxMax13->value();
	this->parameter.spinBoxMaxValue[13] = this->ui.spinBoxMax14->value();
	this->parameter.spinBoxMaxValue[14] = this->ui.spinBoxMax15->value();
	this->parameter.spinBoxMaxValue[15] = this->ui.spinBoxMax16->value();
	this->parameter.spinBoxMaxValue[16] = this->ui.spinBoxMax17->value();
	this->parameter.spinBoxMaxValue[17] = this->ui.spinBoxMax18->value();

	this->parameter.check[0] = this->ui.checkBoxGlove->isChecked();
	this->parameter.check[1] = this->ui.checkBoxHand->isChecked();

	parameter.saveParameter();
}

void GloveApp::ctrlHand()
{
	//生成机械手的位置指令
	auto calcVel = [](int value, int a, int b)->short {
		if (a == b)
		{
			return -1;
		}
		int rtl = qRound(1000.0 / (b - a) * (value - a));
		rtl = qMax(rtl, 0);
		rtl = qMin(rtl, 1000);
		return rtl;
	};

	this->vel[0] = calcVel(this->recData[12], this->ui.spinBoxMax13->value(), this->ui.spinBoxMin13->value());
	this->vel[1] = calcVel(this->recData[9], this->ui.spinBoxMax10->value(), this->ui.spinBoxMin10->value());
	this->vel[2] = calcVel(this->recData[6], this->ui.spinBoxMax7->value(), this->ui.spinBoxMin7->value());
	this->vel[3] = calcVel(this->recData[4], this->ui.spinBoxMax5->value(), this->ui.spinBoxMin5->value());
	this->vel[4] = calcVel(this->recData[2], this->ui.spinBoxMax3->value(), this->ui.spinBoxMin3->value());
	this->vel[5] = calcVel(this->recData[0], this->ui.spinBoxMax1->value(), this->ui.spinBoxMin1->value());

	qDebug() << "vel" << vel[0] << vel[1] << vel[2] << vel[3] << vel[4] << vel[5];

	//控制机械手
	if(this->ui.checkBoxHand->isChecked())
	{
#define FRAME_HEAD1 0xEB
#define FRAME_HEAD2 0x90
#define RH56_ID 1
#define CMD_FINGER_ANGLE_SET_2B(m) (1486 + m * 2)
#define CMD_HANDG3_WRITE 0x12            //写三代手内部寄存器

		auto checkSum = [](const char* vec, int size)->char {
			unsigned int sum = 0;
			for (int i = 0; i < size; i++) {
				sum += vec[i];
			}
			return (sum & 0xff);
		};

		char cmd[20] = { 0 };
		cmd[0] = FRAME_HEAD1;
		cmd[1] = FRAME_HEAD2;
		cmd[2] = RH56_ID; //  ID
		cmd[5] = static_cast<char>(CMD_FINGER_ANGLE_SET_2B(0) & 0xFF);
		cmd[6] = static_cast<char>((CMD_FINGER_ANGLE_SET_2B(0) >> 8) & 0xFF);
		cmd[3] = 12 + 2 + 1;
		cmd[4] = CMD_HANDG3_WRITE;

		short* pS = reinterpret_cast<short*>(&cmd[7]);
		pS[0] = this->vel[0];
		pS[1] = this->vel[1];
		pS[2] = this->vel[2];
		pS[3] = this->vel[3];
		pS[4] = this->vel[4];
		pS[5] = this->vel[5];

		cmd[19] = checkSum(&cmd[2], 17);

		pSerialPort->write(cmd, 20);
		qDebug() << "send serial port Data";
	}
}

void GloveApp::closeEvent(QCloseEvent* event)
{
	this->paraDataSave();
}

void GloveApp::getData()
{
	this->rec += pClientConnection->readAll();
	while (1)
	{
		if (this->rec.length() <= 5)
			break;
		if (rec.length() >= 20)
		{
			if (this->rec.at(0) == (char)0x47 && this->rec.at(19) == (char)0x00)
			{
				//qDebug() << "get data";
				for (auto i = 0; i < 18; i++)
				{
					recData[i] = (unsigned char)this->rec.at(i + 1);
				}
				this->dataShow();
				this->ctrlHand();
				this->rec.remove(0, 20);
			}
			else
			{
				this->rec.remove(0, 1);
			}
		}
		else
			break;
	}
}
