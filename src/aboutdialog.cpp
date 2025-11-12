// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "aboutdialog.h"

#include "apppaths.h"
#include "controlprototypes.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QFontDatabase>


AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    initializeEntries();
    setupUi();
    buttonPressed("About PeasyGUI");
}


QString AboutDialog::getAboutPeasyGUI()
{
    return QString(
"# PeasyGUI\n"
"Version %1\n\n"
"A lightweight and generic GUI for any programming language.\n\n"
"Developed by Stefan Stechert. Licensed under the MIT License.")
    .arg(APP_VERSION);
}

QString AboutDialog::getHelpIntro()
{
    return QString(
"Connecting with PeasyGUI                                                    \n"
"========================                                                    \n"
"                                                                            \n"
"Overview                                                                    \n"
"--------                                                                    \n"
"PeasyGUI is a lightweight graphical user interface that can be controlled   \n"
"by your own program. Your program and PeasyGUI communicate using a simple   \n"
"text-based protocol over TCP.                                               \n"
"                                                                            \n"
"When PeasyGUI is started with --protocol=tcp, it opens a small TCP server   \n"
"that listens for requests from your application. Your program connects as a \n"
"client, sends requests and receives clear, text-based replies in return.    \n"
"                                                                            \n"
"All communication happens quickly and reliably, so your application can     \n"
"update the GUI or react to user actions almost instantly.                   \n"
"                                                                            \n"
"  YourApp            PeasyGUI                                               \n"
" ┌────────┐         ┌────────┐                                              \n"
" │        │         │        │                                              \n"
" │        │ Request │        │                                              \n"
" │        ├────────►│        │                                              \n"
" │        │         │        │                                              \n"
" │ Client │         │ Server │                                              \n"
" │        │         │        │                                              \n"
" │        │  Reply  │        │                                              \n"
" │        │◄────────┤        │                                              \n"
" │        │         │        │                                              \n"
" └────────┘         └────────┘                                              \n"
"                                                                            \n"
"There are four types of requests:                                           \n"
"- events?                                                                   \n"
"- get                                                                       \n"
"- set                                                                       \n"
"- new.                                                                      \n"
"                                                                            \n"
"The last two (set and new) do not return information but simply reply with  \n"
"either OK or an error message in the form ERROR`...`. Multiple set or new   \n"
"requests can be combined into a single TCP packet.                          \n"
"                                                                            \n"
"All elements in a request are separated by a backtick (`). Be careful not   \n"
"to confuse the backtick with an apostrophe (') or accent (´).               \n"
"                                                                            \n"
"                                                                            \n"
"The 'events?' Request                                                       \n"
"---------------------                                                       \n"
"The events? request has no parameters. It returns all active events that    \n"
"have occurred since the last events? query. Each event is followed by a     \n"
"line break.                                                                 \n"
"If an event includes additional information (such as mouse coordinates),    \n"
"that data is appended directly after the event name.                        \n"
"                                                                            \n"
"Example:                                                                    \n"
"If the button named ok_button is clicked, then the window is resized and    \n"
"finally closed, PeasyGUI will respond with:                                 \n"
"ok_button`Clicked`                                                          \n"
"Window`Resized`346,549`                                                     \n"
"Window`Resized`370,566`                                                     \n"
"Window`Closed`                                                              \n"
"                                                                            \n"
"If no events occurred in the meantime, the reply will be:                   \n"
"NONE                                                                        \n"
"                                                                            \n"
"                                                                            \n"
"The 'get' Request                                                           \n"
"-----------------                                                           \n"
"Format:                                                                     \n"
"get`ControlName`Property`                                                   \n"
"                                                                            \n"
"Example:                                                                    \n"
"get`TextBox_0`Text`                                                         \n"
"                                                                            \n"
"If the user has entered 'Hello, World!' into the text box, PeasyGUI will    \n"
"respond with:                                                               \n"
"OK`Hello, World!`                                                           \n"
"                                                                            \n"
"If a control contains multiple entries, the property name begins with       \n"
"'Entire', for example:                                                      \n"
"get`ListBox_7`EntireTexts`                                                  \n"
"                                                                            \n"
"PeasyGUI will respond with:                                                 \n"
"OK`dog`cat`mouse`                                                           \n"
"                                                                            \n"
"                                                                            \n"
"The 'set' Request                                                           \n"
"-----------------                                                           \n"
"Format:                                                                     \n"
"set`ControlName`Property`Value`                                             \n"
"                                                                            \n"
"Example:                                                                    \n"
"set`TextBox_0`Text`42`                                                      \n"
"                                                                            \n"
"The 'Entire' prefix can also be used here to assign multiple values, for    \n"
"example:                                                                    \n"
"set`MyListBox`EntireTexts`a`bc`def`                                         \n"
"                                                                            \n"
"This will clear the ListBox and then fill it with the following entries:    \n"
"a                                                                           \n"
"bc                                                                          \n"
"def                                                                         \n"
"                                                                            \n"
"                                                                            \n"
"The 'new' Request                                                           \n"
"-----------------                                                           \n"
"Format:                                                                     \n"
"new`ControlName`Type`ControlType`                                           \n"
"                                                                            \n"
"Example:                                                                    \n"
"new`cancel_button`Type`Button`                                              \n"
"                                                                            \n"
"This adds a new control of the specified type to the GUI. The control will  \n"
"have default properties, which can then be modified using set requests.     \n"
"                                                                            \n"
"                                                                            \n"
"File-Based Communication                                                    \n"
"------------------------                                                    \n"
"The file-based communication mode (--protocol=filebased) works but is       \n"
"significantly slower than TCP. It was originally intended as an alternative \n"
"for corporate environments where running a local server might not be        \n"
"allowed.                                                                    \n"
"                                                                            \n"
"However, because it relies on disk file exchanges for every request, it     \n"
"becomes inefficient, especially for frequent get requests.                  \n"
"                                                                            \n"
"                                                                            \n"
"                                                                            \n"
"Building Your GUI                                                           \n"
"=================                                                           \n"
"                                                                            \n"
"Using INI Files                                                             \n"
"---------------                                                             \n"
"Instead of constructing the entire GUI from your code, you can predefine    \n"
"parts (or all) of it using an INI file. The easiest way to create one is    \n"
"with the built-in WYSIWYG Design Editor.                                    \n"
"                                                                            \n"
"Start PeasyGUI without parameters. It will look for a file named            \n"
"PeasyGUI.ini in the application root directory and load it. You can add or  \n"
"reposition controls, resize them, and edit their properties in the table.   \n"
"                                                                            \n"
"When PeasyGUI is closed, it will create or update the INI file              \n"
"automatically. The contents of this file (a sequence of new and set         \n"
"requests) can also be sent to PeasyGUI over TCP, either as a single block   \n"
"or as individual requests.                                                  \n"
"                                                                            \n"
"                                                                            \n"
"Working with the Design Editor                                              \n"
"------------------------------                                              \n"
"The design editor is mostly self-explanatory, but some aspects of its       \n"
"operation are slightly unusual. When you hover the mouse over a control,    \n"
"blue squares appear at its corners and edges. These allow you to resize the \n"
"control. The blue square in the center allows you to move the control.      \n"
"                                                                            \n"
"Clicking any of the squares will show the control’s properties in the table \n"
"view. You can also click directly inside a control (not on the blue         \n"
"squares) to just interact with it, for example to edit the text in a        \n"
"TextBox or test selection behavior in a ListBox.                            \n"
"                                                                            \n"
"                                                                            \n"
"Paths and Resources                                                         \n"
"-------------------                                                         \n"
"Wherever PeasyGUI accepts a path (for example, to icons, INI files, image   \n"
"sources, or the exchange directory), you may use either absolute paths or   \n"
"paths relative to the application root directory. So the folder that        \n"
"contains bin/ on Linux and Windows (but is not inside it), or the folder    \n"
"that contains PeasyGUI.app/ on macOS.                                       \n"
"                                                                            \n"
"                                                                            \n"
"Controls and Properties                                                     \n"
"=======================                                                     \n"
"                                                                            \n"
"Geometry                                                                    \n"
"--------                                                                    \n"
"The 4 numbers of the geometry represent left, top, width, height of the     \n"
"control within the window.                                                  \n"
"                                                                            \n"
" ┌──────────────────────────────┐                                           \n"
" │OOO        Window             │                                           \n"
" ├─────────────┬────────────────┤                                           \n"
" │             ▲                │                                           \n"
" │             │ Top            │                                           \n"
" │             ▼                │                                           \n"
" │        ┌────┴─────┐ ┬        │                                           \n"
" │  Left  │          │ ▲        │                                           \n"
" ├◄──────►┤    OK    │ │Height  │                                           \n"
" │        │          │ ▼        │                                           \n"
" │        └──────────┘ ┴        │                                           \n"
" │        ├◄────────►┤          │                                           \n"
" │           Width              │                                           \n"
" └──────────────────────────────┘                                           \n"
"                                                                            \n"
"For the Window itself, top and left represent the distances within your     \n"
"primary display.                                                            \n"
"                                                                            \n"
"Visibility and Show Options                                                 \n"
"---------------------------                                                 \n"
"The Show property controls how a control is displayed and whether it can be \n"
"interacted with. It has four possible values:                               \n"
"- Normal: The control is visible and fully interactive.                     \n"
"- Disabled: The control is visible but cannot be interacted with using the  \n"
"mouse or keyboard.                                                          \n"
"- Invisible: The control is not visible.                                    \n"
"- ToBeDeleted: Used only in Design mode. Controls set to this state will    \n"
"not be saved to the INI file when PeasyGUI is closed.                       \n"
"                                                                            \n"
"Complete Property Reference                                                 \n"
"---------------------------                                                 \n"
    );
}


