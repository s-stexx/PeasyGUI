// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "button.h"


Button::Button(QWidget *parent) : QPushButton(parent), Control(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    connect(this, &QPushButton::clicked, this, &Button::processClick);
}


void Button::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Text") {
        this->setText(value);
    }
}


QString Button::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Text") {
        value = this->text();
    }

    return value;
}


void Button::processClick()
{
    if (propsDict["Events"].contains("Clicked")) {
        emit buttonEvent(propsDict["Name"], "Clicked");
    }
}



