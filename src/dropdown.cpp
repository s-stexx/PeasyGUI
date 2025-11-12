// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "dropdown.h"

#include "QStringList"
#include "QString"

DropDown::DropDown(QWidget *parent) : QComboBox(parent), Control(parent)
{
    connect(this, &QComboBox::currentIndexChanged, this, &DropDown::processSelectionChange);
}


void DropDown::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "EntireTexts") {
        this->clear();
        QStringList items = value.split("`");

        this->addItems(items);

    } else if (prop == "Selected") {
        int selectedIndex = 0;

        if (value.trimmed().isEmpty()) {
            selectedIndex = -1;
        } else {
            selectedIndex = value.toInt();
        }

        this->setCurrentIndex(selectedIndex);
    }
}


QString DropDown::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "EntireTexts") {
        for (int row = 0; row < this->count(); row++) {
            value = value + this->itemText(row) + "`";
        }
        if (value.contains("`")) {
            value.chop(1);
        }

    } else if (prop == "Selected") {
        value = QString::number(this->currentIndex());

        if (value == "-1") {
            value = "";
        }

    }

    return value;
}


void DropDown::processSelectionChange()
{
    if (propsDict["Events"].contains("SelectionChanged")) {
        emit dropDownEvent(propsDict["Name"], "SelectionChanged`" + QString::number(this->currentIndex()));
    }
}

