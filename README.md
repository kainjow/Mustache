# Mustache

C++11 header-only [Mustache](http://mustache.github.io) templates with no dependencies.

## Example 1

    Mustache::Mustache<std::string> tmpl("Hello {{what}}!");
    Mustache::Data<std::string> data("what", "World");
    std::cout << tmpl.render(data) << std::endl;
    // Hello World!
    
## Example 2

    Mustache::Mustache<std::string> tmpl("{{#employees}}{{name}}, {{/employees}}");
    Mustache::Data<std::string> data;
    Data employees(Data::List());
    employees.push_back(Data("name", "Steve"));
    employees.push_back(Data("name", "Bill"));
    data.set("employees", employees);
    tmpl.render(std::cout, data);
    // Steve, Bill, 

## Compilers Tested

- Xcode 6.3 (Clang 3.6)
- Xcode 4.6 (Clang 3.2)
- GCC 4.9
- GCC 4.8
- Clang 3.4
- Visual Studio 2013

## Run Tests

For Linux and OS X:

    make

Older versions of OS X may need to build with `make tests_mac`

For Visual Studio 2013 (CMake 2.8+ required) open a Visual Studio Command Prompt, then:

    cmake .
    cmake --build .
    Debug\mustache.exe

## TODO

- {{.}} syntax for lists of strings
- Partials?
- Lambdas?
- Visual Studio makefile
- Test against [spec](https://github.com/mustache/spec)
