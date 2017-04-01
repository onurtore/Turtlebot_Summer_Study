
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>	// sleep


#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <kobuki_msgs/BumperEvent.h>


const double PI = 3.13159265359;

using namespace std;

ros::Publisher velocity_publisher;
ros::Publisher velocity_publisher2;
ros::Subscriber buffer_subscriber;
kobuki_msgs::BumperEvent kobuki_buffer;

void move();
double degrees2radians(double angle_in_degrees);
void changeDirection(double relative_angle);
void poseCallback(const kobuki_msgs::BumperEvent msg);

int main(int argc, char **argv){

	srand(time(NULL));
	sleep(20);

	ros::init(argc,argv,"move_random");
	ros::NodeHandle n;
	ros::NodeHandle b;
	velocity_publisher = n.advertise<geometry_msgs::Twist>("cmd_vel",1);
	velocity_publisher2 = n.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop",1);

	buffer_subscriber = b.subscribe<kobuki_msgs::BumperEvent>("/mobile_base/events/bumper",10,poseCallback);

	while(ros::ok()){
		move();
	}

	return 0;
}


void move(){
	int angle;
	geometry_msgs::Twist vel_msg;


	vel_msg.linear.x = 0;
	vel_msg.linear.y = 0;
	vel_msg.linear.z = 0;

	vel_msg.angular.x = 0;
	vel_msg.angular.y = 0;
	vel_msg.angular.z = 0;

	ros::Rate loop_rate(10);

//	do{
	if (kobuki_buffer.state == 0){
		vel_msg.linear.x = 0.25;
		velocity_publisher2.publish(vel_msg);
	}
	else {
		vel_msg.linear.x = 0;
		angle = rand() % 100 + 80;

		// Buffer Alarm -> Change Direction
		changeDirection(angle);
	}



	ros::spinOnce();
	//loop_rate.sleep();


	//while(kobuki_buffer.state == 0);
//}while(1);

	//vel_msg.linear.x=0;

	return;

}

void poseCallback(const kobuki_msgs::BumperEvent msg){
	kobuki_buffer.state = msg.state;
}



double degrees2radians(double angle_in_degrees){
	return angle_in_degrees * PI /180.0;

}


void changeDirection(double relative_angle){

	//Change If you want more
	double angular_speed = degrees2radians(60);


	relative_angle = degrees2radians(relative_angle);



	geometry_msgs::Twist vel_msg;
	//Set a random linear velocity in the x-axis
	vel_msg.linear.x=0;
	vel_msg.linear.y=0;
	vel_msg.linear.z=0;
	//set a random angular velocity in the y-axis
	vel_msg.angular.x=0;
	vel_msg.angular.y=0;
	vel_msg.angular.z = -abs(angular_speed);

	double current_angle = 0.0;
	double t0 = ros::Time::now().toSec();
	ros::Rate loop_rate(10);

	do{
		velocity_publisher2.publish(vel_msg);

		double t1 = ros::Time::now().toSec();
		current_angle = angular_speed * (t1-t0);
		ros::spinOnce();
		loop_rate.sleep();

	} while(current_angle < relative_angle);

	vel_msg.angular.z = 0;
	velocity_publisher2.publish(vel_msg);
	return;
}

