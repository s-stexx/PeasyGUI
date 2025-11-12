#include <QObject>
#include <QTest>

#include "../src/requestparser.h"

class test_requestparser : public QObject {
    Q_OBJECT

private slots:
    void good_parse_events() {
        RequestParser parser;
        auto result = parser.parseAll("events?");

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].action, "events?");
        QCOMPARE(result[0].value, "");
    }

    void good_parse_events2() {
        RequestParser parser;
        auto result = parser.parseAll("events?`");

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].action, "events?");
        QCOMPARE(result[0].value, "");
    }

    void good_parse_get() {
        RequestParser parser;
        auto result = parser.parseAll("get`Button_1`Text`");

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].action, "get");
        QCOMPARE(result[0].name, "Button_1");
        QCOMPARE(result[0].prop, "Text");
        QCOMPARE(result[0].value, "");
    }

    void good_parse_set_with_newlines() {
        RequestParser parser;
        QString val = "set`Label_1`Text`Hello\nWorld\nAgain`";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].value, QString("Hello\nWorld\nAgain"));
    }

    void good_parse_entire() {
        RequestParser parser;
        QString val = "set`ListBox_1`EntireTexts`A`B`C`";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].value, "A`B`C");
    }

    void good_parse_multiple_requests() {
        RequestParser parser;
        QString val = "new`Button_1`Type`Button`\nset`Button_1`Text`OK`\n";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 2);

        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].action, "new");

        QVERIFY(result[1].error.isEmpty());
        QCOMPARE(result[1].action, "set");
        QCOMPARE(result[1].value, "OK");
    }

    void good_parse_multiple_requests_events_last() {
        RequestParser parser;
        QString val = "set`Button_1`Text`OK`\nevents?\n";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 2);

        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].action, "set");
        QCOMPARE(result[0].value, "OK");

        QVERIFY(result[1].error.isEmpty());
        QCOMPARE(result[1].action, "events?");
    }

    void good_parse_multiple_requests_with_entire_first() {
        RequestParser parser;
        QString val = "set`ListBox_1`EntireTexts`A`B`C`\nnew`Button_1`Type`Button`\n";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 2);

        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].action, "set");
        QCOMPARE(result[0].value, "A`B`C");

        QVERIFY(result[1].error.isEmpty());
        QCOMPARE(result[1].action, "new");
        QCOMPARE(result[1].value, "Button");
    }

    void bad_parse_multiple_requests_but_second_misses_value() {
        RequestParser parser;
        QString val = "new`Button_1`Type`Button`\nset`Button_1`Text`";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 2);

        QVERIFY(result[0].error.isEmpty());
        QCOMPARE(result[0].action, "new");

        QVERIFY(result[1].error.contains("Value missing"));
        QCOMPARE(result[1].action, "set");
        QCOMPARE(result[1].prop, "Text");
    }

    void bad_parse_invalid_command() {
        RequestParser parser;
        QString val = "bogus`arg1`arg2`";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.contains("Unknown action"));
    }

    void bad_parse_empty() {
        RequestParser parser;
        QString val = "";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 0);
    }

    void bad_parse_missing_property() {
        RequestParser parser;
        QString val = "get`TextBox_1`";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.contains("Property missing"));
    }

    void bad_parse_entire_with_newline_error() {
        RequestParser parser;
        QString val = "set`List_1`EntireTexts`Line1\nLine2`";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.contains("Entire-Property"));
    }

    void bad_parse_missing_separator_at_end() {
        RequestParser parser;
        QString val = "get`TextBox_1`Text";
        auto result = parser.parseAll(val);

        QCOMPARE(result.size(), 1);
        QVERIFY(result[0].error.contains("must end with a separator"));
    }

};

QTEST_MAIN(test_requestparser)

#include "test_requestparser.moc"
