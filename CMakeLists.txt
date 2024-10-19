cmake_minimum_required(VERSION 3.11)
project(su.sovietgames.everlasting_summer CXX)

set(CMAKE_SKIP_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/lib")

add_subdirectory(thirdparty/renpy)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_link_options(${PROJECT_NAME} PUBLIC -rdynamic)

# У меня не получилось пробросить -L../thirdparty/renpy/lib -llibrenpython.so
# используя cmake-таргеты, поэтому прокинул вручную

message(STATUS "Cmake system processor: ${CMAKE_SYSTEM_PROCESSOR}")

target_link_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/renpy/lib/${CMAKE_SYSTEM_PROCESSOR}
)
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        librenpython.so
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(IconSize 86x86 108x108 128x128 172x172)
foreach(_size IN LISTS IconSize)
    install(FILES icons/${_size}/${PROJECT_NAME}.png DESTINATION share/icons/hicolor/${_size}/apps)
endforeach()

install(FILES ${PROJECT_NAME}.desktop
    DESTINATION share/applications
)

add_custom_target(distfiles
    SOURCES
        ${PROJECT_NAME}.desktop
        rpm/${PROJECT_NAME}.spec
        rpm/${PROJECT_NAME}.changes.in
        rpm/${PROJECT_NAME}.changes.run.in)

file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}:bin
")
