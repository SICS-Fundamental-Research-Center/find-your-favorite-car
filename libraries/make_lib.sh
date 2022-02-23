#!/bin/bash

if [ ! -d glpk-4.65 ]; then
	curl -L http://ftp.gnu.org/gnu/glpk/glpk-4.65.tar.gz -O
	tar xf glpk-4.65.tar.gz
fi
cd glpk-4.65
make distclean

GLPK_DIR=$(pwd)

case $1 in
	g++)
		./configure
		make
		make prefix="${GLPK_DIR}" install
        cd ${GLPK_DIR}
		mkdir ../../lib
		cp ./lib/* ../../lib
		;;
	em++)
		emconfigure ./configure
		emmake make
		emmake make prefix="${GLPK_DIR}" install
        cd ${GLPK_DIR}
		mkdir ../../emlib
		cp ./lib/* ../../emlib
		;;
esac

