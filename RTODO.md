# TODO for the refactoring

- *Build system and shipping*: Things to do in order to let users (and future 
  paper reviewers) to build and use the tool in an easier way.
  * Rewrite CMake file to use C++ feature detection to automatically reject 
    unsupported compilers
  * Use [this script](https://github.com/nemequ/configure-cmake) to give the 
    user a familiar way to compile the project (this also makes it easier to 
    build deb/rpm packages and to configure Travis).
  * Try to use a different pool allocator in order to avoid having a branched
    version of Boost in-tree
  * Make use of CMake's generator expressions instead of if/then/else constructs 
    to conditionally set compilation options, in order to be compatible with 
    XCode and VS generators (see CMake's docs)
  * CPU feature detection to switch on the right compiler's optimizations
  * Use git submodules to include TCLAP and easylogging++, and require them with 
    CMake 
  * Maybe use TCLAP as an external dependency
  * Ship packages for Ubuntu/Fedora (and HomeBrew formulas for OS X?)
  * Ship package for Windows (compiled with MinGW is enough?)
  
- *Code refactoring*: easier maintenance and easier comprehension of the code
  * Small hand-written parser (eliminate the need of Bison++)
  * A lot of things already listed in TODO.md
  * Code formatting
  * Factor out the SAT solver support to be able to integrate different solvers
    instead of being tied to minisat

- *Testing and benchmarking*: To have scientifically valid and reproducible 
  results to present in the paper.
  * Setup continuous integration on Travis CI
  * Figure out which test suite of LTL formulas to use
  * Automatize the benchmarking process and generation of useful graphics
    from collected data
