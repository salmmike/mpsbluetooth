find_package(PkgConfig REQUIRED)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0 gio-2.0)

set(MANUALTEST manualtest)

set(SOURCES
    main.c)

add_executable(${MANUALTEST} ${SOURCES})
target_link_libraries(${MANUALTEST}
                      PRIVATE
                      ${PROJECT_NAME}
                      )

target_link_libraries(${MANUALTEST} PRIVATE ${PROJECT_NAME} PkgConfig::glib)
