#!/bin/tcsh

# set prog=./DerivedData/Build/Products/Release/lljson
set proj=lljson

echo "input file=$1"
$prog -verbose $1 >! /tmp/wxnew.json

ls -al $1 /tmp/wxnew.json
jd $1 /tmp/wxnew.json

jq --sort-keys "." /tmp/wxnew.json >!  /tmp/wxNewSort.json
jq --sort-keys "." $1              >!  /tmp/wxOrgSort.json
diff /tmp/wxNewSort.json /tmp/wxOrgSort.json
