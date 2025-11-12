// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "mainwin.h"

#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>
#include <QLabel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPointer>

#include "geometryhandles.h"
#include "requestparser.h"
#include "requestchecker.h"
#include "controlserializer.h"
#include "messagebox.h"
#include "apppaths.h"

// Expand for new controls -----
#include "button.h"
#include "textbox.h"
#include "label.h"
#include "listbox.h"
#include "pathdialogbutton.h"
#include "imageview.h"
#include "checkbox.h"
#include "droparea.h"
#include "dropdown.h"
#include "slider.h"
#include "progressbar.h"

#define CONNECT_IF_TYPE(typeName, className, signal, method) \
if (type == typeName) { \
        if (auto* derived = dynamic_cast<className*>(control)) { \
            QObject::connect(derived, signal, this, method); \
    } \
}


MainWin::MainWin(const AppConfig &config, QWidget *parent) :
    QMainWindow(parent), Control(parent)
{
    mode = config.mode;

    // Element 0 of all controls is the MainWin itself ---
    this->setProp("Name", "Window");
    this->setProp("Type", "Window");
    this->setProp("QuitNow", "False");
    controls.append(static_cast<Control*>(this));

    // MainWin Resize event ---
    resizeEndTimer.setInterval(300);
    resizeEndTimer.setSingleShot(true);
    connect(&resizeEndTimer, &QTimer::timeout, this, &MainWin::onResizeFinished);

    // For ClipboardChanged event, the Clipboard is polled ---
    clipboard = QApplication::clipboard();
    pollClipboardTimer.setInterval(300);
    connect(&pollClipboardTimer, &QTimer::timeout, this, &MainWin::pollClipboard);

    // Connect all MainWin events to Queue
    connect(this, &MainWin::windowEvent, this, &MainWin::addToEventQueue);

    // To resize/move controls, GeometryHandles are displayed in Design-Mode ----
    setMouseTracking(true);
    GeometryHandles::instance(this);
    qApp->installEventFilter(this);
    connect(GeometryHandles::instance(), &GeometryHandles::handleClicked, this, &MainWin::collectPropsOfControl);

    // To highlight in Design-Mode the object needs init
    highlighter.setParentWidget(this);

    // The MainWin needs the (Tab-)Focus to intercept a lot of events --
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    // For legal reasons, license agreement must be linked
    aboutAccess = new AboutAccess(this);
}


void MainWin::collectPropsOfControl(int id)
{
    QString type = controls[id]->getProp("Type");

    QStringList controlValues;
    const auto &prototypes = getPrototypes();
    const QList<PropInfo> &protoList = prototypes.value(type);

    for (const PropInfo &p : protoList) {
        controlValues << controls[id]->getProp(p.propName);
    }

    emit propsOfControlCollected(type, controlValues, id);
    highlightControl(id);
}


void MainWin::setSpecificProp(const QString &prop, const QString &value)
{
    if (prop == "Text") {
        this->setWindowTitle(value);

    } else if (prop == "Clipboard") {
        clipboard->setText(value);

    } else if (prop == "Events") {
        if (value.contains("ClipboardChanged")) {
            lastClipboard = clipboard->text();
            pollClipboardTimer.start();
        } else {
            pollClipboardTimer.stop();
        }

    } else if (prop == "Icon") {
        QIcon icon(AppPaths::cleanAbsPath(value));
        QApplication::setWindowIcon(icon);

        propsDict["Icon"] = value;

    } else if (prop == "AboutHeading") {
        aboutAccess->setHeading(value);
        aboutAccess->repositionButton();

    } else if (prop == "QuitNow") {
        if (value == "True") {
            QTimer::singleShot(100, [=]() {
                emit shutdownConnections();
                QApplication::quit();
            });
        } else {
            propsDict["QuitNow"] = value;
        }

    } else if (prop == "MessageBoxNow") {
        MessageBox::showMessage(value);

    }
}


