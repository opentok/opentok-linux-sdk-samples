#!/bin/bash

unset CDPATH

SCRIPT_PATH=${SCRIPT_PATH:-$(cd `dirname ${BASH_SOURCE[0]}`; pwd)}

OPENTOK_LINUX_SDK_PACKAGE_FILENAME=opentok-ot-libotkit-ubuntu_x86_64-2.17.0-preview.0.tgz

if [ ! -d ${SCRIPT_PATH}/../assets/package ]; then

pushd ${SCRIPT_PATH}/../assets

unp ${OPENTOK_LINUX_SDK_PACKAGE_FILENAME}

popd

fi

SAMPLES=(Publisher-Only Basic-Video-Chat Basic-Video-Chat-With-Server)
for sample in ${SAMPLES[@]}; do
  rm -rf ${SCRIPT_PATH}/../${sample}/build
  mkdir ${SCRIPT_PATH}/../${sample}/build
  pushd ${SCRIPT_PATH}/../${sample}/build
  CC=clang CXX=clang++ cmake ..
  ret=$?
  if [ ${ret} -ne 0 ]; then
    echo "Cannot build ${sample} correctly. 'build.sh' script failed. Abort."
    popd
    exit 255
  fi
  make
  ret=$?
  if [ ${ret} -ne 0 ]; then
    echo "Cannot build ${sample} correctly. 'build.sh' script failed. Abort."
    popd
    exit 255
  fi
  popd
done

exit 0
