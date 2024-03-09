cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)
project(LookTheDungeon VERSION 0.1.13)
set(PROJECT_LABEL "Look! The Dungeon!")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
set(PROJECT_EDITOR ${PROJECT_NAME}Editor)

add_executable(
    ${PROJECT_NAME}
    Sources/Main.cpp
    resource.rc
    Sources/CMakeConfig.h
    Sources/Game/Game.cpp
    Sources/Game/Game.h
    Sources/Game/IsometricCamera.h
    Sources/Game/IsometricCamera.cpp
)

add_executable(
    ${PROJECT_EDITOR}
    Sources/EditorMain.cpp
    resource.rc
    Sources/CMakeConfig.h
    Sources/Editor/Editor.h
    Sources/Editor/Editor.cpp
    Sources/Game/Game.cpp
    Sources/Game/Game.h
    Sources/Editor/IsometricCamera.cpp
    Sources/Editor/IsometricCamera.h
    Sources/Editor/EditorUI.cpp
    Sources/Editor/EditorUI.h
    Sources/Editor/FreeMoveCamera.h
    Sources/Editor/FreeMoveCamera.cpp
)

add_subdirectory(Sources/Engine)

target_include_directories(${PROJECT_NAME} PRIVATE Sources)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${CMAKE_BUILD_TYPE}")

target_include_directories(${PROJECT_EDITOR} PRIVATE Sources)
set_target_properties(${PROJECT_EDITOR} PROPERTIES OUTPUT_NAME "${PROJECT_EDITOR}_${CMAKE_BUILD_TYPE}")

configure_file("${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h.in" "${PROJECT_SOURCE_DIR}/Sources/CMakeConfig.h")
configure_file("${PROJECT_SOURCE_DIR}/resource.rc.in" "${PROJECT_SOURCE_DIR}/resource.rc")

target_link_libraries(${PROJECT_NAME} Cygine)
target_link_libraries(${PROJECT_EDITOR} Cygine)