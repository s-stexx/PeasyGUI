// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "textbox.h"


TextBox::TextBox(QWidget *parent) : QPlainTextEdit(parent), Control(parent)
{
    this->setWordWrapMode(QTextOption::NoWrap);
    this->tabChangesFocus();
    this->setTabChangesFocus(true);

    connect(this, &QPlainTextEdit::textChanged, this, &TextBox::processTextBoxContent);
}


void TextBox::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "Text") {
        this->setPlainText(value);
    }
}


QString TextBox::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "Text") {
        value = this->toPlainText();
    }

    return value;
}


void TextBox::processTextBoxContent()
{
    int countOfReturns = this->toPlainText().count(QLatin1Char('\n'));

    if (countOfReturns > lastCountOfReturns) {
        if (propsDict["Events"].contains("ReturnPressed")) {
            emit textBoxEvent(propsDict["Name"], "ReturnPressed");
        }
        if (propsDict["MultiLine"] == "False") {
            // without blockSignals a endless loop would lead to recursion resulting in a crash
            this->blockSignals(true);
            this->setPlainText(this->toPlainText().replace("\n", ""));
            this->blockSignals(false);

            countOfReturns = 0;
        }
    }

    lastCountOfReturns = countOfReturns;
}

