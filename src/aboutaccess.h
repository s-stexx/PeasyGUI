// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef ABOUTACCESS_H
#define ABOUTACCESS_H

#include <QObject>
#include <QWidget>

class QPushButton;
class QAction;

class AboutAccess : public QObject {
    Q_OBJECT
public:
    explicit AboutAccess(QWidget *parent);

    void setHeading(const QString &heading);
    QString heading() const;

    void repositionButton();

private slots:
    void showAbout();

private:
    QWidget *parentWindow;
    QPushButton *aboutButton = nullptr;
    QAction *aboutFromMenuSelected = nullptr;
    QString currentHeading = "SouthWest";
};

#endif // ABOUTACCESS_H
