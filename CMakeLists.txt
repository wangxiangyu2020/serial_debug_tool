cmake_minimum_required(VERSION 3.30)
project(serial_debug_tool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "D:/software/Qt/6.5.3/msvc2019_64")

file(COPY "D:/software/Qt/6.5.3/msvc2019_64/resources" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "D:/software/Qt/6.5.3/msvc2019_64/translations" DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB UI_HEADERS "include/ui/*.h")  # 收集ui所有.h文件
file(GLOB CORE_HEADERS "include/core/*.h")  # 收集core所有.h文件
file(GLOB UTILS_HEADERS "include/utils/*.h")  # 收集utils所有.h文件
file(GLOB UI_SOURCES "src/ui/*.cpp")  # 收集core所有.cpp文件
file(GLOB CORE_SOURCES "src/core/*.cpp")  # 收集core所有.cpp文件
file(GLOB UTILS_SOURCES "src/utils/*.cpp")  # 收集utils所有.cpp文件

# 包含目录
include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Svg
        SvgWidgets
        REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.cpp
        ${UI_SOURCES}
        ${CORE_SOURCES}
        ${UTILS_SOURCES}
        ${UI_HEADERS}
        ${CORE_HEADERS}
        ${UTILS_HEADERS}
)

# 添加资源文件
qt_add_resources(${PROJECT_NAME} "resources"
        PREFIX "/"
        FILES
        resources/icons/logo.svg
        resources/icons/serial.svg
        resources/icons/waves.svg
        resources/icons/settings.svg
        resources/qss/tab_style.qss
        resources/qss/title_bar_style.qss
        #        resources/web/echarts.min.js
        #        resources/web/wave.html
)

target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Svg
        Qt::SvgWidgets
)

# 添加 Windows 部署规则
if (WIN32)
    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")

    # 部署核心 DLL
    foreach (QT_LIB Core Gui Widgets Svg)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach ()

    # 部署 SVG 插件 (关键!)
    set(PLUGINS_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGINS_DIR}/imageformats"
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/plugins/imageformats/qsvg${DEBUG_SUFFIX}.dll"
            "${PLUGINS_DIR}/imageformats/"
    )

    # 部署平台插件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGINS_DIR}/platforms"
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
            "${PLUGINS_DIR}/platforms/"
    )
endif ()
