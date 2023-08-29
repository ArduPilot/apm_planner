#!/bin/sh
echo "cccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccloolccccccccccccccccccc
cccccccccccccccccccccclollccldxdolcccccccccccccccc
ccccccccccccccccccccccodkkO0OkO00kdolccccccccccccc
ccccccccccccccccccccccccllox0XWWWWNX0kdlcccccccccc
cccccccccccccccccccccccccccccoxOKWMMMWNKkdlccccccc
ccccccccccclllllccccccclllllllllldkKNWMMMWX0xolccc
ccccccccccxKXXX0dcccccdKXXXXXXXX0kolokKNWMMMWXxccc
cccccccclkNMMMMNxccccl0WMMWX0KNMMW0lcclOWMMMNOlccc
cccccccoOWMNNWMWkccccxNMMMXdcoKMMWOlccdKMMWXxlcccc
ccccccdKWMXxOWMWOlccoKMMMMNOOKWMN0ocldKWMWKdcccccc
cccclxXWMWNKNWMM0lcckNMMNXXKKK0kdllld0WMNOoccccccc
ccclONMWXOOOKWMMKocoKMMWOolllccccldx0NMXklcccccccc
cccdO00OocccoO00kocdO00koccccccodxdONWKdcccccccccc
cccccccccccccccccccccccccccccldxdoxKN0occccccccccc
cccccccccccccccccccccccccccldddlco0Xklcccccccccccc
ccccccccccccccccccccccccloodolcclOKxcccccccccccccc
cccccccccccccccccccccccllllcccclxOoccccccccccccccc
cccccccccccccccccccccccccccccccodlcccccccccccccccc
cccccccccccccccccccccccccccccccllccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccc
cccccccccccccccccccccccccccccccccccccccccccccccccc
"
if [ -f "release/apmplanner2.dmg" ] ; then
    rm -rf "release/apmplanner2.dmg"
fi

rm -rf release/apmplanner2.app 
make -j8
echo 'macdeployqt "release/apmplanner2.app"'
macdeployqt "release/apmplanner2.app" 

echo "rm -rf release/apmplanner2.app/Contents/Frameworks/SDL2.framework"
rm -rf release/apmplanner2.app/Contents/Frameworks/SDL2.framework
echo  "cp -R libs/lib/Frameworks/SDL2.framework release/apmplanner2.app/Contents/Frameworks"
cp -R libs/lib/Frameworks/SDL2.framework release/apmplanner2.app/Contents/Frameworks 

SIGN="$1"

ROOT=`pwd`
cd release/

echo "cd apmplanner2.app/Contents/MacOS"
cd apmplanner2.app/Contents/MacOS
echo "mv qml ../Resources"
mv qml ../Resources  
echo "mv sik_uploader  ../Resources "
mv sik_uploader  ../Resources 
echo "ln -s ../Resources/sik_uploader  sik_uploader"
ln -s ../Resources/sik_uploader  sik_uploader
echo "ln -s ../Resources/qml qml"
ln -s ../Resources/qml qml

cd ..
cd ..
cd ..


echo "Sign sub Framework"
find apmplanner2.app -name "*.framework" -exec codesign  --timestamp --options runtime -f -s "$SIGN" {} \; 
echo "Sign sub dylib"
find apmplanner2.app -name "*.dylib" -exec codesign  --timestamp --options runtime -f -s "$SIGN" {} \; 
echo "Sign apmplanner2.ap"
codesign  --timestamp --options runtime -f -s "$SIGN"  apmplanner2.app

cd $ROOT
appdmg deploy/assets/config.json  release/apmplanner2.dmg
