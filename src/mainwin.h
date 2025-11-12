// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QTimer>

#include "aboutaccess.h"
#include "appconfig.h"
#include "control.h"
#include "controlhighlighter.h"

class MainWin : public QMainWindow, public Control
{
    Q_OBJECT

public:
    MainWin(const AppConfig &config, QWidget *parent = nullptr);
    
    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;
    QWidget *detectControlUnderCursor(QEvent *event);

signals:
    void propsOfControlCollected(const QString &type, const QStringList &controlValues, int id);
    void appClosedControlsDataCollected(const QString& content);
    void shutdownConnections();
    void replyForClientPrepared(const QString& content);
    void windowEvent(const QString &name, const QString &event);
    void newControlSpawned(const QString& nameOfControl);
    void nameOfControlHasChanged(const QString &oldName, const QString &newName);

public slots:
    void treatRequests(const QString& content, bool replyExpected);
    void handleIniFileReadReady(const QString& content, bool replyExpected);
    void addToEventQueue(const QString& name, const QString& event);
    void collectAndEmitControlNames();
    void collectPropsOfControl(int id);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    AboutAccess *aboutAccess;
    QList<Control*> controls;
    QStringList eventList;
    QTimer resizeEndTimer;
    QTimer pollClipboardTimer;
    QClipboard *clipboard;
    QString lastClipboard;
    AppMode mode;
    ControlHighlighter highlighter;

    int yPositionForSpawnedControls = 10;
    const int idOfMainWindow = 0;
    bool isFirstResize = true;
    bool designModeInitReady = false;

    void closeEvent(QCloseEvent *event) override;
    void spawnControl(const QString &type, const QString &name);
    void fulfillSetRequest(const QString &name, const QString &prop, const QString &value);
    int countControlsOfThisType(const QString &type);
    QString generateUniqueNameIfEmpty(const QString &name, const QString &type, int typeCount);
    int findControlIndexByName(const QString &name);
    void connectControlEvents(Control *control, const QString &type);
    bool interceptKeyForCustomEvent(QKeyEvent *event);
    QString convertKeysToString(QKeyEvent *event);
    QString fulfillInformationRequest(const QString &action, const QString &name, const QString &prop);
    void highlightControl(int id);

private slots:
    void onResizeFinished();
    void pollClipboard();
};

#endif
