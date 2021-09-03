
#./piranha-objc.sh ObjectiveCFile.m flag_name optimistic

SOURCE=$1
FLAGNAME=$2
FLAGTYPE=$3

XcodeSDK="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" 
SYSTEM_FRAMEWORKS="System/Library/Frameworks"

PIRANHA_LIB="./piranha-objc/bin/XPFlagRefactoring.dylib"

./piranha-objc/bin/clang -x objective-c \
    $SOURCE \
    -isysroot $XcodeSDK \
    -arch armv7s \
    -ObjC \
    -iframeworkwithsysroot $SYSTEM_FRAMEWORKS \
    -F $SYSTEM_FRAMEWORKS \
    -framework "Foundation" \
    -framework "UIKit" \
    -Xclang -load \
    -Xclang $PIRANHA_LIB \
    -Xclang -plugin \
    -Xclang find-named-square\
    -Xclang \
    -fsyntax-only \
    -fmodules \
    -DDEBUG=1