QString MainWin::getSpecificProp(const QString &prop)
{
    QString value = "";

    if (prop == "Text") {
        value = this->windowTitle();

    } else if (prop == "Icon") {
        value = propsDict["Icon"];

    } else if (prop == "Clipboard") {
        value = clipboard->text();

    } else if (prop == "AboutHeading") {
        value = aboutAccess->heading();

    } else if (prop == "QuitNow") {
        value = propsDict["QuitNow"];

    } else if (prop == "MessageBoxNow") {
        value = "";

    }

    return value;
}


void MainWin::treatRequests(const QString &content, bool replyExpected)
{
    RequestParser parser;
    auto requests = parser.parseAll(content);

    if (requests.isEmpty()) {
        return;
    }

    QString finalReply;
    QString parsingError = requests.last().error;

    if ( ! parsingError.isEmpty()) {
        finalReply = "ERROR`" + parsingError + "`";

    } else {
        QString plausibilityError = RequestChecker::checkAll(requests, controls);

        if ( ! plausibilityError.isEmpty()) {
            finalReply = "ERROR`" + plausibilityError + "`";

        } else {
            for (const auto& request : requests) {
                if (request.action == "new") {
                    spawnControl(request.value, request.name);

                } else if (request.action == "set") {
                    fulfillSetRequest(request.name, request.prop, request.value);

                } else {
                    finalReply = fulfillInformationRequest(request.action, request.name, request.prop);

                }
            }
        }
    }

    if (replyExpected) {
        if (finalReply.isEmpty()) {
            finalReply = "OK";
        }

        emit replyForClientPrepared(finalReply);
    }

}


void MainWin::fulfillSetRequest(const QString &name, const QString &prop, const QString &value)
{
    int idxControl = -1;

    idxControl = findControlIndexByName(name);

    controls[idxControl]->setProp(prop, value);

    if (designModeInitReady && mode == AppMode::Designer) {
        // indirect refresh of ToolBoxWin table
        collectPropsOfControl(idxControl);

        if (prop == "Name") {
            emit nameOfControlHasChanged(name, value);
        }
    }
}


QString MainWin::fulfillInformationRequest(const QString &action, const QString &name, const QString &prop)
{
    QString replyMessage;
    int idxControl = -1;

    if (action == "get") {
        idxControl = findControlIndexByName(name);
        replyMessage = "OK`" + controls[idxControl]->getProp(prop) + "`";

    } else if (action == "events?") {
        if (eventList.size() == 0) {
            replyMessage = "NONE";
        } else {
            replyMessage = eventList.join("");
            eventList.clear();
        }
    }

    return replyMessage;
}


void MainWin::handleIniFileReadReady(const QString &content, bool replyExpected)
{
    treatRequests(content, replyExpected);
    designModeInitReady = true;
    yPositionForSpawnedControls = 10;
}


void MainWin::closeEvent(QCloseEvent *event)
{
    if (mode == AppMode::Designer) {
        QString content = ControlSerializer::serialize(controls);

        emit appClosedControlsDataCollected(content);
        event->accept();

    } else {
        if (propsDict["Events"].contains("Closed")) {
            // The user will quit the GUI by closing the window.
            // This is reported via the GUI as an event to the App. The App must
            // then send "set`Window`Quit`True`" to finally close the GUI.
            emit windowEvent(propsDict["Name"], "Closed");
            this->hide();
            event->ignore();

        } else {
            emit shutdownConnections();
            event->accept();
        }
    }
}


int MainWin::findControlIndexByName(const QString &name)
{
    int idxFound = -1;
    int idx = 0;

    while((idxFound == -1) && (idx < controls.size())) {
        if (controls[idx]->getProp("Name") == name) {
            idxFound = idx;
        }
        idx++;
    }

    return idxFound;
}


void MainWin::collectAndEmitControlNames()
{
    for (int idx = 0; idx < controls.size(); idx++) {
        emit newControlSpawned(controls[idx]->getProp("Name"));
    }
}


