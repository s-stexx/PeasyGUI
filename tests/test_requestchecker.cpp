#include <QObject>
#include <QTest>

#include "../src/request.h"
#include "../src/requestchecker.h"
#include "../src/control.h"
#include "mockcontrol.h"

class test_requestchecker : public QObject {
    Q_OBJECT

private:
    MockControl window;
    MockControl listBox;
    QList<Control*> controls;
    const Prototypes& prototypes = getPrototypes();

private slots:
    void initTestCase() {
        // Setup reusable mock controls
        window.setProp("Name", "MyWindow");
        window.setProp("Type", "Window");
        window.setProp("Events", "KeyPressed,Resized");

        listBox.setProp("Name", "MyListBox");
        listBox.setProp("Type", "ListBox");
        listBox.setProp("Geometry", "10,42,80,32");
        listBox.setProp("EntireTexts", "A`B`C");
        listBox.setProp("Selected", "1,2");
        listBox.setProp("MultiSelect", "False");

        controls << &window << &listBox;
    }

    void good_check_events() {
        QString result;

        QList<Request> requests = {
            Request{"events?", "", "", "", ""}
        };

        RequestChecker::checkOnlyOneInfoRequestInBlock(result, requests);

        QVERIFY(result.isEmpty());
    }

    void bad_multiple_requests_one_info() {
        QString result;

        QList<Request> requests = {
            Request{"set", "", "", "", ""},
            Request{"get", "", "", "", ""}
        };

        RequestChecker::checkOnlyOneInfoRequestInBlock(result, requests);

        QVERIFY(result.contains("might not be combined with any other action"));
    }

    void bad_multiple_info_requests() {
        QString result;

        QList<Request> requests = {
            Request{"events?", "", "", "", ""},
            Request{"get", "", "", "", ""}
        };

        RequestChecker::checkOnlyOneInfoRequestInBlock(result, requests);

        QVERIFY(result.contains("might not be combined with any other action"));
    }

    void good_init_with_2_controls() {
        QStringList controlNames;
        QStringList controlTypes;
        ListBoxTexts listBoxTexts;

        QStringList expectedNames{"MyWindow", "MyListBox"};
        QStringList expectedTypes{"Window", "ListBox"};
        ListBoxTexts expectedLBT;
        expectedLBT[1] = "A`B`C";

        RequestChecker::initializeVirtualsWithExistingControls(controlNames, controlTypes, listBoxTexts, controls);

        QCOMPARE(controlNames, expectedNames);
        QCOMPARE(controlTypes, expectedTypes);
        QCOMPARE(listBoxTexts, expectedLBT);
    }

    void index_of_existing_controlnames() {
        QString result;
        QStringList controlNames{"MyWindow", "MyListBox"};

        int index = RequestChecker::getControlIndexByName("MyListBox", controlNames);
        QCOMPARE(index, 1);

        RequestChecker::checkControlExists(result, index);
        QCOMPARE(result, "");

        RequestChecker::checkNewControlNameIsUnique(result, index);
        QCOMPARE(result, "A control with this name does already exist");
    }

    void no_control_with_this_name() {
        QString result;
        QStringList controlNames{"MyWindow", "MyListBox"};

        int index = RequestChecker::getControlIndexByName("DoesNotExist", controlNames);
        QCOMPARE(index, -1);

        RequestChecker::checkNewControlNameIsUnique(result, index);
        QCOMPARE(result, "");

        RequestChecker::checkControlExists(result, index);
        QCOMPARE(result, "A control with this name could not be found");
    }

    void good_spawn_of_existing_control_type() {
        QString result;

        Request request{"new", "Button_42", "Type", "Button", ""};

        QList<PropInfo> propInfos = RequestChecker::getAllPropInfos(request.value, prototypes);

        RequestChecker::checkIfTypeExists(result, request.prop, propInfos);

        QVERIFY(result.isEmpty());
    }

    void bad_spawn_of_non_existing_control_type() {
        QString result;

        Request request{"new", "Button_42", "Type", "DoesNotExist", ""};

        QList<PropInfo> propInfos = RequestChecker::getAllPropInfos(request.value, prototypes);

        RequestChecker::checkIfTypeExists(result, request.prop, propInfos);

        QVERIFY(result.contains("must have the property 'Type' and an existing control type"));
    }

    void bad_try_spawning_new_window() {
        QString result;

        Request request{"new", "Win_42", "Type", "Window", ""};

        RequestChecker::checkIfWindowShouldBeSpawned(result, request.value);

        QVERIFY(result.contains("is a one window application"));
    }

    void good_set_existing_prop() {
        QString result;

        Request request{"set", "MyWindow", "Events", "", ""};

        QList<PropInfo> propInfos = RequestChecker::getAllPropInfos("Window", prototypes);
        RequestChecker::checkIfPropExists(result, request.prop, propInfos);

        QVERIFY(result.isEmpty());
    }

    void bad_set_non_existing_prop() {
        QString result;

        Request request{"set", "MyWindow", "DoesNotExist", "", ""};

        QList<PropInfo> propInfos = RequestChecker::getAllPropInfos("Window", prototypes);
        RequestChecker::checkIfPropExists(result, request.prop, propInfos);

        QVERIFY(result.contains("Unknown property"));
    }

