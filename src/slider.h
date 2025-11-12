// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include "control.h"

class Slider : public QSlider, public Control
{
    Q_OBJECT

public:
    explicit Slider(QWidget *parent = 0);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

signals:
    void sliderEvent(const QString &name, const QString &event);

protected:
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;

};

#endif // SLIDER_H
