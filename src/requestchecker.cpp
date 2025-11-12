// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "requestchecker.h"

#define RETURN_IF_ERROR if ( ! result.isEmpty()) return result;

void RequestChecker::checkOnlyOneInfoRequestInBlock(QString &result, const QList<Request> &requests)
{
    int infoRequests = 0;
    int changeRequests = 0;

    for (const auto& request : requests) {
        if (request.action == "events?" || request.action == "get") {
            infoRequests++;
        } else {
            changeRequests++;
        }
    }

    if (infoRequests > 1 || (infoRequests == 1 && changeRequests > 0)) {
        result = "The actions 'events?' or 'get' might not be combined with any other action (even theirselfs) in the same request";
    }
}

void RequestChecker::initializeVirtualsWithExistingControls(QStringList &controlNames, QStringList &controlTypes, ListBoxTexts &entireListBoxTexts, const QList<Control *> &controls)
{
    QString type;

    for (int idxControl = 0; idxControl < controls.size(); idxControl++) {
        controlNames.append(controls[idxControl]->getProp("Name"));

        type = controls[idxControl]->getProp("Type");
        controlTypes.append(type);
        if (type == "ListBox" || type == "DropDown") {
            entireListBoxTexts[idxControl] = controls[idxControl]->getProp("EntireTexts");
        }
    }
}

void RequestChecker::appendVirtual(const QString &name, const QString &type, QStringList &controlNames, QStringList &controlTypes, ListBoxTexts &entireListBoxTexts)
{
    if (type == "ListBox"  || type == "DropDown") {
        entireListBoxTexts[controlNames.size()] = "";
    }
    controlNames.append(name);
    controlTypes.append(type);
}

QList<PropInfo> RequestChecker::getAllPropInfos(const QString& type, const Prototypes &prototypes) {
    auto protoProps = prototypes.find(type);
    if (protoProps != prototypes.end()) {
        return protoProps.value();
    }

    return {};
}

int RequestChecker::getControlIndexByName(const QString &name, const QStringList &controlNames)
{
    int idxFound = -1;
    int idx = 0;

    while((idxFound == -1) && (idx < controlNames.size())) {
        if (controlNames[idx] == name) {
            idxFound = idx;
        }
        idx++;
    }

    return idxFound;
}

void RequestChecker::checkControlExists(QString &result, const int index)
{
    if (index == -1) {
        result = "A control with this name could not be found";
    }
}

void RequestChecker::checkNewControlNameIsUnique(QString &result, const int index)
{
    if ( ! (index == -1)) {
        result = "A control with this name does already exist";
    }
}

void RequestChecker::checkControlNameContainsColon(QString &result, const QString &name)
{
    if (name.contains(":")) {
        result = "A control might not have a ':' in his name";
    }
}

void RequestChecker::checkValidMulti(QString& result, const QString& value, const QString& rule) {
    if (value.trimmed().isEmpty()) {
        return;
    }

    QStringList allowed = rule.mid(sizeof("Multi:") - 1).split(",");
    QStringList values = value.split(",");

    for (const QString& value : values) {
        if ( ! allowed.contains(value)) {
            result = "At least one of the values is not in the list of possibles";
            return;
        }
    }
}

void RequestChecker::checkAllIntegers(QString& result, const QString& value, const QString& rule) {
    QStringList allowed = rule.mid(sizeof("Int:") - 1).split(",");
    QStringList values = value.split(",");

    if ( ! (allowed.size() == values.size())) {
        result = "Wrong number of Integers given";
        return;
    }

    bool conversionOk;
    int thisValue;

    for (int idxBoth = 0; idxBoth < values.size(); idxBoth++) {

        thisValue = values[idxBoth].toInt(&conversionOk);

        if ( ! conversionOk) {
            result = "All comma separated values must be integers";
            return;
        }

        if (allowed[idxBoth] == "U" && thisValue < 0) {
            result = "Found negative value, where positive is expected";
            return;
        }

    }
}

void RequestChecker::checkValidSingle(QString& result, const QString& value, const QString& rule) {
    if (value.trimmed().isEmpty()) {
        result = "Value might not be empty";
        return;
    }

    QStringList allowed = rule.mid(sizeof("Single:") - 1).split(",");

    if ( ! allowed.contains(value)) {
        result = "Value must be one of the possible choices";
        return;
    }

}

void RequestChecker::checkIfPropIsSettable(QString& result, const QString &prop, const QList<PropInfo>& propInfos) {
    for (const PropInfo& propInfo : propInfos) {
        if (propInfo.propName == prop) {
            if ( ! propInfo.flagsSetWrite.contains("S")) {
                result = "Set not allowed for this property";
            }
            return;
        }
    }
}

void RequestChecker::checkIfTypeExists(QString& result, const QString &prop, const QList<PropInfo>& propInfos) {
    if (prop != "Type" || propInfos.isEmpty()) {
        result = "The 'new' command must have the property 'Type' and an existing control type as value like e.g. 'Button'";
    }
}

