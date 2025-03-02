###################################
# general config
###################################
cmake_minimum_required(VERSION 3.19)
project(RayPainter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Test)

qt_standard_project_setup()


##################################
# Ray Painter GUI binary
##################################
file(GLOB_RECURSE SRC_SOURCES "src/*.cpp" "src/*.hpp")
list(FILTER SRC_SOURCES EXCLUDE REGEX ".*src/main\\.cpp$")

qt_add_executable(RayPainter
    WIN32 MACOSX_BUNDLE
    ${SRC_SOURCES}
    src/main.cpp
)

target_include_directories(RayPainter PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(RayPainter PRIVATE Qt::Core Qt::Widgets)

####################################
# QTest config
####################################
# QtTest executables are built from subdirectory's conf
enable_testing()
add_subdirectory(tests)


##########################
# install/deploy config
##########################
include(GNUInstallDirs)

install(TARGETS RayPainter
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET RayPainter
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
