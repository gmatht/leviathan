# TODO for the refactoring

- *Build system and shipping*: Things to do in order to let users (and future 
  paper reviewers) to build and use the tool in an easier way.
  * Try to use a different pool allocator in order to avoid having a branched
    version of Boost in-tree
  * CPU feature detection to switch on the right compiler's optimizations
  * Ship packages for Ubuntu/Fedora (and HomeBrew formulas for OS X?)
  * Ship package for Windows
  
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
