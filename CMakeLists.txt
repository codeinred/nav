cmake_minimum_required(VERSION 3.14)

project(
    "nav"
    VERSION 0.0.0
    DESCRIPTION "NAV: Reflection on Enumeration Names And Values"
    HOMEPAGE_URL "https://github.com/codeinred/nav/"
    LANGUAGES CXX)

add_library(nav INTERFACE)
add_library(
    nav::nav
    ALIAS
    nav)

# For ${CMAKE_INSTALL_<DIR>} variables that are standarized
include(GNUInstallDirs)

list(
    APPEND
    CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_include_directories(
    nav INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Installation
# See: https://dominikberner.ch/cmake-interface-lib/

install(
    TARGETS nav
    EXPORT nav_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "navConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/navConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/navConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nav/cmake)

install(
    EXPORT nav_Targets
    FILE navTargets.cmake
    NAMESPACE nav::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nav/cmake)

install(FILES "${PROJECT_BINARY_DIR}/navConfig.cmake"
              "${PROJECT_BINARY_DIR}/navConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nav/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/nav
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(helper)

option(
    BUILD_TESTING
    "Build Tests for Nav"
    ON)
option(
    BUILD_BENCHMARKS
    "Build Tests for Nav"
    ON)

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
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
endif()

if(PROJECT_IS_TOP_LEVEL AND BUILD_BENCHMARKS)
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
    find_or_fetch(
        benchmark
        https://github.com/google/benchmark.git
        main
        1.6)
endif()

FetchContent_MakeAvailable(${remote_dependencies})

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    set(test_libs
        nav::nav
        fmt::fmt
        Catch2::Catch2WithMain)
    set(test_sources tests/nav_core.cpp tests/nav_lowercase.cpp)

    # Register test_nav as an executable
    add_executable(nav_core ${test_sources})
    add_executable(nav_core_null_terminated ${test_sources})

    target_compile_features(nav_core PRIVATE cxx_std_17)
    target_compile_features(nav_core_null_terminated PRIVATE cxx_std_17)

    target_compile_definitions(nav_core_null_terminated
                               PRIVATE "NAV_ADD_NULL_TERMINATORS=1")

    target_link_libraries(nav_core ${test_libs})
    target_link_libraries(nav_core_null_terminated ${test_libs})

    list(
        APPEND
        CMAKE_MODULE_PATH
        ${CMAKE_INSTALL_PREFIX}/lib/cmake/Catch2
        ${catch2_SOURCE_DIR}/extras)
    include(CTest)
    include(Catch)
    catch_discover_tests(nav_core)
    catch_discover_tests(nav_core_null_terminated)
endif()

if(PROJECT_IS_TOP_LEVEL AND BUILD_BENCHMARKS)
    set(bench_libs
        nav::nav
        fmt::fmt
        benchmark::benchmark_main)
    set(bench_sources bench/nav_hashing.cpp)

    add_executable(nav_bench ${bench_sources})
    add_executable(nav_bench_null_terminated ${bench_sources})

    target_compile_features(nav_bench PRIVATE cxx_std_17)
    target_compile_features(nav_bench_null_terminated PRIVATE cxx_std_17)

    target_link_libraries(nav_bench ${bench_libs})
    target_link_libraries(nav_bench_null_terminated ${bench_libs})

    target_compile_definitions(nav_bench_null_terminated
                               PRIVATE "NAV_ADD_NULL_TERMINATORS=1")
endif()
