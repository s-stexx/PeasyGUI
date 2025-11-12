// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#ifndef CONTROLSERIALIZER_H
#define CONTROLSERIALIZER_H

#include "control.h"

class ControlSerializer {
public:
    static QString serialize(const QList<Control*>& controls);
};

#endif // CONTROLSERIALIZER_H
