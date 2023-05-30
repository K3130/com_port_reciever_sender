cmake_minimum_required (VERSION 3.8)

project ("com_port_reciever_sender")





add_executable (com_port_reciever_sender WIN32 "src/com_port_reciever_sender.cpp" "src/com_port_reciever_sender.h" "src/main.cpp" "src/ui_from_simple.h" "src/ui_from_simple.cpp")

find_package(wxWidgets CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE wx::core wx::base)

find_package(Boost 1.81.0)


set(PATH_ASIO "D:\\vcpkg\\installed\\x64-windows\\include") #example

target_include_directories(${PROJECT_NAME} PRIVATE ${PATH_ASIO})


#vcpkg install wxwidgets:x64-windows-static