void MainWin::spawnControl(const QString &type, const QString &name = "")
{
    int idxControl;

    Control* control = nullptr;

    // Expand for new controls
    if (type == "Button") {
        control = new Button(this);

    } else if (type == "TextBox") {
        control = new TextBox(this);

    } else if (type == "ListBox") {
        control = new ListBox(this);

    } else if (type == "Label") {
        control = new Label(this);

    } else if (type == "OpenButton" || type == "SaveButton") {
        control = new PathDialogButton(this);

    } else if (type == "ImageView") {
        control = new ImageView(this);

    } else if (type == "CheckBox") {
        control = new CheckBox(this);

    } else if (type == "DropArea") {
        control = new DropArea(this);

    } else if (type == "DropDown") {
        control = new DropDown(this);

    } else if (type == "Slider") {
        control = new Slider(this);

    } else if (type == "ProgressBar") {
        control = new ProgressBar(this);

    }

    if (control) {
        idxControl = controls.size();

        int typeCount = countControlsOfThisType(type);

        QString newName = generateUniqueNameIfEmpty(name, type, typeCount);

        control->setId(idxControl);
        control->setProp("Type", type);
        control->setProp("Name", newName);

        // Sets all Defaults - not only Position stuff
        int height = control->applyDefaultProperties(typeCount, yPositionForSpawnedControls);
        yPositionForSpawnedControls = yPositionForSpawnedControls + height + 10;

        if (mode == AppMode::Designer) {
            control->setControlMightGetInactive(false);

            emit newControlSpawned(newName);

        } else {
            connectControlEvents(control, type);
        }

        controls.append(control);

    }    
}


