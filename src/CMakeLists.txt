#****************************************************************************
#* CMakeLists.txt
#****************************************************************************
cmake_minimum_required (VERSION 3.11)



link_directories(
	${CMAKE_BINARY_DIR}/jrpc/lib
	${CMAKE_BINARY_DIR}/jrpc/lib64
	${CMAKE_BINARY_DIR}/lls/lib
	${CMAKE_BINARY_DIR}/lls/lib64
    "${debug_mgr_LIBDIR}"
    "${zuspec_parser_LIBDIR}"
    "${libjson_rpc_LIBDIR}"
    "${liblangserver_LIBDIR}"
	)

file(GLOB zsp_ls_SRC
	"*.h"
	"*.cpp")
add_library(zsp-ls SHARED
	${zsp_ls_SRC}
	)
    
target_include_directories(zsp-ls PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    "${PACKAGES_DIR}/libjson-rpc/src/include"
    "${PACKAGES_DIR}/liblangserver/src/include"
    "${debug_mgr_INCDIR}"
    "${zuspec_parser_INCDIR}"
    "${libjson_rpc_INCDIR}"
    "${liblangserver_INCDIR}"
    )
#target_link_libraries(zsp-ls
#    json-rpc
#    langserver
#    zsp-parser
#    debug-mgr
#    ast)

add_executable(zsp-langserver "zsp_ls_main.cc")
add_dependencies(zsp-langserver zsp-ls)
target_link_libraries(zsp-langserver 
    zsp-ls
    json-rpc
    langserver
    zsp-parser
    debug-mgr
    ast)
target_include_directories(zsp-langserver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    "${PACKAGES_DIR}/libjson-rpc/src/include"
    "${PACKAGES_DIR}/liblangserver/src/include"
    "${debug_mgr_INCDIR}"
    "${zuspec_parser_INCDIR}"
    "${libjson_rpc_INCDIR}"
    "${liblangserver_INCDIR}"
    )


install(TARGETS zsp-ls
    DESTINATION lib
    EXPORT zsp_ls-targets)
#add_dependencies(zsp-ls JRPC LLS)
    
set_target_properties(zsp-ls PROPERTIES SKIP_BUILD_RPATH TRUE)

