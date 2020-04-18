# About

* [Mustache](http://mustache.github.io) implementation for modern C++ (requires C++11)
* Header only
* Zero dependencies
* Templated string type for compatibility with any STL-like string (std::string, std::wstring, etc)
* Boost license

[![travis](https://travis-ci.org/kainjow/Mustache.svg?branch=master)](https://travis-ci.org/kainjow/Mustache) [![appveyor](https://ci.appveyor.com/api/projects/status/6uh5d5weajrffkyw?svg=true)](https://ci.appveyor.com/project/kainjow/mustache) [![codecov](https://codecov.io/gh/kainjow/Mustache/branch/master/graph/badge.svg)](https://codecov.io/gh/kainjow/Mustache)

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

## Run Tests

For *nix:

    make

For macOS:

    make mac

For Visual Studio 2013 (CMake 2.8+ required):

    build.bat

For Visual Studio 2015 (CMake 3.1+ required):

    build.bat 14

## Release Notes

#### 4.1 - April 18, 2020

* Fixed incorrect results when using lambda renderers

#### 4.0 - October 28, 2019

* Lines with sections that result in an empty line are removed, per the Mustache spec.

#### 3.2.1 - July 22, 2018

* Add an overload to render() that accepts a context and a stream (thanks Kitsune Ral)
* Added checks for empty objects (thanks Snafuuz)
* Refactored parser in preparation for future changes

#### 3.2 - February 24, 2018

* Added ability to provide a custom escape function (thanks to Kitsune Ral)
* Allow `data.set()` to override an existing value

#### 3.1 - July 22, 2017

* Added a new lambda type (innovatively called `lambda2`) that takes an additional render function. It will not render its result but allows the user to call the `render` argument to render the section text, or any other text.

#### 3.0 - July 8, 2017

* Performance improvements - about 45% faster than version 2
* Even simpler API. Not backwards compatible but upgrading should be straightforward:
  * Namespace, classes, and methods are now in snake case to match the STL. For example, `Kainjow::Mustache` is now `kainjow::mustache`
  * Classes and aliases are now under a `mustache` namespace, instead of being under the mustache class
  * Removed `Data::List()` - use `data{data::type::list}` instead
  * Removed `Data::type()` - use the various `is_xxx` methods to identity the type

#### 2.0 - June 11, 2016

* New simpler API (not backwards compatible)
* std::wstring support
* Bug fixes (thanks to Shen-Ta Hsieh)
* Automated tests on OS X
* 100% test coverage

#### 1.0 - April 19, 2015

* All current Mustache features are implemented.
