#!/bin/csh -f

set app=lljson
xcodebuild -list -project $app.xcodeproj

# rm -rf DerivedData/
xcodebuild -configuration Release -alltargets clean
xcodebuild -scheme $app -configuration Release build

echo -----------------------
find ./DerivedData -type f -name $app -perm +111 -ls
set src=./DerivedData/$app/Build/Products/Release/$app
set src=./DerivedData/Build/Products/Release/$app

echo -----------------------
echo "File $src"
ls -al $src
<<<<<<< HEAD:install-lljson.csh
cp $src ~/opt/bin/
=======
cp $src ~/opt/bin/
>>>>>>> 9f25bd9 (fix quote escaping):install.csh
