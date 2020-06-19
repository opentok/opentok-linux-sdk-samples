#!/bin/bash

unset CDPATH

SCRIPT_PATH=${SCRIPT_PATH:-$(cd `dirname ${BASH_SOURCE[0]}`; pwd)}

SAMPLES=(Basic-Video-Chat Publisher-Only Signaling Custom-Audio-Device Custom-Video-Capturer)
for sample in ${SAMPLES[@]}; do
  rm -rf ${SCRIPT_PATH}/../${sample}/build
  mkdir ${SCRIPT_PATH}/../${sample}/build
  pushd ${SCRIPT_PATH}/../${sample}/build
  CC=clang CXX=clang++ cmake -DCI=ON ..
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
