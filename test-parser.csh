#!/bin/tcsh

set lljson=./DerivedData/Build/Products/Release/lljson

echo "input file=$1"
$lljson -verbose $1 >! foo.json

ls -al $1 foo.json
jd $1 foo.json

jq --sort-keys "." foo.json >!  foo1.json
jq --sort-keys "." $1       >!  foo2.json
meld foo1.json foo2.json
   
   