    void good_set_settable_prop() {
        QString result;

        Request request{"set", "MyWindow", "Events", "", ""};

        QList<PropInfo> propInfos = RequestChecker::getAllPropInfos("Window", prototypes);
        RequestChecker::checkIfPropIsSettable(result, request.prop, propInfos);
        QCOMPARE(result, "");
    }

    void bad_try_to_set_non_settable_prop() {
        QString result;

        Request request{"set", "MyWindow", "Type", "NoSetPossible!", ""};

        QList<PropInfo> propInfos = RequestChecker::getAllPropInfos("Window", prototypes);
        RequestChecker::checkIfPropIsSettable(result, request.prop, propInfos);
        QCOMPARE(result, "Set not allowed for this property");
    }

    void good_all_indeces_are_in_range() {
        QString result;

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "0,2", "a`b`c");
        QCOMPARE(result, "");

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "1,2", "a`b`c");
        QCOMPARE(result, "");

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "1", "a`b`c");
        QCOMPARE(result, "");
    }

    void good_no_indeces_selected_by_empty() {
        QString result;

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "", "a`b`c");
        QCOMPARE(result, "");
    }

    void bad_indeces_float_and_string() {
        QString result;

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "1,a", "a`b`c");
        QVERIFY(result.contains("list indeces must be positive integers"));

        result = "";

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "1.0", "a`b`c");
        QVERIFY(result.contains("list indeces must be positive integers"));
    }

    void bad_indeces_out_of_range() {
        QString result;

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "3", "a`b`c");
        QVERIFY(result.contains("List indeces must be in range"));

        result = "";

        RequestChecker::checkIfSelectionIndecesAreInRangeOfItems(result, "0", "");
        QVERIFY(result.contains("List indeces must be in range"));
    }

    void good_all_in_multi_list() {
        QString result;

        RequestChecker::checkValidMulti(result, "Dragged,Clicked", "Multi:Clicked,Dragged,Scrolled");
        QCOMPARE(result, "");
    }

    void good_empty_must_not_be_in_list() {
        QString result;

        RequestChecker::checkValidMulti(result, "", "Multi:Clicked,Dragged,Scrolled");
        QCOMPARE(result, "");
    }

    void bad_one_keyword_not_in_the_list_of_possibles() {
        QString result;

        RequestChecker::checkValidMulti(result, "Clicked,Not_OK", "Multi:Clicked,Hovered");
        QVERIFY(result.contains("one of the values is not in the list"));
    }

    void good_integers_all_correct() {
        QString result;

        RequestChecker::checkAllIntegers(result, "-1,2,2", "Int:S,U,S");
        QCOMPARE(result, "");
    }

    void bad_integers_mixed_with_string() {
        QString result;

        RequestChecker::checkAllIntegers(result, "-1,BAD,2", "Int:S,U,S");
        QVERIFY(result.contains("All comma separated values must be integers"));
    }

    void bad_integers_wrong_signed() {
        QString result;

        RequestChecker::checkAllIntegers(result, "-1,-2,2", "Int:S,U,S");
        QVERIFY(result.contains("Found negative value, where positive is expected"));
    }

    void bad_wrong_integer_count() {
        QString result;

        RequestChecker::checkAllIntegers(result, "", "Int:S,U,S");
        QVERIFY(result.contains("Wrong number of Integers given"));

        result = "";

        RequestChecker::checkAllIntegers(result, "1", "Int:S,U,S");
        QVERIFY(result.contains("Wrong number of Integers given"));

        result = "";

        RequestChecker::checkAllIntegers(result, "1,1", "Int:S,U,S");
        QVERIFY(result.contains("Wrong number of Integers given"));

        result = "";

        RequestChecker::checkAllIntegers(result, "1,1,1,1", "Int:S,U,S");
        QVERIFY(result.contains("Wrong number of Integers given"));
    }

    void good_single() {
        QString result;

        RequestChecker::checkValidSingle(result, "b", "Single:a,b,ce");
        QCOMPARE(result, "");
    }

    void bad_empty_single() {
        QString result;

        RequestChecker::checkValidSingle(result, "", "Single:a,b,ce");
        QVERIFY(result.contains("Value might not be empty"));
    }

    void bad_single_not_one_of_the_possible() {
        QString result;

        RequestChecker::checkValidSingle(result, "e", "Single:a,b,ce");
        QVERIFY(result.contains("Value must be one of the possible choices"));
    }

    void good_checkall_one_set() {
        QList<Request> requests = {
            Request{"set", "MyWindow", "Events", "Closed,KeyPressed", ""}
        };

        QString result = RequestChecker::checkAll(requests, controls);

        QVERIFY(result.isEmpty());
    }

    void good_complex_rename_set_items_and_select_new() {
        QList<Request> requests = {
            Request{"set", "MyListBox", "Name", "MyListBox2", ""},
            Request{"set", "MyListBox2", "EntireTexts", "a`b`c`d", ""},
            Request{"set", "MyListBox2", "Selected", "0,3", ""},
            Request{"new", "MyListBox", "Type", "ListBox", ""}
        };

        QString result = RequestChecker::checkAll(requests, controls);

        QCOMPARE(result, "");
    }

    void bad_check_all_invalid_event_in_list() {
        QList<Request> requests = {
            Request{"set", "MyWindow", "Events", "Closed,InvalidEvent", ""}
        };

        QString result = RequestChecker::checkAll(requests, controls);

        QVERIFY(result.contains("At least one of the values is not in the list of possibles"));
    }
};

QTEST_MAIN(test_requestchecker)

#include "test_requestchecker.moc"
