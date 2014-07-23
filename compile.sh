#! /bin/sh

compiler="clang"
build="Release"
j="0"
size="32"
 
function usage()
{
echo "if this was a real script you would see something useful here"
echo ""
echo "./simple_args_parsing.sh"
echo "\t-h --help"
echo "\t--environment=$ENVIRONMENT"
echo "\t--db-path=$DB_PATH"
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
		-s | --size)
			size=$VALUE
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
		cores=$(sysctl -a | grep machdep.cpu | grep thread_count)
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
	cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ -DCMAKE_BUILD_TYPE=$build -DFORMULA_SIZE:STRING=$size ..
elif [ $compiler == "gcc" ]; then
	cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ -DCMAKE_BUILD_TYPE=$build -DFORMULA_SIZE:STRING=$size ..
else
	echo "ERROR: Unsupported compiler $1"
	exit 1
fi
make -j $cores
cd ..
