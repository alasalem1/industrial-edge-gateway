cmake_minimum_required(VERSION 3.16)

project(SentinelEdgeControlCenter
    VERSION 0.1
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)

qt_add_executable(sentinel-edge-ui
    src/main.cpp
    src/system/SystemMonitor.cpp
    src/system/SystemMonitor.h
    src/telemetry/TelemetryMonitor.cpp
    src/telemetry/TelemetryMonitor.h
)

# --- QML sources -------------------------------------------------------
# Source files are organised in folders, but the QML module is flat:
# QT_RESOURCE_ALIAS gives each file a short resource name, so every type
# can use every other type without imports.
set_source_files_properties(qml/Main.qml
    PROPERTIES QT_RESOURCE_ALIAS Main.qml)

set_source_files_properties(qml/theme/Theme.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
        QT_RESOURCE_ALIAS Theme.qml)

qt_add_qml_module(sentinel-edge-ui
    URI SentinelEdge
    VERSION 1.0
    RESOURCE_PREFIX /
    NO_PLUGIN
    QML_FILES
        qml/Main.qml
        qml/theme/Theme.qml
)

target_link_libraries(sentinel-edge-ui
    PRIVATE
        Qt6::Quick
)

install(TARGETS sentinel-edge-ui
    RUNTIME DESTINATION bin
)
