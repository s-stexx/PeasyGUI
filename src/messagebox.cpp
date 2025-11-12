// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "messagebox.h"

#include <QVBoxLayout>
#include <QTimer>


MessageBox::MessageBox(const QString &text, QWidget *parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);
    setWindowFlag(Qt::Dialog);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(25, 25, 25, 15);
    layout->setSpacing(30);

    label = new QLabel(text, this);
    label->setAlignment(Qt::AlignCenter);
    label->setWordWrap(false);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layout->addWidget(label);

    okButton = new QPushButton("OK", this);
    layout->addWidget(okButton, 0, Qt::AlignCenter);

    connect(okButton, &QPushButton::clicked, this, &QDialog::close);
}


void MessageBox::showMessage(const QString &text, QWidget *parent)
{
    QTimer::singleShot(0, [text, parent]() {
        MessageBox *box = new MessageBox(text, parent);
        box->show();
    });
}