void AboutDialog::initializeEntries()
{
    // Original folder: Dir of executable
    // Fallback folders: Within .app-Bundle, Up from Qt build dir, Up from Qt build Dir .app-Bundle
    entries = {
        {"About PeasyGUI", "", {}},
        {"Show Readme", "README.md", {"../Resources/licenses/", "../../", "../../../../../"}},
        {"More help", "", {}},
        {"About Qt", "", {}},
        {"LICENSE", "LICENSE", {"../Resources/licenses/", "../../", "../../../../../"}},
        {"LGPLv3", "LICENSE.LGPL3", {"../Resources/licenses/", "../../third_party_licenses/", "../../../../../third_party_licenses/"}},
        {"GPLv3", "LICENSE.GPL3", {"../Resources/licenses/", "../../third_party_licenses/", "../../../../../third_party_licenses/"}}
    };
}


void AboutDialog::setupUi()
{
    setWindowTitle("About");
    setGeometry(50, 50, 700, 400);
    setMinimumSize(     630, 400);

    auto *mainLayout = new QVBoxLayout(this);
    auto *buttonLayout = new QHBoxLayout;

    for (auto &entry : entries) {
        auto *button = new QPushButton(entry.buttonCaption, this);
        button->setFlat(true);
        button->setStyleSheet("QPushButton { color: blue; background: transparent; border: none; }");
        button->setCursor(Qt::PointingHandCursor);
        buttonLayout->addWidget(button);
        buttons.append(button);

        connect(button, &QPushButton::clicked, this, [this, entry]() {
            buttonPressed(entry.buttonCaption);
        });
    }

    mainLayout->addLayout(buttonLayout);

    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);

    mainLayout->addWidget(textEdit, 1);
}


