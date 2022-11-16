#pragma once

#include <QtWidgets/QMainWindow>
#include <qtcpsocket.h>
#include <qtcpserver.h>
#include <qtimer.h>
#include <QCloseEvent>
#include <QtSerialPort/QSerialPort>

#include "ui_GloveApp.h"
#include "Parameter.h"

class GloveApp : public QMainWindow
{
    Q_OBJECT

public:
    GloveApp(QWidget *parent = Q_NULLPTR);

private:
    void dataShow();
    void paraDataLoad();
    void paraDataSave();
    void ctrlHand();
    void closeEvent(QCloseEvent* event);


private slots:
    void tcpConnect();
    void getData();
    void timeEvent();

private:
    Ui::GloveAppClass ui;

private:
    short vel[6];
    unsigned int recData[18];
    QTimer* pTimer;
    QTcpServer* m_pTcpServer;
    QTcpSocket* pClientConnection;
    QByteArray rec;
    QSerialPort* pSerialPort;
    Parameter parameter;
};
