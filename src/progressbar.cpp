// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent) : QProgressBar(parent), Control(parent)
{

}

void ProgressBar::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Value") {
        this->setValue(value.toInt());
    }
}


QString ProgressBar::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Value") {
        value = QString::number(this->value());
    }

    return value;
}
