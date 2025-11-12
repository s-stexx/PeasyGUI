// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "controlhighlighter.h"

#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStringList>

ControlHighlighter::ControlHighlighter()
{

}


void ControlHighlighter::setParentWidget(QWidget* parent) {
    parentWidget = parent;
}


void ControlHighlighter::mark(const QString& geometryString, int id) {

    QStringList parts = geometryString.split(',');

    // remove old frame, if it is still fading
    if (activeHighlights.contains(id) && ! activeHighlights[id].isNull()) {
        activeHighlights[id]->deleteLater();
        activeHighlights.remove(id);
    }

    QLabel* highlightFrame = new QLabel(parentWidget);

    highlightFrame->setGeometry(
        parts[0].toInt() - 2,
        parts[1].toInt() - 2,
        parts[2].toInt() + 4,
        parts[3].toInt() + 4
        );

    highlightFrame->setStyleSheet("border: 2px solid red;");
    highlightFrame->setAttribute(Qt::WA_TransparentForMouseEvents);
    highlightFrame->show();

    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(highlightFrame);
    highlightFrame->setGraphicsEffect(effect);

    QPropertyAnimation* fade = new QPropertyAnimation(effect, "opacity", highlightFrame);
    fade->setDuration(500);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);

    // Delete frame after fading finished
    QObject::connect(fade, &QPropertyAnimation::finished, [highlightFrame, id, this]() {
        highlightFrame->deleteLater();
        activeHighlights.remove(id);
    });

    activeHighlights[id] = highlightFrame;
    fade->start(QAbstractAnimation::DeleteWhenStopped);
}
