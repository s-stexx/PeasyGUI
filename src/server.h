// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>

#include "appconfig.h"

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(const AppConfig &config, QObject *parent = nullptr);

signals:
    void messageFromClientReceived(const QString& message, bool replyExpected);

public slots:
    void start();
    void quit();
    void newConnection();
    void disconnected();
    void readyRead();

    void sendReplyToClient(const QString& message);

private:
    QTcpServer server;

    //QTcpSocket *socket = nullptr;
    QPointer<QTcpSocket> socket;

    quint16 port;
    QString ip;
};

#endif // SERVER_H
