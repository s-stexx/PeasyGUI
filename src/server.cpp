// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "server.h"

Server::Server(const AppConfig &config, QObject *parent) : QObject(parent)
{
    ip = config.ip;
    port = config.port;

    connect(&server, &QTcpServer::newConnection, this, &Server::newConnection);
}


void Server::start()
{
    if (!server.listen(QHostAddress::AnyIPv4, port)) {
        qWarning() << server.errorString();
        return;
    }

    qInfo() << "Listening...";
}


void Server::quit()
{
    if (socket) {
        disconnect(socket, nullptr, this, nullptr);

        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->disconnectFromHost();
        }

        if (socket->state() != QAbstractSocket::UnconnectedState) {
            socket->waitForDisconnected(1000);
        }

        socket->deleteLater();
        socket = nullptr;
    }
    server.close();
}


void Server::newConnection()
{
    QTcpSocket* newSocket = server.nextPendingConnection();

    QString peerIp = newSocket->peerAddress().toString();

    // if IPv4-mapped, strip IPv6 prefix
    if (peerIp.startsWith("::ffff:")) {
        peerIp = peerIp.mid(7);
    }

    if (peerIp == ip) {
        if (socket) {
            socket->disconnectFromHost();
            socket->deleteLater();
        }

        socket = newSocket;
        connect(socket, &QTcpSocket::readyRead, this, &Server::readyRead);
        connect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);

        qInfo() << "Connected:" << peerIp;
    } else {
        qInfo() << "Connection refused:" << peerIp;
        newSocket->disconnectFromHost();
        newSocket->deleteLater();
    }
}


void Server::disconnected()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (!s) {
        return;
    }

    qInfo() << "Disconnected:" << s->peerAddress().toString();

    if (s == socket) {
        socket = nullptr;
    }

    s->deleteLater();
}


void Server::readyRead()
{
    QTcpSocket* s = qobject_cast<QTcpSocket*>(sender());
    if (!s || s != socket) {
        return;
    }

    QByteArray receivedBytes = s->readAll();
    QString message = QString::fromUtf8(receivedBytes);

    emit messageFromClientReceived(message, true);
}


void Server::sendReplyToClient(const QString &message)
{
    if (!socket.isNull() && socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(message.toUtf8());
        socket->flush();
    } else {
        qWarning() << "No valid socket to send reply.";
    }
}

