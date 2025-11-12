// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "toolboxwin.h"

#include <QSpinBox>


ToolBoxWin::ToolBoxWin(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Toolbox");
    setGeometry(Geom::Win.X, Geom::Win.Y, Geom::Win.W, Geom::Win.H);

    QWidget::setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    placeToolBoxIconButtons();
    placeControlSelector();
    placePropertyTable();
    placeEditorContainer();
}


void ToolBoxWin::start()
{
    emit requestInitialSelectorFilling();

    this->show();
}


void ToolBoxWin::iconButtonClicked(int type)
{
    emit spawnControlClicked("new``Type`" + controlNames[type] + "`", false);
}

void ToolBoxWin::controlViaSelectorChosen(int id)
{
    emit controlSelected(id);
}


void ToolBoxWin::swapControlNameInSelector(const QString& oldName, const QString& newName)
{
    int index = controlSelector->findText(oldName);
    if (index != -1) {
        controlSelector->setItemText(index, newName);
    }
}


void ToolBoxWin::placeToolBoxIconButtons()
{
    int col = 0;
    int row = 0;

    for (int ctlCount = 0; ctlCount < iconFiles.size(); ctlCount++) {
        buttonsForSpawningControls[ctlCount] = new QPushButton(this);
        buttonsForSpawningControls[ctlCount]->setGeometry(
            (col * (Geom::IconBtnPad + Geom::IconBtn.W) + Geom::IconBtn.X),
            (row * (Geom::IconBtnPad + Geom::IconBtn.H) + Geom::IconBtn.Y),
            Geom::IconBtn.W,
            Geom::IconBtn.H
            );

        // Magic number 6: 6 Icons per row
        if ((ctlCount + 1) % 6 == 0) {
            row++;
            col = 0;
        } else {
            col++;
        }

        buttonsForSpawningControls[ctlCount]->setToolTip("Click to add: " + controlNames[ctlCount]);
        buttonsForSpawningControls[ctlCount]->setStyleSheet("border-image: url(:/" + iconFiles[ctlCount] + ") 0 0 0 0 stretch stretch");

        connect(buttonsForSpawningControls[ctlCount], &QPushButton::clicked, this, [this, ctlCount]{iconButtonClicked(ctlCount);});
    }
}


void ToolBoxWin::placeControlSelector()
{
    controlSelector = new QComboBox(this);
    controlSelector->setGeometry(Geom::CtlSelector.X, Geom::CtlSelector.Y, Geom::CtlSelector.W, Geom::CtlSelector.H);
    connect(controlSelector, &QComboBox::currentIndexChanged, this, &ToolBoxWin::controlViaSelectorChosen);
}


void ToolBoxWin::addToControlSelector(const QString& controlName)
{
    controlSelector->addItem(controlName);
}


void ToolBoxWin::placePropertyTable()
{
    controlPropsTable = new QTableWidget(this);
    controlPropsTable->setColumnCount(3);
    controlPropsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    controlPropsTable->setHorizontalHeaderLabels({"Flags", "Property", "Value"});
    controlPropsTable->verticalHeader()->setVisible(false);
    controlPropsTable->resizeColumnToContents(propColumn);
    controlPropsTable->horizontalHeader()->setStretchLastSection(true);
    controlPropsTable->setGeometry(Geom::PropTable.X, Geom::PropTable.Y, Geom::PropTable.W, Geom::PropTable.H);
    controlPropsTable->setRowCount(0);
    controlPropsTable->verticalHeader()->setDefaultSectionSize(25);

    connect(controlPropsTable, &QTableWidget::cellClicked, this, &ToolBoxWin::cellClicked);
}


