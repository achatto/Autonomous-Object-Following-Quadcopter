#include<ros/ros.h>
#include<stdlib.h>
//#include<cmath>
#include<std_msgs/String.h>
#include<geometry_msgs/PoseStamped.h>
#include<geometry_msgs/Vector3Stamped.h>
#include<opencv_apps/FaceArrayStamped.h>
#include<iomanip>


/*class SubscribeAndPublish
{

public:
  SubscribeAndPublish()
  {
   p1=nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",1000);
  }

}*/
ros::Publisher *pub;

void poseMessageReceived(const opencv_apps::FaceArrayStamped& msg){
//ROS_INFO_STREAM(std::setprecision(2)<<std::fixed<<"position:"<<msg); //<<" p o s i t i o n =("<<msg .faces[0].face
//opencv_apps::FaceArrayStamped msg1;
geometry_msgs::PoseStamped msg2;
//msg1.header.stamp=ros::Time::now();
//msg1.header.seq=0;
//msg1.header.frame_id=1;
//msg1.faces[0].face.x=300;
//msg1.faces[0].face.y=300;
//msg1.faces[0].face.width=100;
//msg1.faces[0].face.height=100; if()
//msg1.eyes[]=0;
//ROS_INFO_STREAM(msg);

//ros::NodeHandle nh;
int count =1;
ros::Rate rate(40);
while(ros::ok){

msg2.header.stamp=ros::Time::now();
msg2.header.seq=count;
msg2.header.frame_id=1;
double temp=0;
if(msg.faces[0].face.x)
  msg2.pose.position.x=(msg.faces[0].face.x-130)/8;
else
  msg2.pose.position.x=temp;

msg2.pose.position.y=0;
msg2.pose.position.z=5;
msg2.pose.orientation.x=0;
msg2.pose.orientation.y=0;
msg2.pose.orientation.z=0;
msg2.pose.orientation.w=1;

//ROS_INFO_STREAM(poseMessageReceived.msg);
/*if(count>600&count<1200){
msg.header.stamp=ros::Time::now();
msg.header.seq=count;
msg.header.frame_id=1;
msg.pose.position.x=3;
msg.pose.position.y=0;
msg.pose.position.z=3;
msg.pose.orientation.x=0;
msg.pose.orientation.y=0;
msg.pose.orientation.z=0;
msg.pose.orientation.w=1;

}
if(count>1200&count<1800){
msg.header.stamp=ros::Time::now();
msg.header.seq=count;
msg.header.frame_id=1;
msg.pose.position.x=3;
msg.pose.position.y=3;
msg.pose.position.z=3;
msg.pose.orientation.x=0;
msg.pose.orientation.y=0;
msg.pose.orientation.z=0;
msg.pose.orientation.w=1;

}
if(count>1800&count<2400){
msg.header.stamp=ros::Time::now();
msg.header.seq=count;
msg.header.frame_id=1;
msg.pose.position.x=0;
msg.pose.position.y=0;
msg.pose.position.z=3;
msg.pose.orientation.x=0;
msg.pose.orientation.y=0;
msg.pose.orientation.z=0;
msg.pose.orientation.w=1;

}
*/
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

pub= new ros::Publisher(nh.advertise<geometry_msgs::PoseStamped>("/mavros/vision_pose/pose",1000));
ros::Subscriber sub = nh.subscribe("/face_detection/faces", 1000 ,&poseMessageReceived);


//while(ros::ok){

//}

//ros::spinOnce();


ros::spin();
}

