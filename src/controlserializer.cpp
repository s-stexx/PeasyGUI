// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "controlserializer.h"

QString ControlSerializer::serialize(const QList<Control*>& controls) {
    QString result;

    for (int idxControl = 0; idxControl < controls.size(); ++idxControl) {
        Control* control = controls[idxControl];
        if (control->getProp("Show") != "ToBeDeleted") {
            QString name = control->getProp("Name");
            if (idxControl > 0) {
                result += "new`" + name + "`Type`" + control->getProp("Type") + "`\n";
            }

            const auto props = control->getAllPropInfos();

            // Magic number 2: 0 = Name, 1 = Type are covered by "new"
            for (int idxProp = 2; idxProp < props.size(); ++idxProp) {
                const PropInfo& prop = props[idxProp];

                // "W" flag = should be written to file
                if (prop.flagsSetWrite.contains("W")) {
                    QString value = control->getProp(prop.propName);
                    if (value != prop.defaultValue) {
                        result += "set`" + name + "`" + prop.propName + "`" + value + "`\n";
                    }
                }
            }
            result += "\n";
        }
    }

    return result;
}
