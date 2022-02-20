cmake_minimum_required(VERSION 3.14)

project(
    "nav"
    VERSION 0.0.0
    DESCRIPTION "NAV: Reflection on Enumeration Names And Values"
    HOMEPAGE_URL "https://github.com/codeinred/nav/"
    LANGUAGES CXX
)

add_library(nav INTERFACE)
add_library(nav::nav ALIAS nav)

# For ${CMAKE_INSTALL_<DIR>} variables that are standarized
include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_include_directories(
    nav
    INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Installation
# See: https://dominikberner.ch/cmake-interface-lib/

install(
    TARGETS nav
    EXPORT nav_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "navConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/navConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/navConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nav/cmake
)

install(
    EXPORT nav_Targets
    FILE navTargets.cmake
    NAMESPACE nav::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nav/cmake
)

install(FILES "${PROJECT_BINARY_DIR}/navConfig.cmake"
              "${PROJECT_BINARY_DIR}/navConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nav/cmake
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/nav
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)


include(helper)

if(PROJECT_IS_TOP_LEVEL)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)

    find_or_fetch(
        fmt
        https://github.com/fmtlib/fmt.git
        master)

    find_or_fetch(
        Catch2
        https://github.com/catchorg/catch2.git
        devel
        3.0.0)

    FetchContent_MakeAvailable(${remote_dependencies})

    add_source_dir(
        src # the name of the directory
        nav::nav # Libraries to link against
        fmt::fmt)



    # Register test_nav as an executable
    add_executable(
        test_nav
        tests/fuzzy_match.cpp
        tests/sparse_enums.cpp)

    target_compile_features(
        test_nav
        INTERFACE
        cxx_std_17
    )

    # Tell CMake that test_nav should link against nav (our library), fmt,
    # and Catch2. We link against Catch2WithMain b/c we're using the main
    # function provided by Catch2
    target_link_libraries(
        test_nav
        nav::nav
        fmt::fmt
        Catch2::Catch2WithMain)

    # Append places where Catch2 might be to the CMake Module path, so we can
    # use the function catch_discover_tests
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/lib/cmake/Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(test_nav)
endif()


