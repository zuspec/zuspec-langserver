
import ctypes
cimport debug_mgr.core as dm_core
cimport libjson_rpc.core as jrpc
cimport lls.core as lls
cimport zsp_parser.core as zspp
cimport zsp_ls.decl as decl
from libcpp cimport bool
from libc.stdint cimport int32_t

cdef class Factory(object):
    cdef decl.IFactory      *_hndl

    cpdef void init(self,
        dm_core.DebugMgr dmgr,
        jrpc.Factory jrpc_f,
        lls.Factory lls_f,
        zspp.Factory zspp_f)

    cpdef ThreadMain mkThreadMain(self, int port)

cdef class ThreadMain(object):
    cdef decl.IThreadMain   *_hndl

    cpdef bool start(self)

    cpdef bool join(self)