void ToolBoxWin::placeEditorContainer()
{
    editorContainer = new QStackedWidget(this);
    editorContainer->setGeometry(Geom::Editor.X, Geom::Editor.Y, Geom::Editor.W, Geom::Editor.H);

    // Page: Label only #############
    labelPage = new QWidget;

    labelNothingSelected = new QLabel("No property from table selected", labelPage);
    labelNothingSelected->setAlignment(Qt::AlignCenter);
    labelNothingSelected->setGeometry(Geom::LabelPage.X, Geom::LabelPage.Y, Geom::LabelPage.W, Geom::LabelPage.H);

    editorContainer->addWidget(labelPage);


    // Page: free Text + Button #############
    freeTextPage = new QWidget;
    freeTextBox = new QPlainTextEdit(freeTextPage);
    ackButton = new QPushButton("Set", freeTextPage);

    freeTextBox->setGeometry(Geom::FreeTextBox.X, Geom::FreeTextBox.Y, Geom::FreeTextBox.W, Geom::FreeTextBox.H);
    freeTextBox->setWordWrapMode(QTextOption::NoWrap);

    ackButton->setGeometry(Geom::AckButton.X, Geom::AckButton.Y, Geom::AckButton.W, Geom::AckButton.H);

    connect(ackButton, &QPushButton::clicked, this, &ToolBoxWin::ackFreeTextChanged);

    editorContainer->addWidget(freeTextPage);


    // Page: Int‑Editor (SpinBoxes dynamically created in Widget) #############
    intPage = new QWidget;

    editorContainer->addWidget(intPage);

    // Page: Multi‑Editor #############
    multiPage = new QWidget;
    multiEditor = new QListWidget(multiPage);

    multiEditor->setGeometry(Geom::MultiEditor.X, Geom::MultiEditor.Y, Geom::MultiEditor.W, Geom::MultiEditor.H);
    multiEditor->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(multiEditor, &QListWidget::itemSelectionChanged, this, &ToolBoxWin::multiSelectionChanged);

    editorContainer->addWidget(multiPage);

    // Page: Single‑Editor #############
    singlePage = new QWidget;
    singleEditor = new QComboBox(singlePage);

    singleEditor->setGeometry(Geom::SingleEditor.X, Geom::SingleEditor.Y, Geom::SingleEditor.W, Geom::SingleEditor.H);

    connect(singleEditor, &QComboBox::currentTextChanged, this, &ToolBoxWin::singleSelectionChanged);

    editorContainer->addWidget(singlePage);

    // Initial #############
    editorContainer->setCurrentWidget(labelPage);
}


QString ToolBoxWin::getHelpForFlags(const QString &flags)
{
    QString text;

    if (flags == "SW") {
        text = "<u><b>S</u></b>etable, <u><b>W</u></b>rittenToIni";

    } else if (flags == "S") {
        text = "<u><b>S</u></b>etable";

    } else if (flags == "W") {
        text = "<u><b>W</u></b>rittenToIni";

    } else {
        text = "Neither Setable, nor written to Ini";

    }

    return text;
}


void ToolBoxWin::updatePropTable(const QString &controlType, const QStringList &controlValues, int id)
{
    clearPropTable();
    collectControlProps(controlType, controlValues);
    fillPropTable(id);
}


void ToolBoxWin::clearPropTable()
{
    controlPropsTable->setRowCount(0);
}


void ToolBoxWin::collectControlProps(const QString &controlType, const QStringList &controlValues)
{
    currentProps.clear();

    // Why do I need it only here to point to "somewhere else" by "::"?
    const Prototypes &prototypes = ::getPrototypes();
    auto typeIter = prototypes.find(controlType);

    const QList<PropInfo> &protoList = typeIter.value();
    currentProps.reserve(protoList.size());

    for (int index = 0; index < protoList.size(); ++index) {
        const PropInfo &proto = protoList[index];

        CollectedProp prop;
        prop.flag = proto.flagsSetWrite;
        prop.name = proto.propName;
        prop.rule = proto.checkRule;
        prop.description = proto.description;
        prop.value = controlValues[index];

        currentProps.append(prop);
    }
}


void ToolBoxWin::fillPropTable(int id)
{
    controlSelector->setCurrentIndex(id);

    for (int row = 0; row < currentProps.size(); ++row) {
        const CollectedProp &prop = currentProps[row];

        controlPropsTable->insertRow(row);

        QTableWidgetItem* flagItem = new QTableWidgetItem(prop.flag);
        QTableWidgetItem* nameItem = new QTableWidgetItem(prop.name);
        QTableWidgetItem* valueItem = new QTableWidgetItem(prop.value);

        flagItem->setToolTip(getHelpForFlags(prop.flag));
        nameItem->setToolTip(prop.description);
        valueItem->setToolTip(prop.description);

        controlPropsTable->setItem(row, flagColumn, flagItem);
        controlPropsTable->setItem(row, propColumn, nameItem);
        controlPropsTable->setItem(row, valueColumn, valueItem);
    }

    // Adjust appearance
    controlPropsTable->resizeColumnToContents(propColumn);
    controlPropsTable->resizeColumnToContents(flagColumn);
    controlPropsTable->horizontalHeader()->setStretchLastSection(true);

    // if the control change ----
    if (controlIdInTable != id) {
        controlIdInTable = id;
        selectedRow = -1;
        editorContainer->setCurrentWidget(labelPage);
    }
}


