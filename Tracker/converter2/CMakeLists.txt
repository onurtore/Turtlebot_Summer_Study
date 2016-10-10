cmake_minimum_required(VERSION 2.8.3)
project(converter2)
find_package(catkin REQUIRED COMPONENTS
	cv_bridge	
  	pcl_conversions
	pcl_ros
	image_transport
	image_geometry
	roscpp
	sensor_msgs
	std_msgs
	tf
)

find_package( OpenCV 3 REQUIRED )

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES converter
#  CATKIN_DEPENDS cv_bridge image_transport roscpp sensor_msgs std_msgs
#  DEPENDS system_lib
)


include_directories(
	${catkin_INCLUDE_DIRS}
	${OpenCV_INCLUDE_DIRS}
)




add_executable(converter2 src/converter2.cpp)
target_link_libraries(converter2 ${OpenCV_LIBRARIES}  ${catkin_LIBRARIES})
add_dependencies(converter2 beginner_tutorials_gencpp)

