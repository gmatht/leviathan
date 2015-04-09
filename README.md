# Leviathan
An implementation of a Tableau method for LTL formula satisfiability checking based on the paper "A new very simple, very fast tableau for LTL" by [Mark Reynolds](http://www.csse.uwa.edu.au/~mark/)

* Author: Matteo Bertello
* Email: bertello.matteo@gmail.com
* Version: 0.2.2

## License
Copyright (c) 2014, Matteo Bertello
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
* The names of its contributors may not be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MATTEO BERTELLO BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## How To
To compile the test application on Linux or Mac OS X run "./compile.sh".
The compile script can accept parameters to define the build type (Debug or Release, defaults to Release), the number of cores to use during compilation (defaults to number of cores available), and whether to use Clang or GCC (defaults to Clang).
Use "./compile.sh --help" to view  complete information about the script.
The binaries are created in the "bin" folder.
For every other platform/build system a CMake file is provided (Mac OS X and Linux supported).

## Note
Setting together the maximum model size AND the backtrack probability could potentially result in a false negative result.
The minimum/maximum backtrack value are currently unused.
