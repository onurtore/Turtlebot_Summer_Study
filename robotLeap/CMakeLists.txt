cmake_minimum_required(VERSION 2.8.3)
project(robotleap)


cmake_policy(SET CMP0015 NEW)

SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
INCLUDE_DIRECTORIES(/include/)
LINK_DIRECTORIES(/lib/x64/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")



find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  kobuki_msgs
  message_generation
  roscpp
  std_msgs
  kobuki_msgs
)


link_directories(/home/turtle/Desktop/LeapSDK/lib/x64)



catkin_package(

)


include_directories(
  ${catkin_INCLUDE_DIRS}
)


add_executable(robotleap robotleap.cpp)
target_link_libraries(robotleap ${catkin_LIBRARIES} -lLeap)
add_dependencies(robotleap beginner_tutorials_gencpp)

