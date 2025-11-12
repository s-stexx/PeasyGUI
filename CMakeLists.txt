cmake_minimum_required(VERSION 3.16)

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment version")
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures to produce code for")

project(PeasyGUI VERSION 0.9 LANGUAGES CXX)
add_definitions(-DAPP_VERSION="${PROJECT_VERSION}")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Network Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network Widgets Test)

# Collect all sources and headers
file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB PROJECT_HEADERS CONFIGURE_DEPENDS src/*.h)

# Remove the "src/" prefix for IDE display
# (this makes them appear flat in Qt Creator project tree)
foreach(_src ${PROJECT_SOURCES} ${PROJECT_HEADERS})
    get_filename_component(_filename ${_src} NAME)
    list(APPEND PROJECT_FILES_FLAT ${_src})
    source_group("" FILES ${_src})
endforeach()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(PeasyGUI
        MANUAL_FINALIZATION
        ${PROJECT_FILES_FLAT}
        resources/resources.qrc
    )
endif()

target_link_libraries(PeasyGUI PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)


if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER de.mesom.PeasyGUI)
endif()
set_target_properties(PeasyGUI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS PeasyGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(PeasyGUI)
endif()

add_subdirectory(tests)