void ToolBoxWin::cellClicked(int row, int col)
{
    selectedRow = row;
    const CollectedProp& p = currentProps[row];

    if (p.rule.startsWith("Int:")) {
        showIntPage(p);

    } else if (p.rule.startsWith("Multi:")) {
        showMultiPage(p);

    } else if (p.rule.startsWith("Single:")) {
        showSinglePage(p);

    } else {
        showFreeTextPage(p);
    }
}


void ToolBoxWin::showIntPage(const CollectedProp& p)
{
    // remove old SpinBoxes
    qDeleteAll(intPage->findChildren<QSpinBox*>());

    const QStringList ruleParts  = p.rule.mid(sizeof("Int")).split(',');
    QStringList valueParts = p.value.split(',');

    for (int i = 0; i < ruleParts.size(); i++) {
        QSpinBox* spinBox = new QSpinBox(intPage);
        spinBox->setGeometry(i * (Geom::SpinBox.W + Geom::SpinBoxXPad) + Geom::SpinBox.X, Geom::SpinBox.Y, Geom::SpinBox.W, Geom::SpinBox.H);
        spinBox->show();

        if (ruleParts[i].trimmed() == "U") {
            spinBox->setMinimum(0);
        } else {
            spinBox->setMinimum(INT_MIN);
        }

        spinBox->setMaximum(INT_MAX);
        if (i < valueParts.size()) {
            spinBox->setValue(valueParts[i].toInt());
        }

        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ToolBoxWin::spinBoxValueChanged);
    }

    editorContainer->setCurrentWidget(intPage);
}


void ToolBoxWin::spinBoxValueChanged(int)
{
    QStringList parts;

    for (QSpinBox* sp : intPage->findChildren<QSpinBox*>()) {
        parts << QString::number(sp->value());
    }

    emitPropertyChange(parts.join(','));
}


void ToolBoxWin::showMultiPage(const CollectedProp& p)
{
    const QStringList ruleOptions = p.rule.mid(sizeof("Multi")).split(',');
    const QStringList values = p.value.split(',');

    multiEditor->clear();
    multiEditor->addItems(ruleOptions);
    multiEditor->clearSelection();

    for (int i = 0; i < ruleOptions.size(); i++) {
        if (values.contains(ruleOptions[i])) {
            multiEditor->item(i)->setSelected(true);
        }
    }

    editorContainer->setCurrentWidget(multiPage);
}


void ToolBoxWin::multiSelectionChanged()
{
    QStringList selected;

    for (QListWidgetItem* item : multiEditor->selectedItems()) {
        selected << item->text();
    }

    emitPropertyChange(selected.join(','));
}


void ToolBoxWin::showSinglePage(const CollectedProp& p)
{
    const QStringList ruleOptions = p.rule.mid(sizeof("Single")).split(',');

    singleEditor->clear();

    singleEditor->blockSignals(true);
    singleEditor->addItems(ruleOptions);
    singleEditor->blockSignals(false);

    int idx = singleEditor->findText(p.value);

    if (idx > -1) {
        singleEditor->setCurrentIndex(idx);
    }

    editorContainer->setCurrentWidget(singlePage);
}


void ToolBoxWin::singleSelectionChanged(const QString& text)
{
    emitPropertyChange(text);
}


void ToolBoxWin::showFreeTextPage(const CollectedProp& p)
{
    freeTextBox->setPlainText(p.value);
    editorContainer->setCurrentWidget(freeTextPage);
}


void ToolBoxWin::emitPropertyChange(const QString& newVal)
{
    if (selectedRow < 0) {
        return;
    }

    const QString& propName = currentProps[selectedRow].name;

    emit controlPropChanged("set`" + nameOfControl() + "`" + propName + "`" + newVal + "`", false);
}


void ToolBoxWin::ackFreeTextChanged()
{
    emitPropertyChange(freeTextBox->toPlainText());
}


QString ToolBoxWin::nameOfControl()
{
    // Magic number 0: The Name of the element (wow, this is dirty)
    return currentProps[0].value;
}
