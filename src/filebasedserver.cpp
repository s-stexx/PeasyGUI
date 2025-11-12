// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "filebasedserver.h"
#include "appconfig.h"
#include <QTextStream>
#include <QFileDevice>
#include <QThread>

FileBasedServer::FileBasedServer(const AppConfig &config, QObject *parent)
    : QObject(parent)
{
    exchangeDir = config.exchangeDir;
    Q_ASSERT(exchangeDir.exists());

    pollTimer.setInterval(100);

    connect(&pollTimer, &QTimer::timeout, this, &FileBasedServer::pollForRequestFileOrEvents);
}


void FileBasedServer::start()
{
    pollTimer.start();
    qInfo() << "FileBasedServer running in" << exchangeDir.absolutePath();
}


void FileBasedServer::quit()
{
    pollTimer.stop();
}


void FileBasedServer::pollForRequestFileOrEvents()
{
    // the poll is time triggered: has every request already be been replied?
    if ( ! (replyCount == requestCount)) {
        return;
    }

    if (pollEventsNext == true) {
        pollEventsNext = false;

        requestCount++;
        emit messageFromClientReceived("events?", true);

    } else {
        pollEventsNext = true;

        QString payload;
        if (readAndDeleteRequestFile(payload)) {
            requestCount++;
            emit messageFromClientReceived(payload, true);
        }
    }
}


bool FileBasedServer::readAndDeleteRequestFile(QString& payloadOut)
{
    const QString requestPath = exchangeDir.filePath("request.txt");
    QFileInfo requestInfo(requestPath);

    if ( ! requestInfo.exists()) {
        return false;
    }

    QFile requestFile{requestPath};
    if ( ! requestFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QByteArray payloadBytes = requestFile.readAll();
    requestFile.close();
    requestFile.remove();
    payloadOut = QString::fromUtf8(payloadBytes);
    return true;
}


void FileBasedServer::sendReplyToClient(const QString &message)
{
    if (pollEventsNext == false) {
        if ( ! (message == "NONE")) {
            nextEventFileIndex++;
            const QString fileName = QString("%1_events.txt").arg(nextEventFileIndex, 12, 10, QLatin1Char('0'));
            writeFile(message, fileName);
        }

    } else {
        writeFile(message, "reply.txt");
    }

    replyCount++;
}


void FileBasedServer::writeFile(const QString &message, const QString &fileName)
{
    const QString path = exchangeDir.filePath(fileName);
    QString draftPath = path;
    draftPath.replace(".txt", "_draft.txt");

    QFile file(draftPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qInfo() << "Error: Not possible to write Server file.";
        return;
    }

    file.write(message.toUtf8());
    file.flush();
    file.close();

    file.rename(draftPath, path);
}



