#/bin/bash

dir=$(cd "$(dirname "$0")"; pwd)

major=1
minor=0

build=$(sed -n '1p' $dir/build.txt)
build=$((build+1))

version=$major.$minor.$build

echo "#define APPVERSION \"$version\"" > $dir/version.h

echo $build > $dir/build.txt
echo $version > $dir/version.txt

echo VERSION = $version
