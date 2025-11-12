// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QProgressBar>
#include "control.h"

class ProgressBar : public QProgressBar, public Control
{
    Q_OBJECT

public:
    explicit ProgressBar(QWidget *parent = 0);

    void setSpecificProp(const QString& prop, const QString& value) override;
    QString getSpecificProp(const QString& prop) override;

};


#endif // PROGRESSBAR_H
