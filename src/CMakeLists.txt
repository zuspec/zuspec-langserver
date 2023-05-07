#****************************************************************************
#* CMakeLists.txt
#****************************************************************************
cmake_minimum_required (VERSION 3.11)



link_directories(
	${CMAKE_BINARY_DIR}/jrpc/lib
	${CMAKE_BINARY_DIR}/jrpc/lib64
	${CMAKE_BINARY_DIR}/lls/lib
	${CMAKE_BINARY_DIR}/lls/lib64
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
    "${debug_mgr_INCDIR}"
    )

install(TARGETS zsp-ls
    DESTINATION lib
    EXPORT zsp_ls-targets)
add_dependencies(zsp-ls JRPC LLS)
    
set_target_properties(zsp-ls PROPERTIES SKIP_BUILD_RPATH TRUE)

