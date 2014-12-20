#! /bin/sh

compiler="clang"
build="Release"
j="0"
 
function usage()
{
echo "Usage:"
echo ""
echo -e "\t-h --help \t\t Show this message"
echo -e "\t-c --compiler \t\t Set the compiler toolchain. Supported values are \"clang\" and \"gcc\" (default is \"clang\")"
echo -e "\t-b --build \t\t Set the build type. Supported values are \"Release\" and \"Debug\" (default is \"Release\")"
echo -e "\t-j --cores \t\t Set the number of parallel builds (default is available number of cores)"
echo ""
}
 
while [ "$1" != "" ]; do
        PARAM=$1
        VALUE=$2
        case $PARAM in
                -h | --help)
                        usage
                        exit
                ;;
                -c | --compiler)
                        compiler=$VALUE
                        shift
                ;;
                -b | --build)
                        build=$VALUE
                        shift
                ;;
                -j | --cores)
                        j=$VALUE
                        shift
                ;;
                *)
                        echo "ERROR: unknown parameter \"$PARAM\""
                        usage
                        exit 1
                ;;
        esac
shift
done

cores="1"
if [ $j == "0" ]; then
        if [ "$(uname -s)" == "Darwin" ]; then
                threads=$(sysctl -a | grep machdep.cpu.thread_count)
                regex="machdep.cpu.thread_count: ([0-9]*)"
                [[ $threads =~ $regex ]]
                cores=${BASH_REMATCH[1]}
        else
                cores=$(grep -c ^processor /proc/cpuinfo) 
        fi
else
        cores=$j
fi

if [ ! -d "build" ]; then
        mkdir "build"
fi

cd "build"
if [ $compiler == "clang" ]; then
        cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_BUILD_TYPE=$build ..
elif [ $compiler == "gcc" ]; then
        cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_BUILD_TYPE=$build ..
else
        echo "ERROR: Unsupported compiler $1"
        exit 1
fi
make -j $cores
cd ..
