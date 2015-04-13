//
//  main.cpp
//  Mustache
//
//  Created by Kevin Wojniak on 4/11/15.
//  Copyright (c) 2015 Kevin Wojniak. All rights reserved.
//

#include "mustache.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("variables") {

    SECTION("empty") {
        Mustache::Mustache<std::string> tmpl("");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data).empty());
    }

    SECTION("none") {
        Mustache::Mustache<std::string> tmpl("Hello");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "Hello");
    }

    SECTION("single_miss") {
        Mustache::Mustache<std::string> tmpl("Hello {{name}}");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "Hello ");
    }

    SECTION("single_exist") {
        Mustache::Mustache<std::string> tmpl("Hello {{name}}");
        Mustache::Data<std::string> data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }

    SECTION("escape") {
        Mustache::Mustache<std::string> tmpl("Hello {{name}}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello &quot;S&quot;&lt;br&gt;te&amp;v&apos;e");
    }
    
    SECTION("unescaped1") {
        Mustache::Mustache<std::string> tmpl("Hello {{{name}}}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2") {
        Mustache::Mustache<std::string> tmpl("Hello {{&name}}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }

    SECTION("unescaped2_spaces") {
        Mustache::Mustache<std::string> tmpl("Hello {{   &      name  }}");
        Mustache::Data<std::string> data;
        data.set("name", "\"S\"<br>te&v\'e");
        CHECK(tmpl.render(data) == "Hello \"S\"<br>te&v\'e");
    }
    
    SECTION("empty_name") {
        Mustache::Mustache<std::string> tmpl("Hello {{}}");
        Mustache::Data<std::string> data;
        data.set("", "Steve");
        CHECK(tmpl.render(data) == "Hello Steve");
    }
    
}

TEST_CASE("comments") {
    
    SECTION("simple") {
        Mustache::Mustache<std::string> tmpl("<h1>Today{{! ignore me }}.</h1>");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "<h1>Today.</h1>");
    }

    SECTION("newlines") {
        Mustache::Mustache<std::string> tmpl("Hello\n{{! ignore me }}\nWorld\n");
        Mustache::Data<std::string> data;
        CHECK(tmpl.render(data) == "Hello\n\nWorld\n");
    }

}

TEST_CASE("setdelimiter") {

    SECTION("basic") {
        Mustache::Mustache<std::string> tmpl("{{name}}{{=<% %>=}}<% name %><%={{ }}=%>{{ name }}");
        Mustache::Data<std::string> data;
        data.set("name", "Steve");
        CHECK(tmpl.render(data) == "SteveSteveSteve");
    }

    SECTION("small") {
        Mustache::Mustache<std::string> tmpl("{{n}}{{=a b=}}anba={{ }}=b{{n}}");
        Mustache::Data<std::string> data;
        data.set("n", "s");
        CHECK(tmpl.render(data) == "sss");
    }
    
    SECTION("noreset") {
        Mustache::Mustache<std::string> tmpl("{{=[ ]=}}[name] [x] + [y] = [sum]");
        Mustache::Data<std::string> data;
        data.set("name", "Steve");
        data.set("x", "1");
        data.set("y", "2");
        data.set("sum", "3");
        CHECK(tmpl.render(data) == "Steve 1 + 2 = 3");
    }

}

TEST_CASE("sections") {
    
    using Data = Mustache::Data<std::string>;

    SECTION("nonexistant") {
        Mustache::Mustache<std::string> tmpl("{{#var}}not shown{{/var}}");
        Data data;
        CHECK(tmpl.render(data) == "");
    }

    SECTION("false") {
        Mustache::Mustache<std::string> tmpl("{{#var}}not shown{{/var}}");
        Data data;
        data.set("var", Data(Data::Type::False));
        CHECK(tmpl.render(data) == "");
    }

    SECTION("emptylist") {
        Mustache::Mustache<std::string> tmpl("{{#var}}not shown{{/var}}");
        Data data;
        data.set("var", Data(Data::Type::List));
        CHECK(tmpl.render(data) == "");
    }

}

TEST_CASE("sections_inverted") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("nonexistant") {
        Mustache::Mustache<std::string> tmpl("{{^var}}shown{{/var}}");
        Data data;
        CHECK(tmpl.render(data) == "shown");
    }
    
    SECTION("false") {
        Mustache::Mustache<std::string> tmpl("{{^var}}shown{{/var}}");
        Data data;
        data.set("var", Data(Data::Type::False));
        CHECK(tmpl.render(data) == "shown");
    }
    
    SECTION("emptylist") {
        Mustache::Mustache<std::string> tmpl("{{^var}}shown{{/var}}");
        Data data;
        data.set("var", Data(Data::Type::List));
        CHECK(tmpl.render(data) == "shown");
    }
    
}

TEST_CASE("section_lists") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("list") {
        Mustache::Mustache<std::string> tmpl("{{#people}}Hello {{name}}, {{/people}}");
        Data people = Data::List();
        for (auto& name : {"Steve", "Bill", "Tim"}) {
            Data item;
            item.set("name", name);
            people.push_back(item);
        }
        Data data;
        data.set("people", people);
        CHECK(tmpl.render(data) == "Hello Steve, Hello Bill, Hello Tim, ");
    }
    
}

TEST_CASE("section_object") {
    
    using Data = Mustache::Data<std::string>;
    
    SECTION("basic") {
        Mustache::Mustache<std::string> tmpl("{{#employee}}name={{name}}, age={{age}}{{/employee}}");
        Data person;
        person.set("name", "Steve");
        person.set("age", "42");
        Data data;
        data.set("employee", person);
        CHECK(tmpl.render(data) == "name=Steve, age=42");
    }

    SECTION("basic_parent") {
        Mustache::Mustache<std::string> tmpl("({{subject}}) {{#employee}}name={{name}}, age={{age}} - {{subject}}{{/employee}}");
        Data person;
        person.set("name", "Steve");
        person.set("age", "42");
        person.set("subject", "email");
        Data data;
        data.set("subject", "test");
        data.set("employee", person);
        CHECK(tmpl.render(data) == "(test) name=Steve, age=42 - email");
    }

}
