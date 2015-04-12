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
