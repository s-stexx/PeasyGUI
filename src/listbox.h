// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef LISTBOX_H
#define LISTBOX_H

#include <QListWidget>
#include "control.h"

class ListBox : public QListWidget, public Control
{
    Q_OBJECT

public:
    explicit ListBox(QWidget *parent = nullptr);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void listBoxEvent(const QString &name, const QString &event);

private slots:
    void processClick();
    void processDoubleClick();
    void processSelectionChange();

private:
    QString getSelectedCommaSeparated();
};

#endif // LISTBOX_H
