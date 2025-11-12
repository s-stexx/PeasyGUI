// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef TEXTBOX_H
#define TEXTBOX_H

#include <QPlainTextEdit>
#include "control.h"



class TextBox : public QPlainTextEdit, public Control
{
    Q_OBJECT

public:
    explicit TextBox(QWidget *parent = 0);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void textBoxEvent(const QString &name, const QString &event);

private:
    int lastCountOfReturns = 0;

private slots:
    void processTextBoxContent();
};

#endif // TEXTBOX_H
