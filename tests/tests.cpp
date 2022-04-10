#include "../ftb/types.hpp"
#include "../ftb/testing.hpp"
#include "../ftb/macros.hpp"
#include "../xml.hpp"
#include "../db.cpp"

auto test_parse_messages_single_stop() -> testresult {
    using namespace db;
    char* copy = heap_copy_c_string(R"XML(
<timetable>
  <s id="-7173313963166475560-2204101908-8" eva="8004775">
      <m id="r1556465" t="h" from="2204092230" to="2204102359" cat="Information" ts="2204092238" ts-tts="22-04-09 23:16:07.937" pr="2"/>
      <m id="r1563275" t="h" from="2204090000" to="2207302359" cat="Information" ts="2203310911" ts-tts="22-04-09 23:16:08.095" pr="2"/>
      <m id="r1527864" t="h" from="2204090000" to="2204102359" cat="Störung" ts="2202151139" ts-tts="22-04-09 23:16:07.787" pr="3"/>
      <ar cpth="Nürnberg Hbf|Allersberg(Rothsee)|Kinding(Altmühltal)|Ingolstadt Nord|Ingolstadt Hbf" cs="c" clt="2204090119" l="1">
          <m id="r116052704" t="d" c="36" ts="2204090119" ts-tts="22-04-09 01:19:50.750"/>
          <m id="r116055566" t="d" c="36" ts="2204090403" ts-tts="22-04-09 04:03:48.281"/>
      </ar>
      <dp cpth="" cs="c" clt="2204090119" l="1">
          <m id="r116052704" t="d" c="36" ts="2204090119" ts-tts="22-04-09 01:19:50.750"/>
          <m id="r116055566" t="d" c="36" ts="2204090403" ts-tts="22-04-09 04:03:48.281"/>
      </dp>
  </s>
</timetable>
)XML");

    Timetable timetable {};
    timetable.messages.init();
    timetable.stops.init();
    defer { timetable.free(); };

    xml::push_handler_to_parser_stack({
        .key           = "timetable",
        .open_handler  = timetable_parser_open,
        .close_handler = timetable_parser_close,
        .user_data     = &timetable
    });
    xml::parse(copy);
    xml::pop_handler_from_parser_stack(); // timetable

    assert_equal_int(timetable.messages.count, 0);
    assert_equal_int(timetable.stops.count, 1);
    assert_equal_int(timetable.stops[0].messages.count, 3);
    assert_true(timetable.stops[0].arrival_event.__exists);
    assert_true(timetable.stops[0].depature_event.__exists);
    assert_equal_int(timetable.stops[0].arrival_event.messages.count, 2);
    assert_equal_int(timetable.stops[0].depature_event.messages.count, 2);

    // timetable.print();

    return pass;
}

auto test_maybe_struct_types() -> testresult {
    struct Test {
        int t;
    };
    Maybe<Test> t = {};


    assert_true(!t.__exists);
    assert_true(!(bool)t);
    assert_true(!t.operator bool());
    assert_equal_int(t.t, 0);

    t = { 12 };


    assert_true(t.__exists);
    assert_true((bool)t);
    assert_true(t.operator bool());
    assert_equal_int(t.t, 12);


    return pass;
}

auto test_maybe_basic_types() -> testresult {
    Maybe<s32> i = {};

    assert_true(!i.__exists);
    assert_true(!(bool)i);
    assert_true(!i.operator bool());
    assert_equal_int(i.value, 0);

    i = 12;
    i.__exists = true;

    assert_true(i.__exists);
    assert_true((bool)i);
    assert_true(i.operator bool());
    assert_equal_int(i.value, 12);

    return pass;
}

int main() {
    db::init();
    defer { db::deinit(); };

    testresult result;
    invoke_test(test_parse_messages_single_stop);
    invoke_test(test_maybe_basic_types);
    invoke_test(test_maybe_struct_types);


}
