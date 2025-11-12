// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "listbox.h"
#include "QStringList"
#include "QString"

ListBox::ListBox(QWidget *parent) : QListWidget(parent), Control(parent)
{
    connect(this, &QListWidget::itemClicked, this, &ListBox::processClick);
    connect(this, &QListWidget::itemDoubleClicked, this, &ListBox::processDoubleClick);
    connect(this, &QListWidget::itemSelectionChanged, this, &ListBox::processSelectionChange);
}


void ListBox::setSpecificProp(const QString& prop, const QString& value)
{
    if (prop == "EntireTexts") {
        this->clear();
        QStringList items = value.split("`");

        if (! (items.size() == 1 && items[0].isEmpty())) {
            this->addItems(items);
        }

        this->clearSelection();

    } else if (prop == "MultiSelect") {
        if (value == "True") {
            this->setSelectionMode(QAbstractItemView::ExtendedSelection);
        } else {
            this->setSelectionMode(QAbstractItemView::SingleSelection);
        }

        this->clearSelection();


    } else if (prop == "Selected") {
        QStringList wishSelected = value.split(",");

        QSignalBlocker blocker(this);
        this->clearSelection();

        for (int idxWish = 0; idxWish < wishSelected.size(); idxWish++) {
            if ( ! wishSelected[idxWish].isEmpty()) {
                this->item(wishSelected[idxWish].toInt())->setSelected(true);
            }
        }
    }
}


QString ListBox::getSpecificProp(const QString& prop)
{
    QString value;

    if (prop == "EntireTexts") {
        for (int row = 0; row < this->count(); row++) {
            value = value + this->item(row)->text() + "`";
        }

        if (value.contains("`")) {
            value.chop(1);
        }

    } else if (prop == "MultiSelect") {
        if (this->selectionMode() == QAbstractItemView::ExtendedSelection) {
            value = "True";
        } else {
            value = "False";
        }

    } else if (prop == "Selected") {
        value = getSelectedCommaSeparated();

    }

    return value;
}


void ListBox::processClick()
{
    // The word "DoubleClicked" contains the Word "Clicked"
    QString workaroundPartOfName = "," + propsDict["Events"];
    if (workaroundPartOfName.contains(",Clicked")) {
        emit listBoxEvent(propsDict["Name"], "Clicked`" + QString::number(this->currentRow()));
    }
}


void ListBox::processDoubleClick()
{
    if (propsDict["Events"].contains("DoubleClicked")) {
        emit listBoxEvent(propsDict["Name"], "DoubleClicked`" + QString::number(this->currentRow()));
    }
}


void ListBox::processSelectionChange()
{
    if (propsDict["Events"].contains("SelectionChanged")) {
        QString commaSeparated = getSelectedCommaSeparated();

        emit listBoxEvent(propsDict["Name"], "SelectionChanged`" + commaSeparated);
    }
}


QString ListBox::getSelectedCommaSeparated()
{
    QStringList selectedList;

    for (QModelIndex index: this->selectedIndexes()){
        selectedList.append(QString::number(index.row()));

    }

    return selectedList.join(",");
}