void MainWin::connectControlEvents(Control* control, const QString &type)
{
    // Expand for new controls
    CONNECT_IF_TYPE("Button", Button, &Button::buttonEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("TextBox", TextBox, &TextBox::textBoxEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("ListBox", ListBox, &ListBox::listBoxEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("OpenButton", PathDialogButton, &PathDialogButton::pathDialogButtonEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("SaveButton", PathDialogButton, &PathDialogButton::pathDialogButtonEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("ImageView", ImageView, &ImageView::imageViewEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("CheckBox", CheckBox, &CheckBox::checkBoxEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("DropArea", DropArea, &DropArea::dropAreaEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("DropDown", DropDown, &DropDown::dropDownEvent, &MainWin::addToEventQueue)
    CONNECT_IF_TYPE("Slider", Slider, &Slider::sliderEvent, &MainWin::addToEventQueue)
}


int MainWin::countControlsOfThisType(const QString &type)
{
    int count = 0;

    for (int i=0; i < controls.size(); i++) {
        if (controls[i]->getProp("Type") == type) {
            count++;
        }
    }

    return count;
}


QString MainWin::generateUniqueNameIfEmpty(const QString &name, const QString &type, int typeCount)
{
    QString newName = name;

    if (newName == "") {
        QStringList allExistingNames;

        for (int i=0; i < controls.size(); i++) {
            allExistingNames.append(controls[i]->getProp("Name"));
        }

        newName = type + "_" + QString::number(typeCount);

        while(allExistingNames.indexOf(newName) > -1) {
            typeCount++;
            newName = type + "_" + QString::number(typeCount);
        }
    }

    return newName;
}


void MainWin::addToEventQueue(const QString &name, const QString &event)
{
    eventList.append(name + "`" + event + "`\n");
}


void MainWin::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // Restart timer on every resize
    resizeEndTimer.start();

    aboutAccess->repositionButton();
}


void MainWin::onResizeFinished()
{
    if (isFirstResize) {
        isFirstResize = false;
    } else if (propsDict["Events"].contains("Resized")) {
        emit windowEvent(propsDict["Name"], "Resized`" +
                                                QString::number(width()) + "," +
                                                QString::number(height()));
    }
}


void MainWin::pollClipboard()
{
    const QString currentClipboard = clipboard->text();
    if (currentClipboard != lastClipboard) {
        lastClipboard = currentClipboard;
        emit windowEvent(propsDict["Name"], "ClipboardChanged");
    }
}


QWidget* MainWin::detectControlUnderCursor(QEvent* event)
{
    QWidget* controlUnderMouse = nullptr;

    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    QPoint globalPos = mouseEvent->globalPosition().toPoint();
    QPoint localPos = this->mapFromGlobal(globalPos);

    for (int i = controls.size() - 1; i >= 1; --i) {
        QWidget* controlWidget = dynamic_cast<QWidget*>(controls[i]);
        if (controlWidget) {
            QPoint widgetPos = controlWidget->mapTo(this, QPoint(0, 0));
            QRect mappedRect(widgetPos, controlWidget->size());
            if (mappedRect.contains(localPos)) {
                controlUnderMouse = controlWidget;
                break;
            }
        }
    }

    return controlUnderMouse;
}


bool MainWin::eventFilter(QObject* obj, QEvent* event)
{
    // For speed: do not interact while dragging
    if (GeometryHandles::instance()->isDragging()) {
        return false;
    }

    if (mode == AppMode::Designer) {
        if (event->type() == QEvent::MouseMove) {
            QWidget* controlUnderMouse = detectControlUnderCursor(event);

            GeometryHandles::instance()->showOrHideGeometryHandles(controlUnderMouse);

        } else if (event->type() == QEvent::MouseButtonPress && obj->objectName() == "MainWinClassWindow") {
            // The Mouse Button is pressed within the Mainwin but did it hit some control in it?
            QWidget* controlUnderMouse = detectControlUnderCursor(event);

            if (controlUnderMouse == nullptr) {
                // It is none of the other controls, so it is the Mainwin
                collectPropsOfControl(idOfMainWindow);
            }
        }
    } else if (event->type() == QEvent::KeyPress &&
               obj->objectName() == "MainWinClassWindow" &&
               propsDict["Events"].contains("KeyPressed")) {
        return interceptKeyForCustomEvent(static_cast<QKeyEvent *>(event));
    }

    return QObject::eventFilter(obj, event);
}


void MainWin::highlightControl(int id) {
    if (id == idOfMainWindow) {
        return;
    }

    highlighter.mark(controls[id]->getProp("Geometry"), id);
}


bool MainWin::interceptKeyForCustomEvent(QKeyEvent *event)
{
    bool isCustomEvent = true;
    QWidget *fw = qApp->focusWidget();
    int key = event->key();
    Qt::KeyboardModifiers mods = event->modifiers();

    // "Enter, Tab, Space" (Navigation between controls) or "Modifier alone"
    if (key == Qt::Key_Enter ||
        key == Qt::Key_Return ||
        key == Qt::Key_Tab ||
        key == Qt::Key_Backtab ||
        key == Qt::Key_Space ||
        key == Qt::Key_Shift ||
        key == Qt::Key_Control ||
        key == Qt::Key_Alt ||
        key == Qt::Key_Meta) {
        isCustomEvent = false;

    // User writing / using Shortcut while in TextBox, DropDown
    } else if (qobject_cast<TextBox *>(fw) || qobject_cast<DropDown *>(fw)) {
        isCustomEvent = false;

    // ListBox: ignore arrow keys
    } else if (qobject_cast<ListBox *>(fw)) {
        if (key == Qt::Key_Up ||
            key == Qt::Key_Down) {
            isCustomEvent = false;
        }
    }

    if (isCustomEvent) {
        QString keyStr = convertKeysToString(event);
        if (!keyStr.isEmpty()) {
            emit windowEvent(propsDict["Name"], "KeyPressed`" + keyStr);
        }
    }

    return isCustomEvent;
}


QString MainWin::convertKeysToString(QKeyEvent *event)
{
    QString result = "";
    int key = event->key();

    QString keyChar;

    if ((key >= Qt::Key_A && key <= Qt::Key_Z) ||
        (key >= Qt::Key_0 && key <= Qt::Key_9)) {
        keyChar = QChar(key);

    } else {
        keyChar = QKeySequence(key).toString(QKeySequence::NativeText);

    }

    if (!keyChar.isEmpty()) {
        Qt::KeyboardModifiers mods = event->modifiers();

        if (mods.testFlag(Qt::ShiftModifier))   result += "Shift+";
        if (mods.testFlag(Qt::ControlModifier)) result += "Ctrl/Cmd+";
        if (mods.testFlag(Qt::AltModifier))     result += "Alt+";
        if (mods.testFlag(Qt::MetaModifier))    result += "Control+"; // Mac

        result += keyChar;
    }

    return result;
}