void RequestChecker::checkIfPropExists(QString& result, const QString &prop, const QList<PropInfo>& propInfos) {
    for (const PropInfo& propInfo : propInfos) {
        if (propInfo.propName == prop) {
            return;
        }
    }

    result = "Unknown property";
}

QString RequestChecker::getCheckRule(const QString &prop, const QList<PropInfo>& propInfos) {
    for (const PropInfo& propInfo : propInfos) {
        if (propInfo.propName == prop) {
            return propInfo.checkRule;
        }
    }

    return "Internal Error";
}

void RequestChecker::swapOldNameByNewName(const QString &oldName, const QString &newName, QStringList &controlNames)
{
    for (int i = 0; i < controlNames.size(); i++) {
        if (controlNames[i] == oldName) {
            controlNames[i] = newName;
            return;
        }
    }
}

void RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(QString& result, const QString& value, const QString &entireTexts)
{
    bool conversionOk;
    int idxSelection;

    QStringList items = entireTexts.split("`");
    QStringList selections = value.split(",");

    // prepare, items, like it is done in ListBox: {""} means no items
    if (items.size() == 1 && items[0].isEmpty()) {
        items.clear();
    }

    // Empty "" is a valid option to select none
    if ( ! (selections.size() == 1 && selections[0].isEmpty()) ) {
        for (const QString& selection : selections) {
            idxSelection = selection.toInt(&conversionOk);

            if ( ! conversionOk) {
                result = "All comma separated list indeces must be positive integers";
                return;
            }

            if (idxSelection < 0 || idxSelection >= items.size()) {
                result = "List indeces must be in range (0 ... item_count-1)";
                return;
            }
        }
    }

}

void RequestChecker::checkIfWindowShouldBeSpawned(QString& result, const QString& type)
{
    if (type == "Window") {
        result = "PeasyGUI is a one window application";
    }
}

QString RequestChecker::checkAll(const QList<Request> &requests, const QList<Control *> &controls)
{
    QString result;
    QStringList controlNames;
    QStringList controlTypes;
    int index;
    QList<PropInfo> propInfos;
    QMap<int, QString> entireListBoxTexts;

    checkOnlyOneInfoRequestInBlock(result, requests);
    RETURN_IF_ERROR

    initializeVirtualsWithExistingControls(controlNames, controlTypes, entireListBoxTexts, controls);

    const Prototypes& prototypes = getPrototypes();

    for (const auto& request : requests) {
        index = getControlIndexByName(request.name, controlNames);

        if (request.action == "new") { // ##########################
            checkNewControlNameIsUnique(result, index);
            RETURN_IF_ERROR

            checkControlNameContainsColon(result, request.name);
            RETURN_IF_ERROR

            propInfos = getAllPropInfos(request.value, prototypes);

            checkIfTypeExists(result, request.prop, propInfos);
            RETURN_IF_ERROR

            checkIfWindowShouldBeSpawned(result, request.value);
            RETURN_IF_ERROR

            appendVirtual(request.name, request.value, controlNames, controlTypes, entireListBoxTexts);

        } else if (request.action == "get") { // ##########################
            checkControlExists(result, index);
            RETURN_IF_ERROR

            propInfos = getAllPropInfos(controlTypes[index], prototypes);

            checkIfPropExists(result, request.prop, propInfos);
            RETURN_IF_ERROR

        } else if (request.action == "set") { // ##########################
            checkControlExists(result, index);
            RETURN_IF_ERROR

            propInfos = getAllPropInfos(controlTypes[index], prototypes);

            checkIfPropExists(result, request.prop, propInfos);
            RETURN_IF_ERROR

            checkIfPropIsSettable(result, request.prop, propInfos);
            RETURN_IF_ERROR

            QString checkRule = getCheckRule(request.prop, propInfos);

            if (checkRule == "UniqueNColon") {
                int indexOfNew = getControlIndexByName(request.value, controlNames);

                checkNewControlNameIsUnique(result, indexOfNew);
                RETURN_IF_ERROR

                checkControlNameContainsColon(result, request.value);
                RETURN_IF_ERROR

                swapOldNameByNewName(request.name, request.value, controlNames);

            } else if (checkRule == "IndecesInRange") {
                checkIfSelectionIndecesAreInRangeOfItems(result, request.value, entireListBoxTexts[index]);
                RETURN_IF_ERROR

            } else if (checkRule.startsWith("Int:")) {
                checkAllIntegers(result, request.value, checkRule);
                RETURN_IF_ERROR

            } else if (checkRule.startsWith("Multi:")) {
                checkValidMulti(result, request.value, checkRule);
                RETURN_IF_ERROR

            } else if (checkRule.startsWith("Single:")) {
                checkValidSingle(result, request.value, checkRule);
                RETURN_IF_ERROR
            }

            if (request.prop == "EntireTexts") {
                entireListBoxTexts[index] = request.value;
            }

        }

    }

    return result;
}
