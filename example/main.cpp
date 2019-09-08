#include <iostream>

#include "kainjow/mustache.hpp"

using namespace kainjow::mustache;

int main(/*int argc, char *argv[]*/) {
    mustache tmpl("Hello {{name}}");
    data data;
    data.set("name", "Steve");
    std::cout << tmpl.render(data) << std::endl; // should print "Hello Steve"
    return 0;
}