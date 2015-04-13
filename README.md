# Mustache

C++11 header-only [Mustache](http://mustache.github.io) templates with no dependencies.

## Example

    Mustache::Mustache<std::string> tmpl("Hello {{what}}!");
    Mustache::Data<std::string> data("what", "World");
    std::cout << tmpl.render(data) << std::endl;

Result:

    Hello World!

## Run Tests for Linux and OS X

To build and run tests:

    make

## TODO

- {{.}} syntax for lists of strings
- Partials?
- Lambdas?
- Visual Studio makefile
- Test against [spec](https://github.com/mustache/spec)
