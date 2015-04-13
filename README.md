# Mustache

C++11 header-only [Mustache](http://mustache.github.io) templates with no dependencies.

## Example

    Mustache::Mustache<std::string> tmpl("Hello {{what}}!");
    Mustache::Data<std::string> data("what", "World");
    std::cout << tmpl.render(data) << std::endl;
    // Hello World!

## Run Tests for Linux and OS X

To build and run tests:

    make

## Run Tests for Visual Studio 2013 (CMake 2.8+ required)

Open a Visual Studios Command Prompt, then:

    cmake .
    cmake --build
    Debug\mustache.exe

## TODO

- {{.}} syntax for lists of strings
- Partials?
- Lambdas?
- Visual Studio makefile
- Test against [spec](https://github.com/mustache/spec)
