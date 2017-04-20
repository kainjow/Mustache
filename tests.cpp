//
//  Created by Kevin Wojniak on 4/11/15.
//  Copyright 2015-2017 Kevin Wojniak. All rights reserved.
//

#include "mustache.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using kainjow::mustache;
using kainjow::mustachew;
using data = mustache::data;

TEST_CASE("variables") {

    SECTION("empty") {
        mustache tmpl("");
        mustache::data data;
        CHECK(tmpl.render(data).empty());
    }

    SECTION("none") {
        mustache tmpl("Hello");
        mustache::data data;
        CHECK(tmpl.render(data) == "Hello");
    }

    SECTION("single_miss") {
        mustache tmpl("Hello {{name}}");
        mustache::data data;
        CHECK(tmpl.render(data) == "Hello ");
    }

    SECTION("single_exist") {
        mustache tmpl("Hello {{name}}");
        mustache::data data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }

    SECTION("single_exist_wide") {
        mustachew tmpl(L"Hello {{name}}");
        mustachew::data data;
        data.set(L"name", L"Steve");
        CHECK(tmpl.render(data) == L"Hello Steve");
    }

    SECTION("escape") {
        mustache tmpl("Hello {{name}}");
        mustache::data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello &quot;S&quot;&lt;br&gt;te&amp;v&apos;e");
    }
    
    SECTION("unescaped1") {
        mustache tmpl("Hello {{{name}}}");
        mustache::data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2") {
        mustache tmpl("Hello {{&name}}");
        mustache::data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2_spaces") {
        mustache tmpl("Hello {{   &      name  }}");
        mustache::data data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }
    
    SECTION("empty_name") {
        mustache tmpl("Hello {{}}");
        mustache::data data;
        data.set("", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }

    SECTION("braces") {
        mustache tmpl("my {{var}}");
        mustache::data data;
        data.set("var", "{{te}}st");
        CHECK(tmpl.render(data) == "my {{te}}st");
    }

}

TEST_CASE("comments") {
    
    SECTION("simple") {
        mustache tmpl("<h1>Today{{! ignore me }}.</h1>");
        mustache::data data;
        CHECK(tmpl.render(data) == "<h1>Today.</h1>");
    }

    SECTION("newlines") {
        mustache tmpl("Hello\n{{! ignore me }}\nWorld\n");
        mustache::data data;
        CHECK(tmpl.render(data) == "Hello\n\nWorld\n");
    }

}

TEST_CASE("set_delimiter") {

    SECTION("basic") {
        mustache tmpl("{{name}}{{=<% %>=}}<% name %><%={{ }}=%>{{ name }}");
        mustache::data data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "SteveSteveSteve");
    }

    SECTION("small") {
        mustache tmpl("{{n}}{{=a b=}}anba={{ }}=b{{n}}");
        mustache::data data;
        data.set("n", "s");
        CHECK(tmpl.render(data) == "sss");
    }
    
    SECTION("noreset") {
        mustache tmpl("{{=[ ]=}}[name] [x] + [y] = [sum]");
        mustache::data data;
        data.set("name", "Steve");
        data.set("x", "1");
        data.set("y", "2");
        data.set("sum", "3");
        CHECK(tmpl.render(data) == "Steve 1 + 2 = 3");
    }
    
    SECTION("whitespace") {
        mustache tmpl("|{{= @   @ =}}|");
        mustache::data data;
        REQUIRE(tmpl.isValid());
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
        data data;
        data.set("var", data(data::Type::False));
        CHECK(tmpl.render(data) == "");
    }

    SECTION("emptylist") {
        mustache tmpl("{{#var}}not shown{{/var}}");
        data data;
        data.set("var", data(data::Type::List));
        CHECK(tmpl.render(data) == "");
    }
    
    SECTION("nested") {
        mustache tmpl("{{#var1}}hello{{#var2}}world{{/var2}}{{/var1}}");
        data data;
        data.set("var1", data::Type::True);
        data.set("var2", data::Type::True);
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
        data data("var", data(data::Type::False));
        CHECK(tmpl.render(data) == "shown");
    }
    
    SECTION("emptylist") {
        mustache tmpl("{{^var}}shown{{/var}}");
        data data("var", data(data::Type::List));
        CHECK(tmpl.render(data) == "shown");
    }
    
}

