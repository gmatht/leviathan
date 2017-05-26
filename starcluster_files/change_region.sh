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
https://cloud-images.ubuntu.com/locator/ec2/
ap-northeast-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-1de1df7a	hvm
ap-northeast-2	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-6722ff09	hvm
ap-south-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-4fa4d920	hvm
ap-southeast-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-93ef68f0	hvm
ap-southeast-2	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-1e01147d	hvm
ca-central-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-e273cf86	hvm
cn-north-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170303	ami-a163b4cc	hvm
eu-central-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-a74c95c8	hvm
eu-west-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-6c101b0a	hvm
eu-west-2	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-056d7a61	hvm
sa-east-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-4bd8b727	hvm
us-east-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-20631a36	hvm
us-east-2	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-a5b196c0	hvm
us-gov-west-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170330	ami-ff22a79e	hvm
us-west-1	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-9fe6c7ff	hvm
us-west-2	xenial	16.04 LTS	amd64	hvm:ebs-ssd	20170516	ami-45224425	hvm

