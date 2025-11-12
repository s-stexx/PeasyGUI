// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "aboutaccess.h"
#include "aboutdialog.h"

#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMainWindow>

AboutAccess::AboutAccess(QWidget *parent)
    : QObject(parent) {
    parentWindow = parent;
#ifdef Q_OS_MACOS
    QMenu *helpMenu = static_cast<QMainWindow*>(parentWindow)->menuBar()->addMenu("Help");
    aboutFromMenuSelected = helpMenu->addAction("About App");
    connect(aboutFromMenuSelected, &QAction::triggered, this, &AboutAccess::showAbout);
#else
    aboutButton = new QPushButton("?", parentWindow);
    aboutButton->setToolTip("About");
    aboutButton->setCursor(Qt::PointingHandCursor);
    aboutButton->setFlat(true);
    aboutButton->resize(20, 20);
    aboutButton->raise();
    connect(aboutButton, &QPushButton::clicked, this, &AboutAccess::showAbout);

    repositionButton();
#endif
}

void AboutAccess::setHeading(const QString &heading) {
    currentHeading = heading;
    repositionButton();
}

QString AboutAccess::heading() const {
    return currentHeading;
}

void AboutAccess::showAbout() {
    AboutDialog dlg(parentWindow);
    dlg.exec();
}

void AboutAccess::repositionButton() {
#ifndef Q_OS_MACOS
    aboutButton->raise();

    int w = parentWindow->width();
    int h = parentWindow->height();
    int bw = aboutButton->width();
    int bh = aboutButton->height();

    if (currentHeading == "North")
        aboutButton->setGeometry(w/2 - bw/2, 2, bw, bh);
    else if (currentHeading == "NorthEast")
        aboutButton->setGeometry(w - bw - 2, 2, bw, bh);
    else if (currentHeading == "East")
        aboutButton->setGeometry(w - bw - 2, h/2 - bh/2, bw, bh);
    else if (currentHeading == "SouthEast")
        aboutButton->setGeometry(w - bw - 2, h - bh - 2, bw, bh);
    else if (currentHeading == "South")
        aboutButton->setGeometry(w/2 - bw/2, h - bh - 2, bw, bh);
    else if (currentHeading == "SouthWest")
        aboutButton->setGeometry(2, h - bh - 2, bw, bh);
    else if (currentHeading == "West")
        aboutButton->setGeometry(2, h/2 - bh/2, bw, bh);
    else if (currentHeading == "NorthWest")
        aboutButton->setGeometry(2, 2, bw, bh);
#endif
}
