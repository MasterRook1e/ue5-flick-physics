cmake_minimum_required(VERSION 3.20)
project(FlickPhysicsPortableCore VERSION 0.4.0 LANGUAGES CXX)

option(FLICK_PHYSICS_BUILD_TESTS "Build portable-core tests" ON)
option(FLICK_PHYSICS_BUILD_EXAMPLES "Build portable examples" ON)
option(FLICK_PHYSICS_BUILD_BENCHMARKS "Build the portable microbenchmark" OFF)
option(FLICK_PHYSICS_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

add_library(FlickPhysicsPortableCore INTERFACE)
add_library(FlickPhysics::PortableCore ALIAS FlickPhysicsPortableCore)
set_target_properties(FlickPhysicsPortableCore PROPERTIES EXPORT_NAME PortableCore)

target_compile_features(FlickPhysicsPortableCore INTERFACE cxx_std_17)
set_target_properties(FlickPhysicsPortableCore PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(
    FlickPhysicsPortableCore
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source/FlickPhysics/Public>
        $<INSTALL_INTERFACE:include/FlickPhysics>
)

function(flick_physics_configure_target TargetName)
    if(MSVC)
        target_compile_options(${TargetName} PRIVATE /W4 /permissive- /WX)
    else()
        target_compile_options(${TargetName} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()

    if(FLICK_PHYSICS_ENABLE_SANITIZERS AND NOT MSVC)
        target_compile_options(${TargetName} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${TargetName} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

include(CTest)

if(FLICK_PHYSICS_BUILD_TESTS)
    add_executable(
        flick_physics_portable_tests
        Tests/Portable/CommandTests.cpp
        Tests/Portable/GeometryTests.cpp
        Tests/Portable/LaunchTests.cpp
        Tests/Portable/LifecyclePropertyTests.cpp
        Tests/Portable/LifecycleTests.cpp
        Tests/Portable/Main.cpp
        Tests/Portable/MotionTests.cpp
        Tests/Portable/PropertyTests.cpp
        Tests/Portable/TrajectoryTests.cpp
    )
    target_link_libraries(flick_physics_portable_tests PRIVATE FlickPhysics::PortableCore)
    flick_physics_configure_target(flick_physics_portable_tests)
    add_test(NAME FlickPhysicsPortableCore COMMAND flick_physics_portable_tests)
endif()

if(FLICK_PHYSICS_BUILD_EXAMPLES)
    add_executable(
        flick_physics_cli
        Examples/FlickPhysicsCli.cpp
    )
    target_link_libraries(flick_physics_cli PRIVATE FlickPhysics::PortableCore)
    flick_physics_configure_target(flick_physics_cli)
    if(FLICK_PHYSICS_BUILD_TESTS)
        add_test(NAME FlickPhysicsCliSmoke COMMAND flick_physics_cli)
    endif()
endif()

if(FLICK_PHYSICS_BUILD_BENCHMARKS)
    add_executable(
        flick_physics_benchmark
        Benchmarks/FlickPhysicsBenchmark.cpp
    )
    target_link_libraries(flick_physics_benchmark PRIVATE FlickPhysics::PortableCore)
    flick_physics_configure_target(flick_physics_benchmark)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS FlickPhysicsPortableCore EXPORT FlickPhysicsPortableCoreTargets)
install(
    FILES Source/FlickPhysics/Public/FlickPhysicsPortableCore.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FlickPhysics
)
install(
    DIRECTORY Source/FlickPhysics/Public/Portable/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FlickPhysics/Portable
    FILES_MATCHING PATTERN "*.h"
)
install(
    EXPORT FlickPhysicsPortableCoreTargets
    FILE FlickPhysicsPortableCoreTargets.cmake
    NAMESPACE FlickPhysics::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FlickPhysicsPortableCore
)

configure_package_config_file(
    cmake/FlickPhysicsPortableCoreConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/FlickPhysicsPortableCoreConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FlickPhysicsPortableCore
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/FlickPhysicsPortableCoreConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMinorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/FlickPhysicsPortableCoreConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/FlickPhysicsPortableCoreConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FlickPhysicsPortableCore
)
