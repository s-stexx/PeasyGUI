// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef CONTROLHIGHLIGHTER_H
#define CONTROLHIGHLIGHTER_H

#include <QMap>
#include <QPointer>

class QLabel;
class QWidget;

class ControlHighlighter {
public:
    ControlHighlighter();

    void setParentWidget(QWidget* parent);
    void mark(const QString& geometryString, int id);

private:
    QWidget* parentWidget = nullptr;
    QMap<int, QPointer<QLabel>> activeHighlights;
};

#endif // CONTROLHIGHLIGHTER_H
