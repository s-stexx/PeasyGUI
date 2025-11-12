// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "slider.h"

Slider::Slider(QWidget *parent) : QSlider(parent), Control(parent)
{

}


void Slider::setSpecificProp(const QString &prop, const QString &value)
{
    if (prop == "Value") {
        this->setSliderPosition(value.toInt());

    } else if (prop == "Min") {
        this->setMinimum(value.toInt());

    } else if (prop == "Max") {
        this->setMaximum(value.toInt());

    } else if (prop == "Orientation") {
        if (value == "Horizontal") {
            this->setOrientation(Qt::Horizontal);

        } else {
            this->setOrientation(Qt::Vertical);

        }
    }
}


QString Slider::getSpecificProp(const QString &prop)
{
    QString value;

    if (prop == "Value") {
        value = QString::number(this->sliderPosition());

    } else if (prop == "Min") {
        value = QString::number(this->minimum());

    } else if (prop == "Max") {
        value = QString::number(this->maximum());

    } else if (prop == "Orientation") {
        if (this->orientation() == Qt::Horizontal) {
            value = "Horizontal";

        } else {
            value = "Vertical";

        }

    }

    return value;
}


void Slider::mouseMoveEvent(QMouseEvent *ev)
{
    if (propsDict["Events"].contains("Moved")) {
        emit sliderEvent(propsDict["Name"], "Moved`" + QString::number(this->sliderPosition()));
    }

    QSlider::mouseMoveEvent(ev);
}


void Slider::mousePressEvent(QMouseEvent *ev)
{
    if (propsDict["Events"].contains("Pressed")) {
        emit sliderEvent(propsDict["Name"], "Pressed`" + QString::number(this->sliderPosition()));
    }

    QSlider::mousePressEvent(ev);
}


void Slider::mouseReleaseEvent(QMouseEvent *ev)
{
    if (propsDict["Events"].contains("Released")) {
        emit sliderEvent(propsDict["Name"], "Released`" + QString::number(this->sliderPosition()));
    }

    QSlider::mouseReleaseEvent(ev);
}
