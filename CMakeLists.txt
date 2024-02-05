cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
project(LookTheDungeon VERSION 0.1.0)   
set(PROJECT_LABEL "Look! The Dungeon!")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)


add_executable(
    ${PROJECT_NAME}
    Sources/Main.cpp
    resource.rc
    Sources/CMakeConfig.h
    Sources/Game/Game.cpp
    Sources/Game/Game.h
    Sources/Game/FreeMoveCamera.h
    Sources/Game/FreeMoveCamera.cpp
)

add_subdirectory(Sources/Engine)
target_include_directories(${PROJECT_NAME} PRIVATE Sources)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${CMAKE_BUILD_TYPE}")

configure_file("${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h.in" "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h")
configure_file("${PROJECT_SOURCE_DIR}/resource.rc.in" "${PROJECT_SOURCE_DIR}/resource.rc")

target_link_libraries(${PROJECT_NAME} Cygine)