//If you change the include orders, program wont compile ( Not a joke ) 

//For ROS
#include <ros/ros.h>

#include <pluginlib/class_list_macros.h>

//Send Velocity Message Over Network
#include <geometry_msgs/Twist.h>

//Using PointCloud ( For  3D Tracking)
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/PCLPointCloud2.h>
#include <pcl/conversions.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/filters/passthrough.h>
#include <pcl_ros/transforms.h>
#include <pcl_ros/transforms.h>
#include <pcl/common/common_headers.h>
#include <pcl/features/normal_3d.h>
#include <pcl/io/pcd_io.h>
#include <pcl/visualization/pcl_visualizer.h>
#include <pcl/console/parse.h>

//For sleeping
//Dont need
#include <unistd.h>

//For Kinect2_Bridge
#include <cv_bridge/cv_bridge.h>

//Using Image Codes
#include <image_transport/image_transport.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>

//Using Opencv (  For 2D Tracking)
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

//C++
#include <iostream>
#include <cstring>

//Using Boost Library
#include <boost/thread/thread.hpp>







//Subscriber for KinectV2 RGB video channel
ros::Subscriber rgb_subscriber;
//Subscriber for Point_Cloud
ros::Subscriber pcl_subscriber;
//Publisher for Velocity Messages
ros::Publisher geometry_publisher;



using namespace std;
//Opencv Namespace
using namespace cv;


//Opencv Rectangle for select
Rect2d roi;

//OpenCv matrix images
Mat frame;
Mat frame_depth;

//Tracker Object for Tracking
Ptr<Tracker> tracker = Tracker::create( "KCF" );

//Image Converter
cv_bridge::CvImagePtr cv_ptr;
cv_bridge::CvImagePtr cv_ptr2;

//Info about first frame or not
int first_frame_flag = 0;

//info about the Rectangle (Rect2d) (Rectangle top_left_x pixel)
double top_left_x	= 0;

//*********************
//For Test Purpose
int point_cloud_frame_number = 0;
//*********************

//Direction for Turtlebot ( left or right )
float direction = 0;

//Geometry Message
geometry_msgs::Twist cmd;

//Depth values for 3D tracking
unsigned long int total_depth = 0;
unsigned long int average_depth = 0;


//Depth distance we want to track the person or thing
int min_distance = 500;

//Real Distance between robot and thing
double speed  = 0;


//Function Prototype
void xAxisMovement();
void yAxisMovement(double distance);



