//
//  Created by Kevin Wojniak on 4/11/15.
//  Copyright 2015-2017 Kevin Wojniak. All rights reserved.
//

#include "mustache.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using kainjow::mustache;
using data = mustache::data;

TEST_CASE("variables") {

    SECTION("empty") {
        mustache tmpl("");
        data data;
        CHECK(tmpl.render(data).empty());
    }

    SECTION("none") {
        mustache tmpl("Hello");
        data data;
        CHECK(tmpl.render(data) == "Hello");
    }

    SECTION("single_miss") {
        mustache tmpl("Hello {{name}}");
        data data;
        CHECK(tmpl.render(data) == "Hello ");
    }

    SECTION("single_exist") {
        mustache tmpl("Hello {{name}}");
        data data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }

    SECTION("single_exist_wide") {
        kainjow::mustachew tmpl(L"Hello {{name}}");
        kainjow::mustachew::data data;
        data.set(L"name", L"Steve");
        CHECK(tmpl.render(data) == L"Hello Steve");
    }

    SECTION("escape") {
        mustache tmpl("Hello {{name}}");
        data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello &quot;S&quot;&lt;br&gt;te&amp;v&apos;e");
    }
    
    SECTION("unescaped1") {
        mustache tmpl("Hello {{{name}}}");
        data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2") {
        mustache tmpl("Hello {{&name}}");
        data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2_spaces") {
        mustache tmpl("Hello {{   &      name  }}");
        data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }
    
    SECTION("empty_name") {
        mustache tmpl("Hello {{}}");
        data data;
        data.set("", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }

    SECTION("braces") {
        mustache tmpl("my {{var}}");
        data data;
        data.set("var", "{{te}}st");
        CHECK(tmpl.render(data) == "my {{te}}st");
    }

}

TEST_CASE("comments") {
    
    SECTION("simple") {
        mustache tmpl("<h1>Today{{! ignore me }}.</h1>");
        data data;
        CHECK(tmpl.render(data) == "<h1>Today.</h1>");
    }

    SECTION("newlines") {
        mustache tmpl("Hello\n{{! ignore me }}\nWorld\n");
        data data;
        CHECK(tmpl.render(data) == "Hello\n\nWorld\n");
    }

}

TEST_CASE("set_delimiter") {

    SECTION("basic") {
        mustache tmpl("{{name}}{{=<% %>=}}<% name %><%={{ }}=%>{{ name }}");
        data data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "SteveSteveSteve");
    }

    SECTION("small") {
        mustache tmpl("{{n}}{{=a b=}}anba={{ }}=b{{n}}");
        data data;
        data.set("n", "s");
        CHECK(tmpl.render(data) == "sss");
    }
    
    SECTION("noreset") {
        mustache tmpl("{{=[ ]=}}[name] [x] + [y] = [sum]");
        data data;
        data.set("name", "Steve");
        data.set("x", "1");
        data.set("y", "2");
        data.set("sum", "3");
        CHECK(tmpl.render(data) == "Steve 1 + 2 = 3");
    }
    
    SECTION("whitespace") {
        mustache tmpl("|{{= @   @ =}}|");
        data data;
        REQUIRE(tmpl.is_valid());
        CHECK(tmpl.render(data) == "||");
    }

}

TEST_CASE("sections") {
    
    SECTION("nonexistant") {
        mustache tmpl("{{#var}}not shown{{/var}}");
        data data;
        CHECK(tmpl.render(data) == "");
    }

    SECTION("false") {
        mustache tmpl("{{#var}}not shown{{/var}}");
        data dat;
        dat.set("var", data(data::type::bool_false));
        CHECK(tmpl.render(dat) == "");
    }

    SECTION("emptylist") {
        mustache tmpl("{{#var}}not shown{{/var}}");
        data dat;
        dat.set("var", data(data::type::list));
        CHECK(tmpl.render(dat) == "");
    }
    
    SECTION("nested") {
        mustache tmpl("{{#var1}}hello{{#var2}}world{{/var2}}{{/var1}}");
        data data;
        data.set("var1", data::type::bool_true);
        data.set("var2", data::type::bool_true);
        CHECK(tmpl.render(data) == "helloworld");
    }

}

