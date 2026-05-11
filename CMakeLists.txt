cmake_minimum_required(VERSION 3.19)
project(Alfred LANGUAGES CXX)

# Set Qt6 path if needed
# cmake .. -DCMAKE_PREFIX_PATH=/home/lucfer/Qt/6.5.3/gcc_64

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Gui Network)

qt_standard_project_setup()

# Include all necessary directories
include_directories(
    ${CMAKE_SOURCE_DIR}/presentation
    ${CMAKE_SOURCE_DIR}/data
    ${CMAKE_SOURCE_DIR}/application
    /usr/local/include    # for QHotkey
)
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})

# Add the executable
qt_add_executable(Alfred
    WIN32 MACOSX_BUNDLE
    main.cpp
    presentation/applicationlauncherwidget.h
    presentation/applicationlauncherwidget.cpp
    data/IndexedItem.h
    data/filesystemindexer.h
    data/filesystemindexer.cpp
    application/searchengine.h
    application/searchengine.cpp


    data/filesystemindexingworker.h
    data/filesystemindexingworker.cpp
    presentation/applicationlauncherwidget.ui
    presentation/SearchBoxResultListRenderer.h
    application/applicationlauncherservice.h application/applicationlauncherservice.cpp
    presentation/applicationlauncherlayout.h presentation/applicationlauncherlayout.cpp

)

# Link libraries
target_link_libraries(Alfred
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Gui
        Qt::Network          # for QLocalSocket
        /usr/local/lib64/libqhotkey.so
        ${X11_LIBRARIES}
)

# X11 link directories
link_directories(${X11_LIBRARY_DIR})

# Installation
install(TARGETS Alfred
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
