cmake_minimum_required(VERSION 3.19)
project(RayPainter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# glob source and header files from project directories
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE HEADER_FILES "src/*.hpp")

qt_add_executable(RayPainter
    WIN32 MACOSX_BUNDLE
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

# include headers in src dir for target and its dependencies
target_include_directories(RayPainter PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(RayPainter
    PRIVATE
        Qt::Core
        Qt::Widgets
)

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
