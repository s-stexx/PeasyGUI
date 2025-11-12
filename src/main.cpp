// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

/*
                    PeasyGUI

A lightweight and generic GUI for any programming language.
 
This main entry point wires together the essential components and runtime
modes via Qt signals and slots.

Components:
 - Initializer:     Parses command-line arguments, reads/writes INI files.

 - MainWin:         The primary GUI window. Handles incoming requests, manages
                    all active controls, and dispatches updates to them.

 - ToolBoxWin:      Design-time utility window. Lets you add controls,
                    inspect and modify their properties interactively.

 - Server:          TCP server interface for runtime communication
                    with user programs.

 - FileBasedServer: Alternative file-based interface for platforms
                    or languages where TCP is inconvenient.

 Modes:
 - Designer: GUI layout editor.
 - TCP: Live program-to-GUI communication via TCP.
 - FileBased: Communication via temporary files.
 
*/

#include <QApplication>

#include "initializer.h"
#include "mainwin.h"
#include "toolboxwin.h"
#include "server.h"
#include "filebasedserver.h"
#include "iconfreetooltip.h"


int main(int argc, char *argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication app(argc, argv);

    app.installEventFilter(new IconFreeToolTip);

    AppConfig config;

    Initializer initializer;

    initializer.parseCmdParamsToConfig(app, config);

    MainWin mainWin(config);

    QObject::connect(&initializer, &Initializer::iniFileRead, &mainWin, &MainWin::handleIniFileReadReady);

    mainWin.show();

    initializer.readIniFile();

    if (config.mode == AppMode::Designer) {
        ToolBoxWin* toolBoxWin = new ToolBoxWin;

        QObject::connect(toolBoxWin, &ToolBoxWin::requestInitialSelectorFilling, &mainWin, &MainWin::collectAndEmitControlNames);
        QObject::connect(&mainWin, &MainWin::propsOfControlCollected, toolBoxWin, &ToolBoxWin::updatePropTable);
        QObject::connect(toolBoxWin, &ToolBoxWin::spawnControlClicked, &mainWin, &MainWin::treatRequests);
        QObject::connect(&mainWin, &MainWin::newControlSpawned, toolBoxWin, &ToolBoxWin::addToControlSelector);
        QObject::connect(&mainWin, &MainWin::nameOfControlHasChanged, toolBoxWin, &ToolBoxWin::swapControlNameInSelector);
        QObject::connect(toolBoxWin, &ToolBoxWin::controlSelected, &mainWin, &MainWin::collectPropsOfControl);
        QObject::connect(toolBoxWin, &ToolBoxWin::controlPropChanged, &mainWin, &MainWin::treatRequests);
        QObject::connect(&mainWin, &MainWin::appClosedControlsDataCollected, &initializer, &Initializer::writeToIniFile);

        toolBoxWin->start();
        mainWin.activateWindow();

    } else if (config.mode == AppMode::TCP) {
        Server* server = new Server(config);

        QObject::connect(&mainWin, &MainWin::replyForClientPrepared, server, &Server::sendReplyToClient);
        QObject::connect(&mainWin, &MainWin::shutdownConnections, server, &Server::quit);
        QObject::connect(server, &Server::messageFromClientReceived, &mainWin, &MainWin::treatRequests);
        QObject::connect(&app, &QCoreApplication::aboutToQuit, server, &Server::quit);

        server->start();

    } else if (config.mode == AppMode::FileBased) {
        FileBasedServer* fileServer = new FileBasedServer(config);

        QObject::connect(&mainWin, &MainWin::replyForClientPrepared, fileServer, &FileBasedServer::sendReplyToClient);
        QObject::connect(&mainWin, &MainWin::shutdownConnections, fileServer, &FileBasedServer::quit);
        QObject::connect(fileServer, &FileBasedServer::messageFromClientReceived, &mainWin, &MainWin::treatRequests);

        fileServer->start();

    }

    return app.exec();
}
