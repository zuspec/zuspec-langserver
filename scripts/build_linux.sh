#!/bin/bash -x

script_dir=$(dirname $(realpath $0))
proj_dir=$(dirname ${script_dir})

if test "x$CI_BUILD" != "x"; then
    yum update -y
    yum install -y glibc-static 
    yum install -y java-11-openjdk-devel uuid-devel libuuid-devel patchelf
fi

if test "x${IVPM_PYTHON}" = "x"; then
  export IVPM_PYTHON=python3
fi

version=0.0.1

if test "x$BUILD_NUM" != "x"; then
   version="${version}.${BUILD_NUM}"
fi

PYTHON=${proj_dir}/packages/python/bin/python

if test ! -d ${proj_dir}/packages; then
  #echo "BUILD_NUM=${BUILD_NUM}" >> python/zsp_parser/__build_num__.py
    ${IVPM_PYTHON} -m pip install ivpm cython
    ${IVPM_PYTHON} -m ivpm update -a
    ${PYTHON} -m pip install twine auditwheel ninja wheel cython
else
    ${PYTHON} ivpm build -g
fi

# First, do all the required code generation. This ensures the
# Python package can be imported during final package build
${PYTHON} setup.py build_ext --inplace

inst_prefix=${proj_dir}/zuspec-langserver-${version}
packages=${proj_dir}/packages

mkdir -p ${inst_prefix}
if test $? -ne 0; then exit 1; fi

mkdir -p ${inst_prefix}/bin ${inst_prefix}/lib
if test $? -ne 0; then exit 1; fi

cp $(pwd)/build/src/zsp-langserver ${inst_prefix}/bin
if test $? -ne 0; then exit 1; fi

cp $(pwd)/build/lib/libzsp-ls.so ${inst_prefix}/lib
if test $? -ne 0; then exit 1; fi

cp ${packages}/debug-mgr/build/lib/*.so ${inst_prefix}/lib
if test $? -ne 0; then exit 1; fi

cp ${packages}/libjson-rpc/build/lib/*.so ${inst_prefix}/lib
if test $? -ne 0; then exit 1; fi

cp ${packages}/liblangserver/build/lib/*.so ${inst_prefix}/lib
if test $? -ne 0; then exit 1; fi

cp ${packages}/zuspec-parser/build/lib/libast.so ${packages}/zuspec-parser/build/lib/libzsp-parser.so ${inst_prefix}/lib
if test $? -ne 0; then exit 1; fi

cp ${packages}/zuspec-parser/build/lib/libantlr4-runtime.so.* ${inst_prefix}/lib
if test $? -ne 0; then exit 1; fi

cwd=$(pwd)
cd ${inst_prefix}/lib
ln -s libantlr4-runtime.so.* libantlr4-runtime.so
if test $? -ne 0; then exit 1; fi

patchelf --set-rpath '$ORIGIN/../lib' ${inst_prefix}/bin/zsp-langserver