TEST_CASE("section_lists") {
    
    SECTION("list") {
        mustache tmpl("{{#people}}Hello {{name}}, {{/people}}");
        data people = data::List();
        for (auto& name : {"Steve", "Bill", "Tim"}) {
            people.push_back(data("name", name));
        }
        data data("people", people);
        CHECK(tmpl.render(data) == "Hello Steve, Hello Bill, Hello Tim, ");
    }
    
    SECTION("nested") {
        mustache tmpl("{{#families}}surname={{surname}}, members={{#members}}{{given}},{{/members}}|{{/families}}");
        data families = data::List();
        data family1;
        family1.set("surname", "Smith");
        data members1 = data::List();
        data member1a; member1a.set("given", "Steve"); members1.push_back(member1a);
        data member1b; member1b.set("given", "Joe"); members1.push_back(member1b);
        family1.set("members", members1);
        data family2;
        family2.set("surname", "Lee");
        data members2 = data::List();
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
        data names = data::List();
        names.push_back(data("Steve"));
        names.push_back(data("Bill"));
        names.push_back(data("Tim"));
        data data("names", names);
        CHECK(tmpl.render(data) == "Hello Steve, Hello Bill, Hello Tim, ");
    }

    SECTION("dot2") {
        mustache tmpl("{{#names}}Hello {{.}}{{/names}}{{#friends}} and {{.}}{{/friends}}");
        data friends = data::List();
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
        CHECK(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render({"what", "World"}) == "Hello World!");
    }

    SECTION("two") {
        mustache tmpl{"{{#employees}}{{name}}, {{/employees}}"};
        data employees{data::List()};
        employees << data{"name", "Steve"} << data{"name", "Bill"};
        std::ostream& stream = tmpl.render({"employees", employees}, std::cout) << std::endl;
        CHECK(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render({"employees", employees}) == "Steve, Bill, ");
        CHECK(&stream == &std::cout);
    }

    SECTION("three") {
        mustache tmpl("Hello {{what}}!");
        std::stringstream ss;
        tmpl.render({"what", "World"}, [&ss](const std::string& str) {
            ss << str;
        });
        CHECK(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "");
        CHECK(ss.str() == "Hello World!");
    }

}

TEST_CASE("data") {

    SECTION("types") {
        data data("age", "42");
        data emptyStr = data::Type::String;
        data["name"] = "Steve";
        data["is_human"] = data::Type::True;
        const data* name;
        const data* age;
        const data* is_human;
        name = data.get("name");
        age = data.get("age");
        is_human = data.get("is_human");
        CHECK(name != (const data*)0);
        CHECK(age != (const data*)0);
        CHECK(is_human != (const data*)0);
        CHECK(data.get("miss") == (const data*)0);
        REQUIRE(name->isString());
        CHECK(name->stringValue() == "Steve");
        REQUIRE(age->isString());
        CHECK(age->stringValue() == "42");
        CHECK(is_human->isTrue());
        CHECK(is_human->isBool());
        CHECK(emptyStr.isString());
        CHECK(emptyStr.stringValue() == "");
    }

    SECTION("move_ctor") {
        data obj1{data::List()};
        CHECK(obj1.isList());
        data obj2{std::move(obj1)};
        CHECK(obj2.isList());
        CHECK(obj1.type() == data::Type::Invalid);
        obj2.push_back({"name", "Steve"}); // this should puke if the internal data isn't setup correctly
    }

    SECTION("move_assign") {
        data obj1{data::List()};
        CHECK(obj1.isList());
        data obj2 = std::move(obj1);
        CHECK(obj2.isList());
        CHECK(obj1.type() == data::Type::Invalid);
        obj2.push_back({"name", "Steve"}); // this should puke if the internal data isn't setup correctly

        data lambda1{data::LambdaType{[](const std::string&){ return "{{#what}}"; }}};
        data lambda2 = std::move(lambda1);
        CHECK(lambda2.isLambda());
        CHECK(lambda1.type() == data::Type::Invalid);
    }

}

