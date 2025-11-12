// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "iconfreetooltip.h"

#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>


bool IconFreeToolTip::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        auto *helpEvent = static_cast<QHelpEvent *>(event);
        QString text = obj->property("toolTip").toString();

        if (!text.isEmpty()) {
            QToolTip::showText(helpEvent->globalPos(), text);
            return true;
        }
    }

    return QObject::eventFilter(obj, event);
}
