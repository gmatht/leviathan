#Everything required to install this on Ubuntu 16.04... hopefully.
sudo apt install cmake libboost-dev libboost-system-dev libboost-thread-dev  
./configure  
make -j`cat /proc/cpuinfo | grep processor | wc -l`
# sudo apt-get install build-essential libssl-dev libffi-dev python-dev
# sudo easy_install starcluster 
