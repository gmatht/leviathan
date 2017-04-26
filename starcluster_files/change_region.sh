region=$1
ami=$2

if [ -z "$2" ]
then 
	echo usage: $0 region ami
	echo $0 ca-central-1 ami-b3d965d7
	firefox  https://simonpbriggs.co.uk/amazonec2/
	exit 1
fi

AWS_REGION_NAME=ap-southeast-1
NODE_IMAGE_ID=ami-8fcc75ec

NEW_HOME=~/.home.$region
mkdir -p $NEW_HOME

ln -s ~/.ssh $NEW_HOME/.ssh 
mkdir $NEW_HOME/.starcluster

< ~/.starcluster/config sed "s/$AWS_REGION_NAME/$region/g
s/$NODE_IMAGE_ID/$ami/g" > $NEW_HOME/.starcluster/config 

ln ~/.gitconfig $NEW_HOME/.gitconfig

#[ -e ~/.ssh/mykey-$region.rsa ] || HOME=$NEW_HOME starcluster ck $region -o ~/.ssh/mykey-$region.rsa
[ -e ~/.ssh/mykey-$region.rsa ] || HOME=$NEW_HOME starcluster ck mykey -o ~/.ssh/mykey-$region.rsa
HOME=$NEW_HOME bash

exit
ca-central-1 ami-b3d965d7

