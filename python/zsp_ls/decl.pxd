
cimport debug_mgr.decl as dm
cimport libjson_rpc.decl as jrpc
cimport lls.decl as lls
cimport zsp_parser.decl as zspp

from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector
from libcpp.memory cimport unique_ptr
from libcpp cimport bool
from libc.stdint cimport int32_t
cimport cpython.ref as cpy_ref

ctypedef IFactory *IFactoryP;

cdef extern from "zsp/ls/IFactory.h" namespace "zsp::ls":
    cdef cppclass IFactory:
        void init(
            dm.IDebugMgr *,
            jrpc.IFactory *,
            lls.IFactory *,
            zspp.IFactory *)

        IThreadMain *mkThreadMain(int port)

cdef extern from "zsp/ls/IThreadMain.h" namespace "zsp::ls":
    cdef cppclass IThreadMain:
        bool start();
        bool join();
