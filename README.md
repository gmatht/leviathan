# Parallel Leviathan

# About Parallel Leviathan

This modification allows Leviathan to divide work between multiple independant jobs that can be run in parallel. This was be [presented](http://staffhome.ecm.uwa.edu.au/~00061811/GandALF2017a) at [GandALF 2017](http://eptcs.web.cse.unsw.edu.au/paper.cgi?GANDALF2017:10.pdf). The raw benchmark data [(705MB)](http://staffhome.ecm.uwa.edu.au/~00061811/parallel_benchdata.tar.gz), used in this paper is available online, including the comparison with PolSAT [(26MB)](http://staffhome.ecm.uwa.edu.au/~00061811/polsat_benchdata.tar.gz). There is some [Documentation](https://github.com/gmatht/leviathan/blob/master/samples/benchmark_data_DOC.txt) for the benchmark data.

The usage of Parallel Leviathan is similar to the original, but you can specify a `JOB_NO` in the format `job-number/number-of-jobs@split-depth`. See the GandALF paper for a discription of split depth, 20 is a reasonable default. The original formula is satisfiable iff any of the JOBs report "IsSat". For example:


    $ JOB_NO=1/2@1 ../bin/checker -l '(p | (p & ~p))'
    Solving formula n° 1: (p) ∨ ((p) ∧ (¬(p)))
    JOB_NO=1/2@1 WIDTH 1:1 2:1 
    ^0:1652
    IsSat! JOB=1/2@1 SEC=0.001663

    $ JOB_NO=2/2@1 ../bin/checker -l '(p | (p & ~p))'
    Solving formula n° 1: (p) ∨ ((p) ∧ (¬(p)))
    JOB_NO=2/2@1 WIDTH 1:1 
    Unsat! JOB=2/2@1 SEC=0.001453
    The formula is unsatisfiable!

## About (Original) Leviathan

Leviathan is an implementation of a tableau method for [LTL](https://en.wikipedia.org/wiki/Linear_temporal_logic) satisfiability checking based on the paper "A new rule for a traditional tree-style LTL tableau" by [Mark Reynolds](http://www.csse.uwa.edu.au/~mark/research/Online/ltlsattab.html).

This work has been accepted for publication at [IJCAI-16](http://ijcai-16.org/index.php/welcome/view/home) and a preprint can be found [here](http://corralx.me/public/leviathan_preprint.pdf).

### Usage

Just call the tool passing the path to a file which contains the formulas to check. The parser is very flexible and supports every common LTL syntax used in other tools. Every line of the file is treated as an independent formula, so more than one formula at a time can be checked.

Moreover several command line options are present:

* **-l** or **--ltl** let the user specify the formula directly on the command line
* **-m** or **--model** generates and prints a model of the formula, if any
* **-p** or **--parsable** generates machine-parsable output
* **--maximum-depth** specifies the maximum depth of the tableau (and therefore the maximum size of the model)
* **-v \<0-5>** or **--verbose \<0-5>** specifies the verbosity of the output
* **--version** prints the current version of the tool
* **-h** or **--help** displays the usage message

### Usage example

The following sample:
> ./checker.exe --ltl "G (req => X grant) & req" --model

Outputs the following answer:
> The formula is satisfiable!<br>
> The following model was found:<br>
> State 0:<br>
> req<br>
> State 1:<br>
> grant, !req<br>
> State 2:<br>
> !req<br>
> Loops to state 2

While with the addition of the parsable flag the output is the following:
> SAT;{req} -> {grant,!req} -> {!req} -> #2

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

