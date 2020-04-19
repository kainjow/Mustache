# About

* [Mustache](http://mustache.github.io) implementation for modern C++ (requires C++11)
* Header only
* Zero dependencies
* Templated string type for compatibility with any STL-like string (std::string, std::wstring, etc)
* Boost license

[![travis](https://travis-ci.org/kainjow/Mustache.svg?branch=master)](https://travis-ci.org/kainjow/Mustache) [![appveyor](https://ci.appveyor.com/api/projects/status/6uh5d5weajrffkyw?svg=true)](https://ci.appveyor.com/project/kainjow/mustache) [![ghe](https://github.com/kainjow/Mustache/workflows/Tests/badge.svg)](https://github.com/kainjow/Mustache/actions) [![codecov](https://codecov.io/gh/kainjow/Mustache/branch/master/graph/badge.svg)](https://codecov.io/gh/kainjow/Mustache)

## Example usage

All examples assume `using namespace kainjow::mustache`. Additional examples and usage can be found in the `tests.cpp` file.

### Example 1 - Hello World

````cpp
mustache tmpl{"Hello {{what}}!"};
std::cout << tmpl.render({"what", "World"}) << std::endl;
// Hello World!
````

### Example 2 - Lists

````cpp
mustache tmpl{"{{#employees}}{{name}}, {{/employees}}"};
data employees{data::type::list};
employees << data{"name", "Steve"} << data{"name", "Bill"};
tmpl.render({"employees", employees}, std::cout);
// Steve, Bill,
````

### Example 3 - Custom Render Handler

````cpp
mustache tmpl("Hello {{what}}!");
std::stringstream ss;
tmpl.render({"what", "World"}, [&ss](const std::string& str) {
    ss << str;
});
// ss.str() == "Hello World!"
````

## Supported Features

This library supports all current Mustache features:

- Variables
- HTML escaping
- Sections
- Inverted Sections
- True/False
- Lists
- Lambdas
- Partials
- Comments
- Set Delimiter

Additional features:

- Custom escape function for use outside of HTML
