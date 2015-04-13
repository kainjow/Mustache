# Mustache

C++11 header-only [Mustache](http://mustache.github.io) templates with no dependencies.

## Example

    using namespace Mustache;
    Mustache<std::string> tmpl("Hello {{what}}!");
    Data<std::string> data;
    data.set("what", "World");
    std::cout << tmpl.render(data) << std::endl;

Result:

    Hello World!

## Build

To build and run tests:

    make

On OS X to generate and open an Xcode project:

    make xcode

## TODO

- Partials?
- Lambdas?
- Visual Studio makefile
- Test against [spec](https://github.com/mustache/spec)
