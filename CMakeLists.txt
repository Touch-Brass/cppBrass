cmake_minimum_required(VERSION 3.1)

project(
    "touch-brass"
    VERSION 1.0.0
)

file(GLOB SRCS_COMMON CONFIGURE_DEPENDS "src/model/*.cpp" "src/dsp/*.cpp" "src/io/*.cpp" "src/controller/*.cpp")
file(GLOB SRCS_CLI CONFIGURE_DEPENDS "src/cli/*.cpp")
file(GLOB SRCS_GUI CONFIGURE_DEPENDS "src/gui/*.cpp")

include_directories("src/model/" "src/dsp/" "src/io/" "src/controller/")

set(PROJECT_NAME_CLI ${PROJECT_NAME}-cli)
add_executable(${PROJECT_NAME_CLI} ${SRCS_CLI} ${SRCS_COMMON})
target_link_libraries(${PROJECT_NAME_CLI} portaudio)

find_package(Qt5 REQUIRED
    Core
    PrintSupport
    Gui
    Widgets
)

set(PROJECT_NAME_GUI ${PROJECT_NAME}-gui)
add_executable(${PROJECT_NAME_GUI} ${SRCS_GUI} ${SRCS_COMMON})
target_compile_options(${PROJECT_NAME_GUI} PRIVATE -fPIC)
target_link_libraries(${PROJECT_NAME_GUI} portaudio Qt5::Widgets Qt5::PrintSupport)
target_include_directories(${PROJECT_NAME_GUI} PUBLIC ${Qt5Widgets_INCLUDE_DIRS})