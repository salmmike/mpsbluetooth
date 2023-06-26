set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/include/)

set(LIBHEADERS
    ${INCDIR}/mpsbluetooth.h
)

set(LIBSOURCES
    ${CMAKE_CURRENT_LIST_DIR}/mpsbluetooth.c
)

include_directories(${INCDIR})
add_library(${PROJECT_NAME} STATIC ${LIBSOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)

find_path(GLIB_INCLUDE_DIR NAMES glib.h PATH_SUFFIXES glib-2.0)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${INCDIR}
                           PkgConfig::glib)

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC
                      PkgConfig::glib)

set(PATHNAME ${PROJECT_NAME})

target_compile_definitions(
    ${PROJECT_NAME} PUBLIC
)
