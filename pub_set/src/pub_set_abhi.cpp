#include<ros/ros.h>
#include<stdlib.h>
#include<std_msgs/String.h>
#include<geometry_msgs/PoseStamped.h>
#include<geometry_msgs/Vector3Stamped.h>
#include<geometry_msgs/TwistStamped.h>
#include<math.h>
#include <stdio.h>
#include "std_msgs/Float64.h"
#define PI 3.14159

int main(int argc, char **argv){

ros::init(argc, argv,"pub_set");

ros::NodeHandle nh;

ros::Publisher p1=nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",20);
//ros::Publisher p2=nh.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel",10);

ros::Rate rate(20);
ros::spinOnce();
geometry_msgs::PoseStamped msg;
geometry_msgs::TwistStamped msg1;
int count =1;
while(ros::ok){

msg.header.stamp=ros::Time::now();
msg.header.seq=count;
msg.header.frame_id=1;
msg.pose.position.x=0;//1.5*sin(count*PI/180);
msg.pose.position.y=0;//1.5*cos(count*PI/180);
msg.pose.position.z=6;
msg.pose.orientation.x=0;
msg.pose.orientation.y=0;
msg.pose.orientation.z=0;
msg.pose.orientation.w=1;


/*msg1.twist.linear.x=0;//1.5*sin(count*PI/180);
msg1.twist.linear.y=0;//1.5*cos(count*PI/180);
msg1.twist.linear.z=0;
msg1.twist.angular.x=0;
msg1.twist.angular.y=0;
msg1.twist.angular.z=0;*/


p1.publish(msg);
//p2.publish(msg1);

ros::spinOnce();
count++;
rate.sleep();

}
return 0;
}

