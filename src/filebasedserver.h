// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef FILEBASEDSERVER_H
#define FILEBASEDSERVER_H

#include "appconfig.h"
#include <QObject>
#include <QTimer>
#include <QDir>

class FileBasedServer : public QObject
{
    Q_OBJECT
public:
    explicit FileBasedServer(const AppConfig &config, QObject *parent = nullptr);

public slots:
    void start();
    void quit();
    void sendReplyToClient(const QString &message);

signals:
    void messageFromClientReceived(const QString &message, bool replyExpected);

private slots:
    void pollForRequestFileOrEvents();

private:
    void writeFile(const QString &payloadUtf8, const QString &fileName);
    bool readAndDeleteRequestFile(QString &payloadOut);

    bool pollEventsNext = true;
    QDir exchangeDir;
    QTimer pollTimer;
    QTimer eventTimer;
    quint64 nextEventFileIndex = 0;
    quint64 requestCount = 0;
    quint64 replyCount = 0;

};

#endif // FILEBASEDSERVER_H
