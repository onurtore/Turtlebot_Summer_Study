Tracking an object, with KCF algorithm and KinectV2 Depth data.

#How it Works ?

We are using Opencv with  KCF algorithm, after selecting the region of interest, we can easily find the coordinates of the
object to track in 3D frame. Left and Right movement use 2D image  Forward and Backward movements use
depth image.

##Dependencies
1.Indigo<br />
2.Opencv3.0 <br />
3.iai_kinect2 <br />

##Setup
First of all, you need an working driver for kinectv2 ( we are using iai_kinect2/From Thiemo Wiedemeyer ), We are using opencv3.0 so  iai_kinect2
and our package should be in distinct workspaces, there are known problems with Opencv3.0 and  kinect2_bridge :

####From Readme file/iai_kinectv2 

>Will it work with OpenCV 3.0

>Short answer: No.
>Long answer: Yes, it is possible to compile this package with OpenCV 3.0, but it will not work. This is because cv_bridge is used, which itself is compiled with OpenCV 2.4.x in ROS Indigo/Jade and linking against both OpenCV versions is not possible. Working support for OpenCV 3.0 might come with a future ROS release.

Kinetic uses Opencv3.0 as a default, maybe using it could solve this problem.

also our package should be in the same directory with  vision_opencv-kinetic package for use Tracking features

##Future Work
1)Fix Tracking for better analysis of the target<br />
2)Velocity Smoother for Turtlebot<br />
3)Adding Cliff and bumper sensors<br />
4)Maybe using a pointcloud data for depth tracking.<br />

In my code, i leave some of the outputs for future work, such as distance between the robot and object, also because of 
health of turtlebot our primary concern ( we have only one ) , all movements in low speed.

 
##Conclusion 
This project is not intended for a professional real time person tracking ( especially without an Velocity Smoother ), there are still flaws
, but comparing to the turtlebot_follower ( which uses point cloud data, for tracking the center of points in the y axis ) our
code is better, we are  tracking only a selected person or object, which is impossible with turtlebot_follower.other than this real-time
performences are mostly same.
