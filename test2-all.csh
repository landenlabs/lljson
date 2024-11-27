#!/bin/tcsh  


set lljson=./DerivedData/Build/Products/Release/lljson
set lljson=lljson

<<<<<<< HEAD
foreach testFile (test2/*.json)
    echo $testFile
=======
foreach testFile (test2/*.json) 
>>>>>>> 9f25bd9 (fix quote escaping)
    ($lljson -verbose ${testFile} >! foo.json) >& /dev/null
    jd ${testFile} foo.json >& /dev/null
    set result=$status
    if ($result != 0) then
<<<<<<< HEAD
        echo == Failed, jd status=$result File=${testFile}
=======
        echo == $result ${testFile}
>>>>>>> 9f25bd9 (fix quote escaping)
    endif
end