TEST_CASE("errors") {

    SECTION("unclosed_section") {
        mustache tmpl("test {{#employees}}");
        CHECK_FALSE(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "Unclosed section \"employees\" at 5");
    }
    
    SECTION("unclosed_section_nested") {
        mustache tmpl("{{#var1}}hello{{#var2}}world");
        mustache::data data;
        data.set("var1", mustache::data::Type::True);
        data.set("var2", mustache::data::Type::True);
        CHECK(tmpl.render(data) == "");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"var1\" at 0");
    }

    SECTION("unclosed_section_nested2") {
        mustache tmpl("{{#var1}}hello{{#var2}}world{{/var1}}");
        mustache::data data;
        data.set("var1", mustache::data::Type::True);
        data.set("var2", mustache::data::Type::True);
        CHECK(tmpl.render(data) == "");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"var1\" at 0");
    }

    SECTION("unclosed_tag") {
        mustache tmpl("test {{employees");
        CHECK_FALSE(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "Unclosed tag at 5");
    }
    
    SECTION("unopened_section") {
        mustache tmpl("test {{/employees}}");
        CHECK_FALSE(tmpl.isValid());
        CHECK(tmpl.errorMessage() == "Unopened section \"employees\" at 5");
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
            CHECK_FALSE(tmpl.isValid());
            CHECK(tmpl.errorMessage() == "Invalid set delimiter tag at 5");
            ++total;
        }
        CHECK(total == invalids.size());
        CHECK(total == 7);
    }
    
    SECTION("lambda") {
        using data = mustache::data;
        mustache tmpl{"Hello {{lambda}}!"};
        data data("lambda", data{data::LambdaType{[](const std::string&){
            return "{{#what}}";
        }}});
        CHECK(tmpl.isValid() == true);
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render(data) == "Hello ");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"what\" at 0");
    }

    SECTION("lambda2") {
        using data = mustache::data;
        mustache tmpl{"Hello {{lambda}}!"};
        data data("lambda", data{data::LambdaType{[](const std::string&){
            return "{{what}}";
        }}});
        data["what"] = data{data::LambdaType{[](const std::string&){
            return "{{#blah}}";
        }}};
        CHECK(tmpl.isValid() == true);
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render(data) == "Hello ");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"blah\" at 0");
    }

    SECTION("partial") {
        using data = mustache::data;
        mustache tmpl{"Hello {{>partial}}!"};
        data data("partial", data{data::PartialType{[](){
            return "{{#what}}";
        }}});
        CHECK(tmpl.isValid() == true);
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render(data) == "Hello ");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"what\" at 0");
    }

    SECTION("partial2") {
        using data = mustache::data;
        mustache tmpl{"Hello {{>partial}}!"};
        data data("partial", {data::PartialType{[](){
            return "{{what}}";
        }}});
        data["what"] = data::LambdaType{[](const std::string&){
            return "{{#blah}}";
        }};
        CHECK(tmpl.isValid() == true);
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render(data) == "Hello ");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed section \"blah\" at 0");
    }
    
    SECTION("section_lambda") {
        using data = mustache::data;
        mustache tmpl{"{{#what}}asdf{{/what}}"};
        data data("what", data::LambdaType{[](const std::string&){
            return "{{blah";
        }});
        CHECK(tmpl.isValid() == true);
        CHECK(tmpl.errorMessage() == "");
        CHECK(tmpl.render(data) == "");
        CHECK(tmpl.isValid() == false);
        CHECK(tmpl.errorMessage() == "Unclosed tag at 0");
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
        data::PartialType partial = []() {
            return "Hello World";
        };
        data data("header", data{partial});
        CHECK(tmpl.render(data) == "Hello World");
    }

    SECTION("context") {
        mustache tmpl{"{{>header}}"};
        data::PartialType partial{[]() {
            return "Hello {{name}}";
        }};
        data data("header", data{partial});
        data["name"] = "Steve";
        CHECK(tmpl.render(data) == "Hello Steve");
    }
    
    SECTION("nested") {
        mustache tmpl{"{{>header}}"};
        data::PartialType header{[]() {
            return "Hello {{name}} {{>footer}}";
        }};
        data::PartialType footer{[]() {
            return "Goodbye {{#names}}{{.}}|{{/names}}";
        }};
        data names{data::List()};
        names.push_back("Jack");
        names.push_back("Jill");
        data data("header", header);
        data["name"] = "Steve";
        data["footer"] = data{footer};
        data["names"] = data{names};
        CHECK(tmpl.render(data) == "Hello Steve Goodbye Jack|Jill|");
    }

    SECTION("dotted") {
        mustache tmpl{"{{>a.b}}"};
        data::PartialType a_b{[]() {
            return "test";
        }};
        data data("a.b", a_b);
        CHECK(tmpl.render(data) == "test");
    }
}

