cmake_minimum_required(VERSION 3.20)
project(FlickPhysicsPortableCoreConsumer LANGUAGES CXX)

find_package(FlickPhysicsPortableCore 0.4 CONFIG REQUIRED)

add_executable(flick_physics_consumer main.cpp)
target_link_libraries(flick_physics_consumer PRIVATE FlickPhysics::PortableCore)
target_compile_features(flick_physics_consumer PRIVATE cxx_std_17)

enable_testing()
add_test(NAME FlickPhysicsInstalledConsumer COMMAND flick_physics_consumer)
