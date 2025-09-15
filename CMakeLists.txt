cmake_minimum_required(VERSION 3.30)
project(serial_debug_tool)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


#set(CMAKE_PREFIX_PATH "D:/software/Qt/6.5.3/msvc2019_64")
set(CMAKE_PREFIX_PATH "D:/Software/Qt/6.5.3/msvc2019_64")


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
        resources/icons/sample_rate.svg
        resources/icons/tcp_network.svg
        resources/icons/modbus.svg
        resources/icons/silder_ikun.svg
        resources/icons/code.svg
        resources/icons/help.svg
        resources/image/ikun.gif
        resources/qss/main_window.qss
        resources/qss/tab_bar.qss
        resources/qss/settings_tab.qss
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
        resources/qss/remove_channel_dialog.qss
        resources/qss/sample_rate_dialog.qss
        resources/qss/dialog_base.qss
        resources/qss/tcp_network_config_tab.qss
        resources/qss/tcp_network_client_wdiget.qss
        resources/qss/tcp_network_server_wdiget.qss
        resources/qss/script_editor_dialog.qss
        resources/web/echarts.min.js
        resources/web/wave.html
        resources/qss/modbus_config_tab.qss
        resources/qss/modbus_display_widget.qss
        resources/qss/tag_manager_dialog.qss
        README.md
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
        OUTPUT_NAME "iKUN"  # 指定输出文件名（不带扩展名）
        WIN32_EXECUTABLE ON
)

# 添加 Windows 部署规则
if (WIN32)

    set(DEBUG_SUFFIX)

    if (CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()

    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")

    # 使用 windeployqt 进行全自动部署
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${QT_INSTALL_PATH}/bin/windeployqt.exe
            # --verbose 1 # 可以加上这个参数查看详细输出
            --dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>"  # 指定部署到的目录
            "$<TARGET_FILE:${PROJECT_NAME}>"           # 指定要部署的可执行文件
            COMMENT "Running windeployqt..."
    )
endif ()