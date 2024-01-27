
import os
import ctypes
from libcpp.vector cimport vector as cpp_vector
from libc.stdint cimport intptr_t
cimport debug_mgr.core as dm_core
cimport libjson_rpc.core as jrpc
cimport lls.core as lls
cimport zsp_parser.core as zspp
cimport zsp_ls.decl as zsp_ls_decl

cdef class Factory(object):

    def __init__(self):
        pass

    cpdef void init(self,
        dm_core.DebugMgr dmgr,
        jrpc.Factory jrpc_f,
        lls.Factory lls_f,
        zspp.Factory zspp_f):
        self._hndl.init(
            dmgr._hndl,
            jrpc_f._hndl,
            lls_f._hndl,
            zspp_f._hndl)

    cpdef ThreadMain mkThreadMain(self, int port):
        cdef decl.IThreadMain *threadmain_h = NULL

        threadmain_h = self._hndl.mkThreadMain(port)
        ret = ThreadMain()
        ret._hndl = threadmain_h

        return ret

    @staticmethod
    def inst():
        cdef zsp_ls_decl.IFactory *hndl = NULL
        cdef Factory factory
        global _inst

        if _inst is None:
            ext_dir = os.path.dirname(os.path.abspath(__file__))

            build_dir = os.path.abspath(os.path.join(ext_dir, "../../build"))

            # First, look in the build directory
            core_lib = None

            # TODO: check already-loaded libraries for libdebug-mgr symbols?
            libname = "libzsp-ls.so"
            # for libdir in ("lib", "lib64"):
            #     if os.path.isfile(os.path.join(build_dir, libdir, libname)):
            #         core_lib = os.path.join(build_dir, libdir, libname)
            #         break
            if core_lib is None:
                core_lib = os.path.join(ext_dir, libname)

            if not os.path.isfile(core_lib):
                raise Exception("Extension library core \"%s\" doesn't exist" % core_lib)

            print("LoadLibrary: %s" % core_lib)
            so = ctypes.cdll.LoadLibrary(core_lib)

            func = so.zsp_ls_getFactory
            func.restype = ctypes.c_void_p

            hndl_v = func()
            hndl = <zsp_ls_decl.IFactoryP>(<intptr_t>(hndl_v))

            factory = Factory()
            factory._hndl = hndl
            factory.init(
                dm_core.Factory.inst().getDebugMgr(),
                jrpc.Factory.inst(),
                lls.Factory.inst(),
                zspp.Factory.inst())
            _inst = factory

        return _inst

_inst = None

cdef class ThreadMain(object):

    cpdef bool start(self):
        return self._hndl.start()