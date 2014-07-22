#! /bin/sh

if [ ! -d "build" ]; then
	mkdir "build"
fi

compiler="clang"
if [ ! -z "$1" ]; then
	compiler=$1
fi

cores=1
if [ "$(uname -s)" == "Darwin" ]; then
	cores=$(sysctl -a | grep machdep.cpu | grep thread_count)
else
	cores=$(grep -c ^processor /proc/cpuinfo) 
fi

cd "build"
if [ $compiler == "clang" ]; then
	cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
elif [ $compiler == "gcc" ]; then
	cmake -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ..
else
	echo "ERROR: Unsupported compiler $1"
	exit 1
fi
make -j $cores
cd ..
