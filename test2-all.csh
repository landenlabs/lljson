#!/bin/tcsh  


set lljson=./DerivedData/Build/Products/Release/lljson
set lljson=lljson

foreach testFile (test2/*.json)
    echo $testFile
    ($lljson -verbose ${testFile} >! foo.json) >& /dev/null
    jd ${testFile} foo.json >& /dev/null
    set result=$status
    if ($result != 0) then
        echo == Failed, jd status=$result File=${testFile}
    endif
end
