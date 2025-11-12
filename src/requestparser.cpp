// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Stefan Stechert

#include "requestparser.h"

QList<Request> RequestParser::parseAll(const QString &input)
{
    QStringList tokens = input.split('`');

    QList<Request> parsedRequests;

    int index = 0;

    while (index < tokens.size()) {
        Request request = parseSingle(tokens, index);

        // is at the end of tokens = empty token
        if (request.action.isEmpty()) {
            break;

        // there is an error
        } else if (! request.error.isEmpty()) {
            parsedRequests.append(request);
            break;

        // normal, go on
        } else {
            parsedRequests.append(request);
        }

    }

    return parsedRequests;
}


Request RequestParser::parseSingle(const QStringList &tokens, int &index)
{
    Request result;

    QString lastTokenTrimmed = tokens.last().trimmed();

    if ( ! lastTokenTrimmed.isEmpty() && lastTokenTrimmed != "events?") {
        result.error = "All requests must end with a separator-char";
        result.action = "No valid action";

        return result;
    }

    enum State {
        ExpectAction,
        ExpectName,
        ExpectProp,
        ExpectValue,
        CollectExtraValue,
        Done,
        Error
    };

    State state = ExpectAction;
    QString valueAccum;

    while (state != Done && state != Error) {
        switch (state) {
        case ExpectAction:
            if (index < tokens.size()) {
                QString trimmed = tokens[index].trimmed();
                if (trimmed == "set" || trimmed == "new" || trimmed == "get" || trimmed == "events?") {
                    result.action = trimmed;
                    index++;
                    if (trimmed == "events?") {
                        state = Done;
                    } else {
                        state = ExpectName;
                    }
                } else {
                    result.action = trimmed;
                    result.error = "Unknown action: " + trimmed;
                    state = Error;
                }
            } else {
                state = Done;
            }

            break;
        case ExpectName:
            if (index < tokens.size() - 1) {
                result.name = tokens[index];
                index++;
                state = ExpectProp;
            } else {
                result.error = "Name of control element missing";
                state = Error;
            }

            break;
        case ExpectProp:
            if (index < tokens.size() - 1) {
                result.prop = tokens[index];
                index++;

                if (result.action == "get") {
                    state = Done;
                } else {
                    state = ExpectValue;
                }
            } else {
                result.error = "Property missing";
                state = Error;
            }

            break;
        case ExpectValue:
            if (index < tokens.size() - 1) {
                valueAccum = tokens[index];
                index++;
                if (result.prop.startsWith("Entire")) {
                    if (valueAccum.contains("\n")) {
                        result.error = "No line breaks in values from Entire-Property";
                        state = Error;
                    } else {
                        state = CollectExtraValue;
                    }
                } else {
                    result.value = valueAccum;
                    state = Done;
                }
            } else {
                result.error = "Value missing";
                state = Error;
            }

            break;
        case CollectExtraValue:
            while (index < tokens.size()) {
                QString t = tokens[index];
                if (t.trimmed().isEmpty() || t.contains("\n")) {
                    break;
                }
                valueAccum += "`" + t;
                index++;
            }
            result.value = valueAccum;
            state = Done;

            break;
        default:
            result.error = "Internal error";
            state = Error;

            break;
        }
    }

    return result;
}