TEST_CASE("sections_inverted") {
    
    SECTION("nonexistant") {
        mustache tmpl("{{^var}}shown{{/var}}");
        CHECK(tmpl.render(data()) == "shown");
    }
    
    SECTION("false") {
        mustache tmpl("{{^var}}shown{{/var}}");
        data dat("var", data(data::type::bool_false));
        CHECK(tmpl.render(dat) == "shown");
    }
    
    SECTION("emptylist") {
        mustache tmpl("{{^var}}shown{{/var}}");
        data dat("var", data(data::type::list));
        CHECK(tmpl.render(dat) == "shown");
    }
    
}

TEST_CASE("section_lists") {
    
    SECTION("list") {
        mustache tmpl("{{#people}}Hello {{name}}, {{/people}}");
        data people = data::type::list;
        for (auto& name : {"Steve", "Bill", "Tim"}) {
            people.push_back(data("name", name));
        }
        data data("people", people);
        CHECK(tmpl.render(data) == "Hello Steve, Hello Bill, Hello Tim, ");
    }
    
    SECTION("nested") {
        mustache tmpl("{{#families}}surname={{surname}}, members={{#members}}{{given}},{{/members}}|{{/families}}");
        data families = data::type::list;
        data family1;
        family1.set("surname", "Smith");
        data members1 = data::type::list;
        data member1a; member1a.set("given", "Steve"); members1.push_back(member1a);
        data member1b; member1b.set("given", "Joe"); members1.push_back(member1b);
        family1.set("members", members1);
        data family2;
        family2.set("surname", "Lee");
        data members2 = data::type::list;
        data member2a; member2a.set("given", "Bill"); members2.push_back(member2a);
        data member2b; member2b.set("given", "Peter"); members2.push_back(member2b);
        family2.set("members", members2);
        families.push_back(family1);
        families.push_back(family2);
        data data;
        data.set("families", families);
        CHECK(tmpl.render(data) == "surname=Smith, members=Steve,Joe,|surname=Lee, members=Bill,Peter,|");
    }
    
    SECTION("dot") {
        mustache tmpl("{{#names}}Hello {{.}}, {{/names}}");
        data names = data::type::list;
        names.push_back(data("Steve"));
        names.push_back(data("Bill"));
        names.push_back(data("Tim"));
        data data("names", names);
        CHECK(tmpl.render(data) == "Hello Steve, Hello Bill, Hello Tim, ");
    }

    SECTION("dot2") {
        mustache tmpl("{{#names}}Hello {{.}}{{/names}}{{#friends}} and {{.}}{{/friends}}");
        data friends = data::type::list;
        friends.push_back("Bill");
        friends.push_back("Tim");
        data data;
        data.set("friends", friends);
        CHECK(tmpl.render(data) == " and Bill and Tim");
        data.set("names", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve and Bill and Tim");
    }

}

TEST_CASE("section_object") {
    
    SECTION("basic") {
        mustache tmpl("{{#employee}}name={{name}}, age={{age}}{{/employee}}");
        data person;
        person.set("name", "Steve");
        person.set("age", "42");
        data data;
        data.set("employee", person);
        CHECK(tmpl.render(data) == "name=Steve, age=42");
    }

    SECTION("basic_parent") {
        mustache tmpl("({{subject}}) {{#employee}}name={{name}}, age={{age}} - {{subject}}{{/employee}}");
        data person;
        person.set("name", "Steve");
        person.set("age", "42");
        person.set("subject", "email");
        data data;
        data.set("subject", "test");
        data.set("employee", person);
        CHECK(tmpl.render(data) == "(test) name=Steve, age=42 - email");
    }

}

TEST_CASE("examples") {
    
    SECTION("one") {
        mustache tmpl{"Hello {{what}}!"};
        std::cout << tmpl.render({"what", "World"}) << std::endl;
        CHECK(tmpl.is_valid());
        CHECK(tmpl.error_message() == "");
        CHECK(tmpl.render({"what", "World"}) == "Hello World!");
    }

    SECTION("two") {
        mustache tmpl{"{{#employees}}{{name}}, {{/employees}}"};
        data employees{data::type::list};
        employees << data{"name", "Steve"} << data{"name", "Bill"};
        std::ostream& stream = tmpl.render({"employees", employees}, std::cout) << std::endl;
        CHECK(tmpl.is_valid());
        CHECK(tmpl.error_message() == "");
        CHECK(tmpl.render({"employees", employees}) == "Steve, Bill, ");
        CHECK(&stream == &std::cout);
    }

    SECTION("three") {
        mustache tmpl("Hello {{what}}!");
        std::stringstream ss;
        tmpl.render({"what", "World"}, [&ss](const std::string& str) {
            ss << str;
        });
        CHECK(tmpl.is_valid());
        CHECK(tmpl.error_message() == "");
        CHECK(ss.str() == "Hello World!");
    }

}

TEST_CASE("data") {

    SECTION("types") {
        data dat("age", "42");
        data emptyStr = data::type::string;
        dat["name"] = "Steve";
        dat["is_human"] = data::type::bool_true;
        const data* name;
        const data* age;
        const data* is_human;
        name = dat.get("name");
        age = dat.get("age");
        is_human = dat.get("is_human");
        CHECK(name != (const data*)0);
        CHECK(age != (const data*)0);
        CHECK(is_human != (const data*)0);
        CHECK(dat.get("miss") == (const data*)0);
        REQUIRE(name->is_string());
        CHECK(name->string_value() == "Steve");
        REQUIRE(age->is_string());
        CHECK(age->string_value() == "42");
        CHECK(is_human->is_true());
        CHECK(is_human->is_bool());
        CHECK(emptyStr.is_string());
        CHECK(emptyStr.string_value() == "");
    }

    SECTION("move_ctor") {
        data obj1{data::type::list};
        CHECK(obj1.is_list());
        data obj2{std::move(obj1)};
        CHECK(obj2.is_list());
        CHECK(obj1.is_invalid());
        obj2.push_back({"name", "Steve"}); // this should puke if the internal data isn't setup correctly
    }

    SECTION("move_assign") {
        data obj1{data::type::list};
        CHECK(obj1.is_list());
        data obj2 = std::move(obj1);
        CHECK(obj2.is_list());
        CHECK(obj1.is_invalid());
        obj2.push_back({"name", "Steve"}); // this should puke if the internal data isn't setup correctly

        data lambda1{data::lambda_type{[](const std::string&){ return "{{#what}}"; }}};
        data lambda2 = std::move(lambda1);
        CHECK(lambda2.is_lambda());
        CHECK(lambda1.is_invalid());
    }

}

TEST_CASE("errors") {

    SECTION("unclosed_section") {
        mustache tmpl("test {{#employees}}");
        CHECK_FALSE(tmpl.is_valid());
        CHECK(tmpl.error_message() == "Unclosed section \"employees\" at 5");
    }
    
    SECTION("unclosed_section_nested") {
        mustache tmpl("{{#var1}}hello{{#var2}}world");
        data data;
        data.set("var1", data::type::bool_true);
        data.set("var2", data::type::bool_true);
        CHECK(tmpl.render(data) == "");
        CHECK(tmpl.is_valid() == false);
        CHECK(tmpl.error_message() == "Unclosed section \"var1\" at 0");
    }

    SECTION("unclosed_section_nested2") {
        mustache tmpl("{{#var1}}hello{{#var2}}world{{/var1}}");
        data data;
        data.set("var1", data::type::bool_true);
        data.set("var2", data::type::bool_true);
        CHECK(tmpl.render(data) == "");
        CHECK(tmpl.is_valid() == false);
        CHECK(tmpl.error_message() == "Unclosed section \"var1\" at 0");
    }

    SECTION("unclosed_tag") {
        mustache tmpl("test {{employees");
        CHECK_FALSE(tmpl.is_valid());
        CHECK(tmpl.error_message() == "Unclosed tag at 5");
    }
    
    SECTION("unopened_section") {
        mustache tmpl("test {{/employees}}");
        CHECK_FALSE(tmpl.is_valid());
        CHECK(tmpl.error_message() == "Unopened section \"employees\" at 5");
    }
    
    SECTION("invalid_set_delimiter") {
        std::vector<std::string> invalids;
        invalids.push_back("test {{=< =}}");  // not 5 characters
        invalids.push_back("test {{=....}}"); // not ending with =
        invalids.push_back("test {{=...=}}"); // does not contain space
        invalids.push_back("test {{=.  ==}}"); // can't contain equal sign
        invalids.push_back("test {{==  .=}}"); // can't contain equal sign
        invalids.push_back("test {{=[ ] ] ]=}}"); // can't contain space
        invalids.push_back("test {{=[ [ ]=}}"); // can't contain space
        std::vector<std::string>::size_type total = 0;
        for (const auto& str: invalids) {
            mustache tmpl(str);
            CHECK_FALSE(tmpl.is_valid());
            CHECK(tmpl.error_message() == "Invalid set delimiter tag at 5");
            ++total;
        }
        CHECK(total == invalids.size());
        CHECK(total == 7);
    }
    
    SECTION("lambda") {
        mustache tmpl{"Hello {{lambda}}!"};
        data dat("lambda", data{data::lambda_type{[](const std::string&){
            return "{{#what}}";
        }}});
        CHECK(tmpl.is_valid() == true);
        CHECK(tmpl.error_message() == "");
        CHECK(tmpl.render(dat) == "Hello ");
        CHECK(tmpl.is_valid() == false);
        CHECK(tmpl.error_message() == "Unclosed section \"what\" at 0");
    }

    SECTION("lambda2") {
        mustache tmpl{"Hello {{lambda}}!"};
        data dat("lambda", data{data::lambda_type{[](const std::string&){
            return "{{what}}";
        }}});
        dat["what"] = data{data::lambda_type{[](const std::string&){
            return "{{#blah}}";
        }}};
        CHECK(tmpl.is_valid() == true);
        CHECK(tmpl.error_message() == "");
        CHECK(tmpl.render(dat) == "Hello ");
        CHECK(tmpl.is_valid() == false);
        CHECK(tmpl.error_message() == "Unclosed section \"blah\" at 0");
    }

    SECTION("partial") {
        mustache tmpl{"Hello {{>partial}}!"};
        data dat("partial", data{data::partial_type{[](){
            return "{{#what}}";
        }}});
        CHECK(tmpl.is_valid() == true);
        CHECK(tmpl.error_message() == "");
        CHECK(tmpl.render(dat) == "Hello ");
        CHECK(tmpl.is_valid() == false);
        CHECK(tmpl.error_message() == "Unclosed section \"what\" at 0");
    }

    SECTION("partial2") {
        mustache tmpl{"Hello {{>partial}}!"};
        data data("partial", {data::partial_type{[](){
            return "{{what}}";
        }}});
        data["what"] = data::lambda_type{[](const std::string&){
            return "{{#blah}}";
        }};
        CHECK(tmpl.is_valid() == true);
        CHECK(tmpl.error_message() == "");
        CHECK(tmpl.render(data) == "Hello ");
        CHECK(tmpl.is_valid() == false);
        CHECK(tmpl.error_message() == "Unclosed section \"blah\" at 0");
    }
    
    SECTION("section_lambda") {
        mustache tmpl{"{{#what}}asdf{{/what}}"};
        data data("what", data::lambda_type{[](const std::string&){
            return "{{blah";
        }});
        CHECK(tmpl.is_valid() == true);
        CHECK(tmpl.error_message() == "");
        CHECK(tmpl.render(data) == "");
        CHECK(tmpl.is_valid() == false);
        CHECK(tmpl.error_message() == "Unclosed tag at 0");
    }

}

TEST_CASE("partials") {

    SECTION("empty") {
        mustache tmpl{"{{>header}}"};
        data data;
        CHECK(tmpl.render(data) == "");
    }

    SECTION("basic") {
        mustache tmpl{"{{>header}}"};
        data::partial_type partial = []() {
            return "Hello World";
        };
        data dat("header", data{partial});
        CHECK(tmpl.render(dat) == "Hello World");
    }

    SECTION("context") {
        mustache tmpl{"{{>header}}"};
        data::partial_type partial{[]() {
            return "Hello {{name}}";
        }};
        data dat("header", data{partial});
        dat["name"] = "Steve";
        CHECK(tmpl.render(dat) == "Hello Steve");
    }
    
    SECTION("nested") {
        mustache tmpl{"{{>header}}"};
        data::partial_type header{[]() {
            return "Hello {{name}} {{>footer}}";
        }};
        data::partial_type footer{[]() {
            return "Goodbye {{#names}}{{.}}|{{/names}}";
        }};
        data names{data::type::list};
        names.push_back("Jack");
        names.push_back("Jill");
        data dat("header", header);
        dat["name"] = "Steve";
        dat["footer"] = data{footer};
        dat["names"] = data{names};
        CHECK(tmpl.render(dat) == "Hello Steve Goodbye Jack|Jill|");
    }

    SECTION("dotted") {
        mustache tmpl{"{{>a.b}}"};
        data::partial_type a_b{[]() {
            return "test";
        }};
        data data("a.b", a_b);
        CHECK(tmpl.render(data) == "test");
    }
}

TEST_CASE("lambdas") {
    
    SECTION("basic") {
        mustache tmpl{"{{lambda}}"};
        data dat("lambda", data{data::lambda_type{[](const std::string&){
            return "Hello {{planet}}";
        }}});
        dat["planet"] = "world";
        CHECK(tmpl.render(dat) == "Hello world");
    }

    SECTION("delimiters") {
        mustache tmpl{"{{= | | =}}Hello, (|&lambda|)!"};
        data dat("lambda", data{data::lambda_type{[](const std::string&){
            return "|planet| => {{planet}}";
        }}});
        dat["planet"] = "world";
        CHECK(tmpl.render(dat) == "Hello, (|planet| => world)!");
    }

    SECTION("nocaching") {
        mustache tmpl{"{{lambda}} == {{{lambda}}} == {{lambda}}"};
        int calls = 0;
        data dat("lambda", data{data::lambda_type{[&calls](const std::string&){
            ++calls;
            return std::to_string(calls);
        }}});
        CHECK(tmpl.render(dat) == "1 == 2 == 3");
    }

    SECTION("escape") {
        mustache tmpl{"<{{lambda}}{{{lambda}}}"};
        data dat("lambda", data{data::lambda_type{[](const std::string&){
            return ">";
        }}});
        CHECK(tmpl.render(dat) == "<&gt;>");
    }
    
    SECTION("section") {
        mustache tmpl{"<{{#lambda}}{{x}}{{/lambda}}>"};
        data dat("lambda", data{data::lambda_type{[](const std::string& text){
            return text == "{{x}}" ? "yes" : "no";
        }}});
        CHECK(tmpl.render(dat) == "<yes>");
    }

    SECTION("section_expansion") {
        mustache tmpl{"<{{#lambda}}-{{/lambda}}>"};
        data dat("lambda", data{data::lambda_type{[](const std::string& text){
            return text + "{{planet}}" + text;
        }}});
        dat["planet"] = "Earth";
        CHECK(tmpl.render(dat) == "<-Earth->");
    }

    SECTION("section_alternate_delimiters") {
        mustache tmpl{"{{= | | =}}<|#lambda|-|/lambda|>"};
        data dat("lambda", data{data::lambda_type{[](const std::string& text){
            return text + "{{planet}} => |planet|" + text;
        }}});
        dat["planet"] = "Earth";
        CHECK(tmpl.render(dat) == "<-{{planet}} => Earth->");
    }

    const data::lambda_type sectionLambda{[](const std::string& text){
        return "__" + text + "__";
    }};

    SECTION("section_multiple_calls") {
        mustache tmpl{"{{#lambda}}FILE{{/lambda}} != {{#lambda}}LINE{{/lambda}}"};
        data data("lambda", sectionLambda);
        CHECK(tmpl.render(data) == "__FILE__ != __LINE__");
    }

    SECTION("section_inverted") {
        mustache tmpl{"<{{^lambda}}{{static}}{{/lambda}}>"};
        data data("lambda", sectionLambda);
        data["static"] = "static";
        CHECK(tmpl.render(data) == "<>");
    }

}

TEST_CASE("dotted_names") {
    
    SECTION("basic") {
        mustache tmpl{"\"{{person.name}}\" == \"{{#person}}{{name}}{{/person}}\""};
        data person{"name", "Joe"};
        CHECK(tmpl.render({"person", person}) == "\"Joe\" == \"Joe\"");
    }

    SECTION("triple_mustache") {
        mustache tmpl{"\"{{{person.name}}}\" == \"{{#person}}{{name}}{{/person}}\""};
        data person{"name", "Joe"};
        CHECK(tmpl.render({"person", person}) == "\"Joe\" == \"Joe\"");
    }

    SECTION("ampersand") {
        mustache tmpl{"\"{{&person.name}}\" == \"{{#person}}{{&name}}{{/person}}\""};
        data person{"name", "Joe"};
        CHECK(tmpl.render({"person", person}) == "\"Joe\" == \"Joe\"");
    }

    SECTION("depth") {
        mustache tmpl{"\"{{a.b.c.d.e.name}}\" == \"Phil\""};
        data data{"a", {"b", {"c", {"d", {"e", {"name", "Phil"}}}}}};
        CHECK(tmpl.render(data) == "\"Phil\" == \"Phil\"");
    }

    SECTION("broken_chains1") {
        mustache tmpl{"\"{{a.b.c}}\" == \"\""};
        data data{"a", data::type::list};
        CHECK(tmpl.render(data) == "\"\" == \"\"");
    }

    SECTION("broken_chains2") {
        mustache tmpl{"\"{{a.b.c.name}}\" == \"\""};
        data data;
        data["a"] = {"b", data::type::list};
        data["c"] = {"name", "Jim"};
        CHECK(tmpl.render(data) == "\"\" == \"\"");
    }

    SECTION("depth2") {
        mustache tmpl{"\"{{#a}}{{b.c.d.e.name}}{{/a}}\" == \"Phil\""};
        data data;
        data["a"] = {"b", {"c", {"d", {"e", {"name", "Phil"}}}}};
        data["b"] = {"c", {"d", {"e", {"name", "Wrong"}}}};
        CHECK(tmpl.render(data) == "\"Phil\" == \"Phil\"");
    }

    SECTION("scope") {
        mustache tmpl{"\"{{#a}}{{b.name}}{{/a}}\" == \"Phil\""};
        data data;
        data["a"] = {"x", "y"};
        data["b"] = {"name", "Phil"};
        CHECK(tmpl.render(data) == "\"Phil\" == \"Phil\"");
    }
}
