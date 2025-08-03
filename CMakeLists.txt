cmake_minimum_required(VERSION 3.30)
project(serial_debug_tool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# 公司
#set(CMAKE_PREFIX_PATH "D:/software/Qt/6.5.3/msvc2019_64")
#file(COPY "D:/software/Qt/6.5.3/msvc2019_64/resources" DESTINATION ${CMAKE_BINARY_DIR})
#file(COPY "D:/software/Qt/6.5.3/msvc2019_64/translations" DESTINATION ${CMAKE_BINARY_DIR})
#自己
set(CMAKE_PREFIX_PATH "D:/Software/Qt/6.5.3/msvc2019_64")
file(COPY "D:/Software/Qt/6.5.3/msvc2019_64/resources" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "D:/Software/Qt/6.5.3/msvc2019_64/translations" DESTINATION ${CMAKE_BINARY_DIR})

set(PUBLISH_BIN_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_BIN_RELEASE})

file(GLOB UI_HEADERS "include/ui/*.h")  # 收集ui所有.h文件
file(GLOB CORE_HEADERS "include/core/*.h")  # 收集core所有.h文件
file(GLOB UTILS_HEADERS "include/utils/*.h")  # 收集utils所有.h文件
file(GLOB UI_SOURCES "src/ui/*.cpp")  # 收集core所有.cpp文件
file(GLOB CORE_SOURCES "src/core/*.cpp")  # 收集core所有.cpp文件
file(GLOB UTILS_SOURCES "src/utils/*.cpp")  # 收集utils所有.cpp文件
set(RC_FILE resources/version.rc) # 设置运行图标

# 包含目录
include_directories(include ${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Svg
        SvgWidgets
        SerialPort
        Concurrent
        WebEngineWidgets
        REQUIRED)

add_executable(${PROJECT_NAME}
        src/main.cpp
        ${UI_SOURCES}
        ${CORE_SOURCES}
        ${UTILS_SOURCES}
        ${UI_HEADERS}
        ${CORE_HEADERS}
        ${UTILS_HEADERS}
        ${RC_FILE}
)

# 添加资源文件
qt_add_resources(${PROJECT_NAME} "resources"
        PREFIX "/"
        FILES
        resources/icons/logo.svg
        resources/icons/serial.svg
        resources/icons/waves.svg
        resources/icons/settings.svg
        resources/icons/down_arrow.svg
        resources/icons/checkmark_blue.svg
        resources/icons/send.svg
        resources/icons/ikun.ico
        resources/icons/un_dev.svg
        resources/icons/ikun.svg
        resources/icons/add_series.svg
        resources/icons/remove_series.svg
        resources/icons/clear_series.svg
        resources/icons/import_series.svg
        resources/icons/export_series.svg
        resources/icons/start_series.svg
        resources/icons/stop_series.svg
        resources/qss/main_window.qss
        resources/qss/tab_bar.qss
        resources/qss/title_bar.qss
        resources/qss/serial_prot_config_tab.qss
        resources/qss/serial_port_connect_config_widget.qss
        resources/qss/serial_port_receive_settings_widget.qss
        resources/qss/serial_port_send_settings_widget.qss
        resources/qss/serial_port_data_receive_widget.qss
        resources/qss/serial_port_data_send_widget.qss
        resources/qss/serial_port_real_time_save_widget.qss
        resources/qss/wave_form_tab.qss
        resources/qss/wave_form_ctrl_widget.qss
        resources/qss/add_channel_dialog.qss
        resources/qss/dialog_base.qss
        resources/web/echarts.min.js
        resources/web/wave.html
)

target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Svg
        Qt::SvgWidgets
        Qt::SerialPort
        Qt::Concurrent
        Qt::WebEngineWidgets
)

# 设置最终生成的可执行文件名
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "IKUN"  # 指定输出文件名（不带扩展名）
        #        WIN32_EXECUTABLE ON
)

# 添加 Windows 部署规则
if (WIN32)
    set(DEBUG_SUFFIX)
    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")

    # 部署核心 DLL
    foreach (QT_LIB Core Gui Widgets Svg SvgWidgets SerialPort Concurrent WebEngineWidgets WebEngineCore)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        )
    endforeach ()

    # 部署 WebEngine 进程文件 (关键!)
    #    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #            COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #            "${QT_INSTALL_PATH}/bin/QtWebEngineProcess${DEBUG_SUFFIX}.exe"
    #            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    #    )

    # 部署 WebEngine 资源
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QT_INSTALL_PATH}/resources"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources"
    )

    # 部署 WebEngine 翻译文件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${QT_INSTALL_PATH}/translations"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/translations"
    )

    # 部署插件
    set(PLUGINS_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins")

    # SVG 插件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGINS_DIR}/imageformats"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_INSTALL_PATH}/plugins/imageformats/qsvg${DEBUG_SUFFIX}.dll"
            "${PLUGINS_DIR}/imageformats/"
    )

    # 平台插件
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGINS_DIR}/platforms"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
            "${PLUGINS_DIR}/platforms/"
    )
endif ()
