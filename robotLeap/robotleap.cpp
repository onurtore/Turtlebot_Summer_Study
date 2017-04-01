#include <iostream>
#include <string.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <kobuki_msgs/BumperEvent.h>
#include <kobuki_msgs/CliffEvent.h>
#include <yocs_controllers/default_controller.hpp>




#include <unistd.h>	// time
#include <time.h> //time


#include "Leap.h"

using namespace std;
using namespace Leap;

ros::Subscriber buffer_subscriber;
ros::Subscriber cliff_subscriber;
ros::Publisher velocity_publisher;

geometry_msgs::Twist vel_msg;
kobuki_msgs::BumperEvent kobuki_buffer;
kobuki_msgs::CliffEvent kobuki_cliff;


float pitch = 0;
float hand_palm_roll = 0;

Vector direction;

int bumper = 0;
int cliff = 0;

int first_movement = 1;

class SampleListener : public Listener {
    public:
    virtual void onConnect(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void move(const Frame &frame);
    virtual void sensors(const Frame &frame);
    void poseCallback(const kobuki_msgs::BumperEvent msg);
    void cliffCallback(const kobuki_msgs::CliffEvent msg);
 };

void SampleListener::onConnect(const Controller& controller) {
	std::cout << "Connected" << std::endl;
	controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::poseCallback(const kobuki_msgs::BumperEvent msg){
	kobuki_buffer.state = msg.state;
}

void SampleListener::cliffCallback(const kobuki_msgs::CliffEvent msg){
	kobuki_cliff.state = msg.state;
}

void SampleListener::move(const Frame &frame){
		//For forward ancd backward
		if(pitch > 0.5 )
			vel_msg.linear.x = pitch /4;

		else if(pitch < -0.5)
			vel_msg.linear.x = pitch/4;
		else
			vel_msg.linear.x = 0.0;

		//For left and right
			if(hand_palm_roll  > 0.43 )
				vel_msg.angular.z = (hand_palm_roll);

			else if(hand_palm_roll < -0.43)
				vel_msg.angular.z = (hand_palm_roll);
			else{
				vel_msg.angular.z = 0.0;
			}
}

void SampleListener::sensors(const Frame &frame){
	if(kobuki_buffer.state == 1){
		bumper = 1;
	}
	if(kobuki_cliff.state == 1){
		cliff = 1;
	}
}

void SampleListener::onFrame(const Controller& controller) {


	while( !velocity_publisher ) {
		  ROS_WARN("Publisher invalid!");
	}

	const Frame frame = controller.frame();

	Hand rightmostHand = frame.hands().rightmost();
	Hand leftmostHand = frame.hands().leftmost();


	hand_palm_roll = rightmostHand.palmNormal().roll();
	pitch = leftmostHand.direction().pitch();



	SampleListener::move(frame);
	SampleListener::sensors(frame);


	if(bumper == 1 || cliff == 1){
		if(frame.fingers().count() != 10){
			vel_msg.linear.x = 0;
			vel_msg.angular.z = 0;
		}
		else
			bumper = 0;
			cliff = 0;
	}
	if(frame.fingers().count() == 0 ){
		vel_msg.linear.x = 0;
		vel_msg.angular.z = 0;

	}

	velocity_publisher.publish(vel_msg);

	ros::spinOnce();

}

int main(int argc, char** argv)
{
	ros::init(argc,argv,"robotleap");

	ros::NodeHandle n;
	ros::NodeHandle b;

	SampleListener listener;
	Controller controller;


	controller.addListener(listener);


	velocity_publisher = n.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop",1);


	buffer_subscriber = b.subscribe<kobuki_msgs::BumperEvent>("/mobile_base/events/bumper",1,&SampleListener::poseCallback,&listener);
	cliff_subscriber = b.subscribe<kobuki_msgs::CliffEvent>("/mobile_base/events/cliff",1,&SampleListener::cliffCallback,&listener);

	std::cout << "Press Enter to quit..." << std::endl;
	std::cin.get();
	controller.removeListener(listener);

return 0;
}