TEST_CASE("lambdas") {
    
    SECTION("basic") {
        mustache tmpl{"{{lambda}}"};
        data data("lambda", data{data::LambdaType{[](const std::string&){
            return "Hello {{planet}}";
        }}});
        data["planet"] = "world";
        CHECK(tmpl.render(data) == "Hello world");
    }

    SECTION("delimiters") {
        mustache tmpl{"{{= | | =}}Hello, (|&lambda|)!"};
        data data("lambda", data{data::LambdaType{[](const std::string&){
            return "|planet| => {{planet}}";
        }}});
        data["planet"] = "world";
        CHECK(tmpl.render(data) == "Hello, (|planet| => world)!");
    }

    SECTION("nocaching") {
        mustache tmpl{"{{lambda}} == {{{lambda}}} == {{lambda}}"};
        int calls = 0;
        data data("lambda", data{data::LambdaType{[&calls](const std::string&){
            ++calls;
            return std::to_string(calls);
        }}});
        CHECK(tmpl.render(data) == "1 == 2 == 3");
    }

    SECTION("escape") {
        mustache tmpl{"<{{lambda}}{{{lambda}}}"};
        data data("lambda", data{data::LambdaType{[](const std::string&){
            return ">";
        }}});
        CHECK(tmpl.render(data) == "<&gt;>");
    }
    
    SECTION("section") {
        mustache tmpl{"<{{#lambda}}{{x}}{{/lambda}}>"};
        data data("lambda", data{data::LambdaType{[](const std::string& text){
            return text == "{{x}}" ? "yes" : "no";
        }}});
        CHECK(tmpl.render(data) == "<yes>");
    }

    SECTION("section_expansion") {
        mustache tmpl{"<{{#lambda}}-{{/lambda}}>"};
        data data("lambda", data{data::LambdaType{[](const std::string& text){
            return text + "{{planet}}" + text;
        }}});
        data["planet"] = "Earth";
        CHECK(tmpl.render(data) == "<-Earth->");
    }

    SECTION("section_alternate_delimiters") {
        mustache tmpl{"{{= | | =}}<|#lambda|-|/lambda|>"};
        data data("lambda", data{data::LambdaType{[](const std::string& text){
            return text + "{{planet}} => |planet|" + text;
        }}});
        data["planet"] = "Earth";
        CHECK(tmpl.render(data) == "<-{{planet}} => Earth->");
    }

    const data::LambdaType sectionLambda{[](const std::string& text){
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
        data data{"a", data::List()};
        CHECK(tmpl.render(data) == "\"\" == \"\"");
    }

    SECTION("broken_chains2") {
        mustache tmpl{"\"{{a.b.c.name}}\" == \"\""};
        data data;
        data["a"] = {"b", data::List()};
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
