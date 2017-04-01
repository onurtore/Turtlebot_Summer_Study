#include <kobuki_msgs/SensorState.h>
#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <unistd.h>

using namespace std;

ros::Publisher velocity_publisher;
ros::Subscriber buffer_subscriber;
kobuki_msgs::SensorState kobuki_sensor;

void move();
void poseCallback(const kobuki_msgs::SensorState msg);



int main(int argc, char **argv){

	kobuki_sensor.bottom.resize(3);

	sleep(10); //Sleep for 20 second


	ros::init(argc,argv,"move_random");
	ros::NodeHandle n;
	ros::NodeHandle b;


	velocity_publisher = n.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop",1);
	buffer_subscriber = b.subscribe<kobuki_msgs::SensorState>("/mobile_base/sensors/core",1,poseCallback);

	while(ros::ok()){
			move();
		}
return 0;
}

void move(){
	geometry_msgs::Twist vel_msg;

	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;
	vel_msg.angular.x = 0;
	vel_msg.angular.y = 0;




	//turn left if left IR sensor larger than 1650
	if(kobuki_sensor.bottom[2] >= 1650){
		vel_msg.angular.z = 0.7;
		vel_msg.linear.x = 0.01;
	}
	//turn right if right IR sensor larger than 1650
	else if(kobuki_sensor.bottom[0] >= 1650){
		vel_msg.angular.z = -0.7;
		vel_msg.linear.x = 0.01;
	}
	else
		vel_msg.angular.z = 0;
		vel_msg.linear.x = 0.12;

	velocity_publisher.publish(vel_msg);


	ros::spinOnce();


return;
}

void poseCallback(const kobuki_msgs::SensorState msg){



	kobuki_sensor.bottom[0] = msg.bottom[0];
	kobuki_sensor.bottom[1] = msg.bottom[1];
	kobuki_sensor.bottom[2] = msg.bottom[2];

}



