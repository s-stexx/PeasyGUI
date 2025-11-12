// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "droparea.h"

#include <QMimeData>
#include <QDragEnterEvent>

DropArea::DropArea(QWidget *parent) : QLabel(parent)
{
    setAcceptDrops(true);
}


void DropArea::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Text") {
        this->setText(value);
    }
}


QString DropArea::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Text") {
        value = this->text();
    }

    return value;
}


void DropArea::dropEvent(QDropEvent *event)
{
    QString strUrls = "";

    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QStringList list;
        for (const QUrl &url : mimeData->urls()) {
            QString localPath = url.toLocalFile();
            if (!localPath.isEmpty()) {
                list << localPath;
            }
        }
        strUrls = list.join("`");
    }

    propsDict["EntirePaths"] = strUrls;

    if (propsDict["Events"].contains("Dropped")) {
        emit dropAreaEvent(propsDict["Name"], "Dropped`" + strUrls);
    }

    this->setStyleSheet(propsDict["Style"]);
    event->acceptProposedAction();
}


void DropArea::dragEnterEvent(QDragEnterEvent *event)
{
    this->setStyleSheet(propsDict["DragStyle"]);
    event->accept();
}


void DropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    this->setStyleSheet(propsDict["Style"]);
}

