# Remove Xcode temporary build files
xcodebuild -configuration Release -alltargets clean
find . -type d -name DerivedData -prune -exec rm -rf {} \;

# Remove visual studio temporary build files
find . -type d -name x64 -prune -exec rm -rf {} \;

