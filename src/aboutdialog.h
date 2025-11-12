// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVector>
#include <QPair>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);

private slots:
    void buttonPressed(const QString &key);

private:
    struct Entry {
        QString buttonCaption;
        QString file;
        QStringList fallbackRelFolders;
    };
    QVector<Entry> entries;

    QTextEdit *textEdit;
    QVector<QPushButton*> buttons;

    void loadFileIntoTextEdit(const Entry &entry);
    void setupUi();


    void initializeEntries();
    QString getAboutPeasyGUI();
    QString makePropHelp();
    QString getHelpIntro();
};

#endif // ABOUTDIALOG_H

