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
  * Remove the compareFunc lambda, to use the operator< overload instead
  * Remove shared_ptr in Formula AST by using a custom pool allocator
  * Investigate if it's worth treating Not Until Formula as a special type
  * Remove duplicated code, especially on rollback during PRUNE rule
  * Investigate if it's worth compressing some Frame attributes (like FormulaID and FrameID)

- *New Features*: Things to investigate/implement
  * Propositional SAT solver support through a common interface
  * Better crash handling, see:
    - http://oroboro.com/stack-trace-on-crash/
    - http://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
  * Let the user choose the application order of the tableaux rules
  * Investigate caching of common crossed subtrees

- *Testing and benchmarking*: To have scientifically valid and reproducible 
  results to present in the paper.
  * Setup the tool on StarExec
  * Eventually add other tools and other benchmark sets to StarExec
  * Setup a compact rendering of benchmark results
