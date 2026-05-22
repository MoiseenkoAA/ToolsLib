#!/bin/sh

git add *.h *.cpp *.nasm
git add archiver/*.h archiver/*.cpp
git add wwwcgi/*.h wwwcgi/*.cpp
git add unix/*.h unix/*.cpp
git add WIN32/*.h WIN32/*.cpp
git add *hpp gitadd.sh Licence.txt json.png
git add ToolsLib.vcxproj ToolsLib.vcxproj.filters ToolsLib.pro Makefile Makefile1

if [ x"$1" = "x" ]; then
    #echo 'git commit -m "Comment"'
    #echo 'git push origin master'
    git commit -m '~' && git push origin main
else
    git commit -m "$1" && git push origin main
fi
