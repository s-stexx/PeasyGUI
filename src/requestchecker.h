// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef REQUESTCHECKER_H
#define REQUESTCHECKER_H

#include <QObject>
#include "request.h"
#include "controlprototypes.h"
#include "control.h"

using ListBoxTexts = QMap<int, QString>;

class RequestChecker : public QObject
{
    friend class test_requestchecker;
    Q_OBJECT

public:
    static QString checkAll(const QList<Request> &requests, const QList<Control*>& controls);

private:
    static void initializeVirtualsWithExistingControls(QStringList &controlNames, QStringList &controlTypes, ListBoxTexts &entireListBoxTexts, const QList<Control *> &controls);
    static int getControlIndexByName(const QString &name, const QStringList &controlNames);
    static QList<PropInfo> getAllPropInfos(const QString &type, const Prototypes &prototypes);
    static QString getCheckRule(const QString &prop, const QList<PropInfo> &propInfos);
    static void appendVirtual(const QString &name, const QString &type, QStringList &controlNames, QStringList &controlTypes, ListBoxTexts &entireListBoxTexts);
    static void swapOldNameByNewName(const QString &oldName, const QString &newName, QStringList &controlNames);

    static void checkControlExists(QString &result, const int index);
    static void checkOnlyOneInfoRequestInBlock(QString &result, const QList<Request> &requests);
    static void checkIfPropIsSettable(QString& error, const QString &prop, const QList<PropInfo> &propInfos);
    static void checkValidMulti(QString &error, const QString &value, const QString &rule);
    static void checkNewControlNameIsUnique(QString &result, const int index);
    static void checkControlNameContainsColon(QString &result, const QString &name);
    static void checkIfTypeExists(QString &result, const QString &prop, const QList<PropInfo> &propInfos);
    static void checkIfPropExists(QString &result, const QString &prop, const QList<PropInfo> &propInfos);
    static void checkIfSelectionIndecesAreInRangeOfItems(QString& result, const QString& value, const QString &entireTexts);
    static void checkIfWindowShouldBeSpawned(QString &result, const QString &type);
    static void checkAllIntegers(QString &result, const QString &value, const QString &rule);
    static void checkValidSingle(QString &result, const QString &value, const QString &rule);
};

#endif // REQUESTCHECKER_H
