// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "pathdialogbutton.h"

#include "apppaths.h"

#include <QFileDialog>
#include <QString>

PathDialogButton::PathDialogButton(QWidget *parent) : QPushButton(parent), Control(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(this, &QPushButton::clicked, this, &PathDialogButton::processClick);
}


void PathDialogButton::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Text") {
        propsDict["Text"] = value;

    } else if (prop == "Icon") {
        propsDict["Icon"] = value;

        this->setIcon(QIcon(AppPaths::cleanAbsPath(value)));
        this->setIconSize(QSize(this->width()-10, this->height()-10));
    }
}


QString PathDialogButton::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Text") {
        value = propsDict["Text"];

    } else if (prop == "Icon") {
        value = propsDict["Icon"];

    }

    return value;
}


void PathDialogButton::processClick()
{
    QString pathOrDir;
    QString suggestion;

    if (propsDict["Category"] == "Path") {
        suggestion = AppPaths::cleanAbsPath(propsDict["Suggestion"]);

        if (propsDict["Type"] == "OpenButton") {
            pathOrDir = QFileDialog::getOpenFileName(
                this,
                propsDict["Text"],
                suggestion,
                propsDict["Extensions"]
                );

        } else {
            pathOrDir = QFileDialog::getSaveFileName(
                this,
                propsDict["Text"],
                suggestion,
                propsDict["Extensions"]
                );
        }

    } else if (propsDict["Category"] == "Dir") {
        suggestion = AppPaths::cleanAbsDir(propsDict["Suggestion"]);

        pathOrDir = QFileDialog::getExistingDirectory(
            this,
            propsDict["Text"],
            suggestion
        );
    }

    if ( ! pathOrDir.isEmpty()) {
        propsDict["Path"] = pathOrDir;

        if (propsDict["Events"].contains("Chosen")) {
            emit pathDialogButtonEvent(propsDict["Name"], "Chosen`" + pathOrDir);
        }
    }
}
