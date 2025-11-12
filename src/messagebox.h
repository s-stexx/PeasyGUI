// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QString>

class MessageBox : public QDialog
{
    Q_OBJECT
public:
    explicit MessageBox(const QString &text, QWidget *parent = nullptr);

    static void showMessage(const QString &text, QWidget *parent = nullptr);

private:
    QLabel *label;
    QPushButton *okButton;
};


#endif // MESSAGEBOX_H
