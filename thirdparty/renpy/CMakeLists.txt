cmake_minimum_required(VERSION 3.11)

install(DIRECTORY lib/python2.7/ DESTINATION "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/lib/python2.7")
install(
    FILES lib/${CMAKE_SYSTEM_PROCESSOR}/librenpython.so
    DESTINATION "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/lib"
)

install(DIRECTORY renpy/ DESTINATION "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/renpy")
install(DIRECTORY game/ DESTINATION "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/game")
install(FILES main.py DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})
