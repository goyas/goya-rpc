#!/bin/bash

BASE=$(dirname `readlink -f $0`)
cd $BASE
#echo $BASE

DEPS=$BASE/thirdparty
DEPS_SOURCE=$DEPS
DEPS_PREFIX=$DEPS/install
export PATH=${DEPS_PREFIX}/bin:$PATH
mkdir -p ${DEPS_PREFIX} ${DEPS_SOURCE}

# 1 protobuf
pushd $DEPS
if [ ! -f "${DEPS_PREFIX}/lib/libprotobuf.a" ] \
  || [ ! -d "${DEPS_PREFIX}/include/google/protobuf" ]; then
  rm -rf protobuf-2.6.1
  wget https://github.com/protocolbuffers/protobuf/archive/v2.6.1.tar.gz
  tar zxvf v2.6.1.tar.gz
  cd protobuf-2.6.1
  autoreconf -ivf
  ./configure --prefix=${DEPS_PREFIX}/
  make -j4 && make install
  cd -
  rm v2.6.1.tar.gz
fi
popd

# 2 boost
pushd $DEPS
if [ ! -f "${DEPS_PREFIX}/lib/libboost_system.a" ] \
  || [ ! -d "${DEPS_PREFIX}/include/boost" ]; then
  rm -rf boost_1_57_0
  wget https://raw.githubusercontent.com/lylei9/boost_1_57_0/master/boost_1_57_0.tar.gz
  tar zxvf boost_1_57_0.tar.gz
  cd boost_1_57_0
  ./bootstrap.sh --with-libraries=all --with-toolset=gcc
  ./b2 toolset=gcc
  ./b2 install --prefix=${DEPS_PREFIX}/
  sudo ldconfig
  #rm -rf ${DEPS_PREFIX}/boost_1_57_0
  #mv boost_1_57_0 ${DEPS_PREFIX}/boost_1_57_0
  cd -
  rm boost_1_57_0.tar.gz
fi
popd

if [ ! -d build ]; then
  mkdir -p build
fi

pushd $BASE/build
cmake .. && make && make install
cp -rf $BASE/include $BASE/build 


