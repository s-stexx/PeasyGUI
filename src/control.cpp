// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "control.h"

#include <QMouseEvent>

Control::Control(QObject* parent)
{
    dynParent = parent;
}


void Control::setProp(const QString &prop, const QString &value)
{
    // Expand for new props
    if (   (prop == "Type")
        || (prop == "Name")
        || (prop == "Events")
        || (prop == "MultiLine")
        || (prop == "Category")
        || (prop == "Extensions")
        || (prop == "Suggestion")
        || (prop == "Path")
        || (prop == "EntirePaths")
        || (prop == "DragStyle")
        ) {

        propsDict[prop] = value;

        // One exception: A timer needs to become immediately active
        if (propsDict["Type"] == "Window" && prop == "Events") {
            setSpecificProp(prop, value);
        }

    } else if (    (prop == "Text")
                || (prop == "MultiSelect")
                || (prop == "EntireTexts")
                || (prop == "Icon")
                || (prop == "Image")
                || (prop == "Scaling")
                || (prop == "Opacity")
                || (prop == "Offsets")
                || (prop == "State")
                || (prop == "QuitNow")
                || (prop == "Clipboard")
                || (prop == "Selected")
                || (prop == "Value")
                || (prop == "Min")
                || (prop == "Max")
                || (prop == "Orientation")
                || (prop == "AboutHeading")
                || (prop == "MessageBoxNow")
                ) {

        setSpecificProp(prop, value);

    } else {
        // Apply more general property to Widget -----
        auto widget = dynamic_cast<QWidget*>(this);

        if (prop == "Style") {
            propsDict[prop] = value;
            widget->setStyleSheet(value);

        } else if (prop == "ToolTip") {
            widget->setToolTip(value);

        } else if (prop == "Show") {     // Different at getProp
            propsDict[prop] = value;

            if (controlMightGetInactive) {
                if (value == "Normal") {
                    widget->show();
                    widget->setEnabled(true);

                } else if (value == "Disabled") {
                    widget->show();
                    widget->setEnabled(false);

                } else if (value == "Invisible") {
                    widget->hide();

                }
            }

        } else if (prop == "Geometry") {
            QStringList data = value.split(",");

            widget->setGeometry(data[0].toInt(), data[1].toInt(), data[2].toInt(), data[3].toInt());
            widget->show();

        }
    }
}


QString Control::getProp(const QString &prop)
{
    QString value = "";
    // Expand for new props
    if (   (prop == "Type")
        || (prop == "Name")
        || (prop == "Events")
        || (prop == "Show")      // Different at setProp
        || (prop == "MultiLine")
        || (prop == "Category")
        || (prop == "Extensions")
        || (prop == "Suggestion")
        || (prop == "Path")
        || (prop == "EntirePaths")
        || (prop == "DragStyle")
        ) {

        value = propsDict[prop];

    } else if (    (prop == "Text")
                || (prop == "MultiSelect")
                || (prop == "EntireTexts")
                || (prop == "Icon")
                || (prop == "Image")
                || (prop == "Scaling")
                || (prop == "Opacity")
                || (prop == "Offsets")
                || (prop == "Dimensions")  // Read-Only, no setProp
                || (prop == "State")
                || (prop == "QuitNow")
                || (prop == "Clipboard")
                || (prop == "Selected")
                || (prop == "Value")
                || (prop == "Min")
                || (prop == "Max")
                || (prop == "Orientation")
                || (prop == "AboutHeading")
                || (prop == "MessageBoxNow")
                ) {

        value = getSpecificProp(prop);

    } else {
        // Get more general property from Widget -----
        auto widget = dynamic_cast<QWidget*>(this);

        if (prop == "Style") {
            value = widget->styleSheet();

        } else if (prop == "ToolTip") {
            value = widget->toolTip();

        } else if (prop == "Geometry") {
            int data[4];

            widget->geometry().getRect(&data[0], &data[1], &data[2], &data[3]);

            value = QString::number(data[0]) + "," + QString::number(data[1]) + "," +
                    QString::number(data[2]) + "," + QString::number(data[3]);
        }
    }

    return value;
}


QList<PropInfo> Control::getAllPropInfos() {
    const auto& prototypes = getPrototypes();

    auto protoProps = prototypes.find(getProp("Type"));
    if (protoProps != prototypes.end()) {
        return protoProps.value();
    }

    return {};
}


int Control::applyDefaultProperties(int typeCount, int yPos)
{
    int height = 32;
    const auto& propsInfos = getAllPropInfos();
    // Magic number 2: 0 = Name, 1 = Type are covered by "new"
    for (int idxProp = 2; idxProp < propsInfos.size(); ++idxProp) {

        // Init with default value
        QString propValue = propsInfos[idxProp].defaultValue;

        // "W" flag = were written to file before (if different) otherwise use default
        if ( ! propValue.isEmpty() && propsInfos[idxProp].flagsSetWrite.contains("W") ) {

            if (propValue.contains("#TYPECOUNT#")) {
                propValue.replace("#TYPECOUNT#", QString::number(typeCount));
            }

            if (propValue.contains("#YPOS#")) {
                propValue.replace("#YPOS#", QString::number(yPos));
                QStringList tokens = propValue.split(",");
                height = tokens.last().toInt();
            }

            setProp(propsInfos[idxProp].propName, propValue);
        }
    }

    return height;
}


int Control::getId() const
{
    return id;
}


void Control::setId(int newId)
{
    id = newId;
}


void Control::setControlMightGetInactive(bool newControlMightGetInactive)
{
    controlMightGetInactive = newControlMightGetInactive;
}


