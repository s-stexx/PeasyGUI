// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef TOOLBOXWIN_H
#define TOOLBOXWIN_H

#include "controlprototypes.h"

#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QStackedWidget>
#include <QListWidget>

struct CollectedProp {
    QString flag;
    QString name;
    QString value;
    QString rule;
    QString description;
};

class ToolBoxWin : public QWidget
{
    Q_OBJECT

public:
    ToolBoxWin(QWidget *parent = nullptr);

    void start();

signals:
    void spawnControlClicked(const QString& content, bool replyExpected);
    void controlPropChanged(const QString& content, bool replyExpected);
    void requestInitialSelectorFilling();
    void controlSelected(int id);

public slots:
    void updatePropTable(const QString &controlType, const QStringList &controlValues, int id);
    void addToControlSelector(const QString &controlName);
    void swapControlNameInSelector(const QString &oldName, const QString &newName);

private slots:
    void cellClicked(int row, int column);
    void iconButtonClicked(int type);
    void controlViaSelectorChosen(int id);

private:
    QPushButton *buttonsForSpawningControls[20];
    QTableWidget *controlPropsTable;

    QStackedWidget* editorContainer;
    QWidget* labelPage;
    QWidget* intPage;
    QWidget* singlePage;
    QWidget* multiPage;
    QWidget* freeTextPage;

    QLabel* labelNothingSelected;
    QComboBox* singleEditor;
    QListWidget* multiEditor;
    QPlainTextEdit* freeTextBox;
    QPushButton* ackButton;

    QComboBox* controlSelector;

    using Prototypes = QMap<QString, QList<PropInfo>>;
    const Prototypes &getPrototypes();

    QVector<CollectedProp> currentProps;
    int selectedRow = -1;
    int controlIdInTable = -1;
    const int flagColumn = 0;
    const int propColumn = 1;
    const int valueColumn = 2;

    struct GeomParts {
        int X, Y, W, H;
    };

    struct Geom {
        inline static const GeomParts Win          = {600, 20, 350, 450};
        inline static const GeomParts IconBtn      = {10, 10, 50, 50};
        inline static const GeomParts PropTable    = {10, 150, 325, 220};
        inline static const GeomParts CtlSelector  = {10, 120, 325, 30};
        inline static const GeomParts Editor       = {10, 380, 340, 100};
        inline static const GeomParts LabelPage    = {0, 0, 340, 30};
        inline static const GeomParts FreeTextBox  = {0, 5, 290, 50};
        inline static const GeomParts AckButton    = {295, 0, 30, 32};
        inline static const GeomParts MultiEditor  = {0, 0, 325, 60};
        inline static const GeomParts SingleEditor = {0, 0, 325, 30};
        inline static const GeomParts SpinBox      = {0, 10, 65, 30};
        inline static const int SpinBoxXPad = 15;
        inline static const int IconBtnPad = 5;
    };

    void placeControlSelector();
    void placeToolBoxIconButtons();
    void placePropertyTable();
    void placeEditorContainer();

    // Expand for new controls
    QStringList iconFiles = {
        "pushbutton.png",
        "textbox.png",
        "listbox.png",
        "label.png",
        "open.png",
        "save.png",
        "imageview.png",
        "checkbox.png",
        "droparea.png",
        "dropdown.png",
        "slider.png",
        "progressbar.png"
    };

    // Expand for new controls
    QStringList controlNames = {
        "Button",
        "TextBox",
        "ListBox",
        "Label",
        "OpenButton",
        "SaveButton",
        "ImageView",
        "CheckBox",
        "DropArea",
        "DropDown",
        "Slider",
        "ProgressBar"
    };

    void ackFreeTextChanged();
    void showIntPage(const CollectedProp &p);
    void spinBoxValueChanged(int);
    void showMultiPage(const CollectedProp &p);
    void multiSelectionChanged();
    void showSinglePage(const CollectedProp &p);
    void singleSelectionChanged(const QString &text);
    void showFreeTextPage(const CollectedProp &p);
    void emitPropertyChange(const QString &newVal);
    void clearPropTable();
    void fillPropTable(int id);
    void collectControlProps(const QString &controlType, const QStringList &controlValues);
    QString nameOfControl();
    QString getHelpForFlags(const QString &flags);

};

#endif
