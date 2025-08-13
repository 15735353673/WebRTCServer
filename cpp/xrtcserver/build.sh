##########################################################################
# File Name: build.sh
# Author: lingye
# mail: 15735353673@163.com
# Created Time: Wed 02 Jul 2025 08:37:19 PM CST
#########################################################################
#!/bin/zsh

cd out && cmake ../

if test $# -gt 0 && test $1 = "clean"
then
	echo "make clean"
	make clean
else
	echo "make"
	make
fi



