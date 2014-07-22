#! /bin/sh
if [ ! -d "build" ]; then
	mkdir "build"
fi

cd "build"
cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
make
cd ..
