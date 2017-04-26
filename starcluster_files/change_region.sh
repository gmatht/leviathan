region=$1
ami=$2

if [ -z "$2" ]
then 
	echo usage: $0 region ami
	firefox  https://simonpbriggs.co.uk/amazonec2/
	exit 1
fi

AWS_REGION_NAME=ap-southeast-1
NODE_IMAGE_ID=ami-8fcc75ec

NEW_HOME=~/.home.$region
mkdir -p $NEW_HOME

ln -s $NEW_HOME/.ssh ~/.ssh 
mkdir $NEW_HOME/.starcluster
< ~/.starcluster/config sed "s/$AWS_REGION_NAME/$region/g
s/$NODE_IMAGE_ID/$ami/g" > $NEW_HOME/.starcluster/config 

HOME=$NEW_HOME bash

exit
ca-central-1 ami-b3d965d7

