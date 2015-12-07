# TODO for the refactoring

- *Build system and shipping*: Things to do in order to let users (and future 
  paper reviewers) to build and use the tool in an easier way.
  * Try to use a different pool allocator in order to avoid having a branched
    version of Boost in-tree
  * Make use of CMake's generator expressions instead of if/then/else constructs 
    to conditionally set compilation options, in order to be compatible with 
    XCode and VS generators (see CMake's docs)
  * CPU feature detection to switch on the right compiler's optimizations
  * Ship packages for Ubuntu/Fedora (and HomeBrew formulas for OS X?)
  * Ship package for Windows (compiled with MinGW is enough?)
  
- *Code refactoring*: easier maintenance and easier comprehension of the code
  * Small hand-written parser (eliminate the need of Bison++)
  * A lot of things already listed in TODO.md
  * Factor out the SAT solver support to be able to integrate different solvers
    instead of being tied to minisat

- *Testing and benchmarking*: To have scientifically valid and reproducible 
  results to present in the paper.
  * Setup the tool on StarExec
  * Eventually add other tools and other benchmark sets to StarExec
  * Setup a compact rendering of benchmark results

- *Operating Systems and Compilers support*:
  * We have to support at least the last two Ubuntu LTS releases and the 
    latest version at the time of the release:
    - Test G++ 4.8/4.9/5               on Ubuntu 12.04
    - Test G++ 4.8/4.9/5 and Clang 3.5 on Ubuntu 14.04
    - Test G++ 5.2       and Clang 3.6 on Ubuntu 15.04
      (Cannot do it in travis, though)
    - Test Xcode toolchain (Apple Clang) on OS X
