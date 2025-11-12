// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "initializer.h"

#include "apppaths.h"
#include "quarantinechecker.h"

#include <QCommandLineParser>
#include <QDir>
#include <QFile>

Initializer::Initializer()
{

}


void Initializer::parseCmdParamsToConfig(QCoreApplication &app, AppConfig &config)
{
    QuarantineChecker::checkAndQuitIfQuarantined();

    QCommandLineParser parser;
    parser.setApplicationDescription("PeasyGUI");
    parser.addHelpOption();

    QCommandLineOption protocolOpt("protocol", "Protocol: tcp or filebased", "protocol");
    QCommandLineOption portOpt("port", "TCP port", "port", "10000");
    QCommandLineOption ipOpt("ip", "Bind to this IP address", "ip", "127.0.0.1");
    QCommandLineOption exchangeOpt("exchange_dir", "Temp dir for info exchange in filebased mode", "exchange_dir");
    QCommandLineOption iniOpt("ini", "Path of the initialization file", "ini", "PeasyGUI.ini");

    parser.addOption(protocolOpt);
    parser.addOption(portOpt);
    parser.addOption(ipOpt);
    parser.addOption(exchangeOpt);
    parser.addOption(iniOpt);

    parser.process(app);

    // Mode of the App -----
    config.mode = AppMode::Designer;
    const QString protocol = parser.value(protocolOpt).toLower();

    if (protocol == "tcp") {
        config.mode = AppMode::TCP;
    } else if (protocol == "filebased") {
        config.mode = AppMode::FileBased;
    }

    // Server settings --
    config.port = parser.value("port").toUShort();
    config.ip = parser.value("ip");

    // Paths and dirs of the textfiles --
    iniPath = AppPaths::cleanAbsPath(parser.value("ini"));
    config.iniPath = iniPath;

    config.exchangeDir = AppPaths::cleanAbsDir(parser.value("exchange_dir"));
}


void Initializer::writeToIniFile(const QString &content)
{
    QFile filehandle(iniPath);
    if (filehandle.open(QIODevice::WriteOnly)) {
        QTextStream stream(&filehandle);
        QString normalizedContent = content;
        normalizedContent.remove('\r');
        stream << normalizedContent;
    }
    filehandle.close();
}


void Initializer::readIniFile()
{
    QString content = "";

    QFile file(iniPath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray rawData = file.readAll();
        file.close();

        content = QString::fromUtf8(rawData).remove('\r');
    } else {
        qInfo() << "There was no Ini file at:" << iniPath;

        // Setting at least a good initial Window size
        content = "set`Window`Geometry`60,60,300,500`";
    }

    emit iniFileRead(content, false);
}
