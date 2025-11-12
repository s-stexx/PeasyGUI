// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef ICONFREETOOLTIP_H
#define ICONFREETOOLTIP_H

#include <QObject>

// I see it as a bug:
// If the StyleSheet for any control contains a image file,
// the image is rendered into the ToolTip aswell.
// So, this class exists to prevent this behaviour. It can
// maybe be removed if Qt changes behaviour. To detect current
// version write:
//
// #include <QtGlobal>
// qDebug() << QT_VERSION_STR;
//
// Mine is 6.8.1

class IconFreeToolTip : public QObject {

public:
    bool eventFilter(QObject *obj, QEvent *event) override;

};

#endif // ICONFREETOOLTIP_H