void imageCb(const sensor_msgs::ImageConstPtr& msg)
{
	
	//Change the image type
	cv_bridge::CvImagePtr cv_ptr;
	try
	{
		cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}	
	
	//First Frame (For selecting the tracking object)
	if(!cv_ptr->image.empty() && first_frame_flag == 0)
	{
		frame = cv_ptr->image;
		roi = selectROI("tracker",frame);
		if(roi.width==0 || roi.height == 0)
			return;
		tracker->init(frame,roi);
		ROS_INFO("press ESC to quit");
		
		//Not first frame
		first_frame_flag++;

	}
	
	//Refresh Tracking Rectangle
	if(!cv_ptr->image.empty() &&  first_frame_flag == 1)
	{

		frame = cv_ptr->image;

		tracker->update(frame,roi);

		rectangle(frame,roi,Scalar(255,0,0),2,1);

		top_left_x = roi.x;

		//Only comment it if doesnt want to go left or right
		xAxisMovement();


		imshow("tracker",frame);
		//So suspicious
		if(waitKey(1) == 27 ) ;
	}
}
//For Y coordinate moving
void poseCallback(const sensor_msgs::ImageConstPtr& input)
{
	//Real distance between Robot and Object
	double distance;

	//If flag is zero thats means we didnt choose any  object for tracking
	//so dont do anything
	if (first_frame_flag == 0)
		return;

	//Number of points in the Rectangle
	unsigned int pixel_counter = 0;

	//ROS image Type
	try
	{
		cv_ptr = cv_bridge::toCvCopy(input,sensor_msgs::image_encodings::TYPE_16UC1);

	}
	catch (cv_bridge::Exception& e)
	{
		ROS_ERROR("cv_bridge exception: %s", e.what());
		return;
	}


	point_cloud_frame_number++;
	cout <<"\n\n" <<"point_cloud_frame_number :  "<< point_cloud_frame_number << "\n";


	//Calculate average depth by adding all depth values in Rectangle
	//Rectangle from 2D tracker
	for(int y = roi.y; y < roi.y + roi.height; y++)
	{
		for(int x = roi.x; x < roi.x + roi.width; x++){

			unsigned short int  pixel_value = cv_ptr->image.at<unsigned short int>(cv::Point(x,y));
			if ( pixel_value != 0)
			{
					total_depth  += pixel_value;
					//Actually its always same
					pixel_counter++;
			}

		}
	}

	//If total_depth is zero then
	/*
	 *  Turtlebot is too close to target ( it's a problem when we
	 *  select the target )
	 */
	if( total_depth != 0 )
	{
		average_depth = total_depth / pixel_counter;

		cout  << "average_depth is: " << average_depth << "\n";

		if ( average_depth >  min_distance)
		{
			distance = average_depth - min_distance;
			yAxisMovement(distance);
		}
		else
		{
			//Go back
			distance  = -1;
			yAxisMovement(distance);
		}
	}
	//Tracking start from  close distance
	else
	{
		average_depth = 0;
		cout  << "average_depth is: " << average_depth  << "\n";
		//Go back
		distance = -1;
		yAxisMovement(distance);

	}

	average_depth = 0;
	total_depth = 0;
}
//Forward and backward
void yAxisMovement(double distance)
{

	/*I want my backspeed always -100;
	For precaution
	*/
	if (distance < 0  ){
		distance  = -100;;

	}

	/*I want my max_speed always less then 0.2 ;
	/For precaution
	*/
	if(distance > 200 ){
		distance = 200;
		/*Only test purpose
		distance  = 100;
		*/
	}
	cout << "distance is(distance - min_distance) :" << distance << "\n";

	//For likening the logical turtlebot velocity commands divide by 1000.
	speed = distance / 1000;
	cout << "speed is :" << speed << "\n" ;

	//Dont move for small changes
	if(speed > 0  && speed < 0.1)
	{
		speed = 0;
	}

	cout << "speed is (normalized) :" << speed << "\n" ;

	//constant for speed factor
	cmd.linear.x =  speed * 1;


	geometry_publisher.publish(cmd);



}

//We are using top_left_x value for adjusting speed and other things
void xAxisMovement()
{
	//500 is the middle value
	// - value  = right
	// + value  = left



	if( top_left_x > 600 || top_left_x < 400 )
	{
		direction = 500 - top_left_x;
		//For likening the logical turtlebot velocity commands divide by 100.
		direction = direction / 100;


		//Ultra super-duper complex formula
		//Constant for speed factor
		cmd.angular.z = direction * 0.1 * 2 ;

		cout << "cmd.angular.z " << cmd.angular.z << "\n";

	}
	//Stop turtlebot
	else
		cmd.angular.z = 0;

	geometry_publisher.publish(cmd);
}



int main(int argc, char **argv)
{
	
	//Initialize ROS
	ros::init(argc,argv,"follower");

	//Nodehandle For differend Nodes
	ros::NodeHandle n;
	ros::NodeHandle p;

	//Subscribing the RGB image for Tracking operation
	rgb_subscriber = n.subscribe("/kinect2/qhd/image_color",1,imageCb);

	//Subscribing the depth image
	pcl_subscriber = n.subscribe("/kinect2/qhd/image_depth_rect",10,poseCallback);

	//Publishing for Velocity commands
	geometry_publisher = p.advertise<geometry_msgs::Twist>("cmd_vel_mux/input/teleop",1);

	//Taking Call-Backs
	ros::spin();

	return 0;

	//Epic Sax Guy Was Here
}
