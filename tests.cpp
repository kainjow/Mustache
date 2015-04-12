//
//  main.cpp
//  Mustache
//
//  Created by Kevin Wojniak on 4/11/15.
//  Copyright (c) 2015 Kevin Wojniak. All rights reserved.
//

#include "mustache.hpp"

int main() {
    
    //Mustache::Mustache<std::wstring> templw(L"");
    
    using Data = Mustache::Data<std::string>;
    Data data;
    data.set("name", "Kevin");
    data.set("dayOfWeek", "Monday");
    Data list(Data::Type::List);
    for (auto &name : {"Peter", "Paul", "Mary"}) {
        Data item;
        item.set("name", name);
        list.push_back(item);
    }
    data.set("names", list);
    data.set("html", "<b>\"Bold\"</b>");

    std::string input = "{{html}}{{{html}}}{{#names}}secret::{{/names}}Hello {{name}}! Today is {{dayOfWeek}}.";
    Mustache::Mustache<std::string> templ(input);
    if (!templ.isValid()) {
        std::cout << "ERROR: " << templ.errorMessage() << std::endl;
        return 0;
    }
    
    std::stringstream ss;
    templ.render(ss, data);
    std::cout << ss.str() << std::endl;
    
    return 0;
}
