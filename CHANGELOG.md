# Changelog

## 5.0 - pending

* Removed support for Visual Studio 2013. The library may still continue to function, but tests will no longer be ran on this compiler.

## 4.1 - April 18, 2020

* Fixed incorrect results when using lambda renderers

## 4.0 - October 28, 2019

* Lines with sections that result in an empty line are removed, per the Mustache spec.

## 3.2.1 - July 22, 2018

* Add an overload to render() that accepts a context and a stream (thanks Kitsune Ral)
* Added checks for empty objects (thanks Snafuuz)
* Refactored parser in preparation for future changes

## 3.2 - February 24, 2018

* Added ability to provide a custom escape function (thanks to Kitsune Ral)
* Allow `data.set()` to override an existing value

## 3.1 - July 22, 2017

* Added a new lambda type (innovatively called `lambda2`) that takes an additional render function. It will not render its result but allows the user to call the `render` argument to render the section text, or any other text.

## 3.0 - July 8, 2017

* Performance improvements - about 45% faster than version 2
* Even simpler API. Not backwards compatible but upgrading should be straightforward:
  * Namespace, classes, and methods are now in snake case to match the STL. For example, `Kainjow::Mustache` is now `kainjow::mustache`
  * Classes and aliases are now under a `mustache` namespace, instead of being under the mustache class
  * Removed `Data::List()` - use `data{data::type::list}` instead
  * Removed `Data::type()` - use the various `is_xxx` methods to identity the type

## 2.0 - June 11, 2016

* New simpler API (not backwards compatible)
* std::wstring support
* Bug fixes (thanks to Shen-Ta Hsieh)
* Automated tests on OS X
* 100% test coverage

## 1.0 - April 19, 2015

* All current Mustache features are implemented.
