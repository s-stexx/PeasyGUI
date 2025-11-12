// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef DROPAREA_H
#define DROPAREA_H

#include <QLabel>
#include "control.h"

class DropArea : public QLabel, public Control
{
    Q_OBJECT

public:
    DropArea(QWidget *parent = nullptr);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void dropAreaEvent(const QString &name, const QString &event);

protected:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

};

#endif // DROPAREA_H
