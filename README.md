# Leviathan

[![Build Status](https://travis-ci.org/Corralx/leviathan.svg?branch=master)](https://travis-ci.org/Corralx/leviathan)
[![Build status](https://ci.appveyor.com/api/projects/status/d9pox240noqo6mo0/branch/master?svg=true)](https://ci.appveyor.com/project/Corralx/leviathan/branch/master)

## About

Leviathan is an implementation of a tableau method for [LTL](https://en.wikipedia.org/wiki/Linear_temporal_logic) satisfiability checking based on the paper "A new rule for a traditional tree-style LTL tableau" by [Mark Reynolds](http://www.csse.uwa.edu.au/~mark/research/Online/ltlsattab.html).

### Usage

Just call the tool passing the path to a file which contains the formulas to check. The parser is very flexible and supports every common LTL syntax used in other tools. Every line of the file is treated as an independent formula, so more than one formula at a time can be checked.

Moreover several command line options are present:

* **-l** or **--ltl** let the user specify the formula directly on the command line
* **-m** or **--model** generates and prints a model of the formula, if any
* **-p** or **--parsable** generates machine-parsable output
* **-v \<0-5>** or **--verbose \<0-5>** specifies the verbosity of the output
* **--maximum-depth** specifies the maximum depth of the tableau (and therefore the maximum size of the model)
* **--version** prints the current version of the tool
* **-h** or **--help** displays the usage message

### Compatibility

Leviathan has been written since the beginning with the portability in mind. The only prerequisite is a fairly recent C++11 compiler. It is known to work on Windows, Mac OS X and Linux and compiles correctly under MSVC 2015, GCC 4.8+ and Clang 3.5+.

## Future Work

* Refactor the code to enhance redability and simplify new features addition
* Remove the dependency on Boost by reimplementing a custom stack and allocator
* Rewrite the parser to remove the need of Flec++/Bisonc++ which are not easily available
* Let the user choose the order of application of the tableau rules
* Investigate the use of caching to quickly prune identical subtrees

See the [TODO](https://github.com/Corralx/leviathan/blob/master/TODO.md) file for a more complete recap of the work in progress!

## License

It is licensed under the very permissive [MIT License](https://opensource.org/licenses/MIT).
For the complete license see the provided [LICENSE](https://github.com/Corralx/leviathan/blob/master/LICENSE.md) file in the root directory.

## Thanks

Several open-source third-party libraries are currently used in this project:
* [Boost](http://www.boost.org/) for the dynamic bitset and pool allocator
* [Optional](https://github.com/akrzemi1/Optional) std::optional implementation for C++11
* [cppformat](https://github.com/fmtlib/fmt) to format the output
* [tclap](http://tclap.sourceforge.net/) to parse the command line arguments
* [flexc++](https://fbb-git.github.io/flexcpp/) and [bisonc++](https://fbb-git.github.io/bisoncpp/) as the parser generators

