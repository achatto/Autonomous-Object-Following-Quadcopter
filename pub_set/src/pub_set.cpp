#include<ros/ros.h>
#include<stdlib.h>
//#include<cmath>
#include<std_msgs/String.h>
#include<geometry_msgs/PoseStamped.h>
#include<geometry_msgs/Vector3Stamped.h>
#include<opencv_apps/FaceArrayStamped.h>
#include<iomanip>


ros::Publisher *pub;

void poseMessageReceived(const geometry_msgs::PoseStamped&  msg){

geometry_msgs::PoseStamped msg2;

int count =1;
ros::Rate rate(40);
while(ros::ok){

msg2.header.stamp=ros::Time::now();
msg2.header.seq=count;
msg2.header.frame_id=1;
msg2.pose.position.x=(msg.faces[0].face.x-130)/8;
msg2.pose.position.y=0;
msg2.pose.position.z=6;
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

using namespace std;

int main(int argc, char **argv){

ros::init(argc, argv,"pub_set");

ros::NodeHandle nh;

pub= new ros::Publisher(nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",1000));
ros::Subscriber sub = nh.subscribe("/face_detection/faces", 1000 ,&poseMessageReceived);



ros::spin();
}