void AboutDialog::buttonPressed(const QString &key)
{
    if (key == "About PeasyGUI") {
        textEdit->setFont(QFont());
        textEdit->setMarkdown(getAboutPeasyGUI());

    } else if (key == "About Qt") {
        QMessageBox::aboutQt(this);

    } else if (key == "More help") {
        QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        font.setPointSize(12);
        textEdit->setFont(font);
        textEdit->setPlainText((getHelpIntro() + makePropHelp())); //

    } else {
        for (const auto &entry : entries) {
            if (entry.buttonCaption == key) {
                loadFileIntoTextEdit(entry);
                return;
            }
        }
    }
}


QString AboutDialog::makePropHelp()
{
    const auto &prototypes = getPrototypes();

    QStringList lines;

    for (auto it = prototypes.cbegin(); it != prototypes.cend(); ++it) {
        const QString &controlName = it.key();
        const QList<PropInfo> &props = it.value();

        lines << QString("# %1").arg(controlName);

        for (const PropInfo &p : props) {
            QStringList block;
            block << QString("  • %1:").arg(p.propName);

            QString desc = p.description;
            QString flagInfo;

            if (p.flagsSetWrite.isEmpty()) {
                flagInfo = " (read-only, not in Ini)";

            } else if (p.flagsSetWrite == "S") {
                flagInfo = " (not in Ini)";

            } else if (p.flagsSetWrite == "W") {
                flagInfo = " (read-only)";
            }

            if (!flagInfo.isEmpty())  {
                desc += flagInfo;
            }

            block << QString("      %1").arg(desc);

            if (p.checkRule.startsWith("Single:")) {
                const QStringList opts = p.checkRule.section(':', 1).split(',', Qt::SkipEmptyParts);
                block << QString("      Select one: %1").arg(opts.join(", "));
            } else if (p.checkRule.startsWith("Multi:")) {
                const QStringList opts = p.checkRule.section(':', 1).split(',', Qt::SkipEmptyParts);
                block << QString("      Select any or none: %1").arg(opts.join(", "));
            }

            lines << block;
        }

        lines << "";
    }

    return lines.join('\n');
}


void AboutDialog::loadFileIntoTextEdit(const Entry &entry)
{
    QStringList paths;

    paths << QDir(AppPaths::dirOfExecutable()).filePath(entry.file);

    // Add fallback dirs, but make them relative to executableRoot
    for (const auto &fallbackRelFolder : entry.fallbackRelFolders) {
        QDir baseDir(AppPaths::dirOfExecutable());
        QString absPath = QDir::cleanPath(baseDir.filePath(QDir(fallbackRelFolder).filePath(entry.file)));
        paths << absPath;
    }

    for (const auto &pathTry : paths) {
        QFile file(pathTry);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textEdit->setFont(QFont());
            textEdit->setMarkdown(in.readAll());
            return;
        }
    }

    // If none worked, show all attempted paths in error message
    textEdit->setPlainText("Error: could not open. Tried paths:\n" + paths.join("\n"));
}

