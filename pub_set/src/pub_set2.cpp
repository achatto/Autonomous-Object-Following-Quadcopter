#include<ros/ros.h>
#include<stdlib.h>
#include<std_msgs/String.h>
#include<geometry_msgs/PoseStamped.h>
#include<geometry_msgs/Point.h>
#include<geometry_msgs/Vector3Stamped.h>
#include<mavros_msgs/PositionTarget.h>
#include<math.h>

#define PI 3.14159

int main(int argc, char **argv){

ros::init(argc, argv,"pub_set");

ros::NodeHandle nh;

ros::Publisher p1=nh.advertise<mavros_msgs::PositionTarget>("/mavros/setpoint_raw/local",40);

ros::Rate rate(40);
ros::spinOnce();
mavros_msgs::PositionTarget msg;
int count =1;
while(ros::ok){

msg.header.stamp=ros::Time::now();
msg.header.seq=count;
msg.header.frame_id=1;
msg.position.x=0;
msg.position.y=0;
msg.position.z=4;
msg.velocity.z=1;
/*msg.pose.position.x=0;//sin(count*PI/180);
msg.pose.position.y=0;//cos(count*PI/180);
msg.pose.position.z=5;
msg.pose.orientation.x=0;
msg.pose.orientation.y=0;
msg.pose.orientation.z=0;
msg.pose.orientation.w=1;*/

p1.publish(msg);

ros::spinOnce();
count++;
rate.sleep();

}

}

