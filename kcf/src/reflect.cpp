#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ros/ros.h>
#include<geometry_msgs/PoseStamped.h>


using namespace std;
using namespace cv;

ros::Publisher *pub;

void poseMessageReceived(const geometry_msgs::PoseStamped& msg){

geometry_msgs::PoseStamped msg2;

int count =1;
ros::Rate rate(100);
while(ros::ok){

msg2.header.stamp=ros::Time::now();
msg2.header.seq=count;
msg2.header.frame_id=1;
msg2.pose.position.x=msg.pose.position.x;
msg2.pose.position.y=0;
msg2.pose.position.z=5;
msg2.pose.orientation.x=0;
msg2.pose.orientation.y=0;
msg2.pose.orientation.z=0;
msg2.pose.orientation.w=1;

pub->publish(msg2);
ros::spinOnce();
count++;
rate.sleep();

}

}


int main(int argc, char **argv){

ros::init(argc, argv,"pub_set");

ros::NodeHandle nh;
pub= new ros::Publisher(nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",100));
  ros::Subscriber sub = nh.subscribe("/kcf", 100 ,&poseMessageReceived);
  ros::spin();
}

