#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <ros/callback_queue.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <cmath>
#include <geometry_msgs/TwistStamped.h>
using namespace std;
using namespace cv;
#define PI 3.14159

geometry_msgs::PoseStamped msg2;
void poseMessageReceived(const geometry_msgs::PoseStamped& msg){
msg2=msg;

}

int main( int argc, char** argv ){
  
  // declares all required variables
  ros::init(argc, argv,"kcf");

  ros::NodeHandle nh;
  image_transport::ImageTransport it(nh);
  ros::Publisher p1=nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",5);
  //  ros::Publisher p2=nh.advertise<geometry_msgs::PoseStamped>("/hi",5);

  //ros::Publisher p2=nh.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel",10);

  //image_transport::Publisher pub = it.advertise("image", 10);
  ros::Subscriber sub = nh.subscribe("/mavros/local_position/pose", 5 ,&poseMessageReceived); //It was 10.
   ros::Rate rate(20);
  geometry_msgs::PoseStamped msg;

  //geometry_msgs::TwistStamped msg1;

  // sensor_msgs::ImagePtr msg3;
  //cout<<sub<<endl;
  double Z, theta;
  float factor_x,factor_y, temp = 0;
  factor_x=0.1;
  factor_y=0.1;
  int flag = 0;
  int delaycount=20;
  theta = 0.52359877559;
  Rect2d roi;
  Mat frame;
  // create a tracker object
  Ptr<Tracker> tracker = Tracker::create( "KCF" );
  // set input video
  //std::string video = argv[1];
  VideoCapture cap(0);//video);
  
  int frame_width=   cap.get(CV_CAP_PROP_FRAME_WIDTH);
   int frame_height=   cap.get(CV_CAP_PROP_FRAME_HEIGHT);
   VideoWriter video("ouT.avi",CV_FOURCC('M','J','P','G'),30, Size(frame_width,frame_height),true);

  // get bounding box
  cap >> frame;
  roi=selectROI("tracker",frame);
  //quit if ROI was not selected
  if(roi.width==0 || roi.height==0)
    return 0;
  // initialize the tracker
  tracker->init(frame,roi);
  // perform the tracking process
  printf("Start the tracking process, press ESC to quit.\n");
  //float q_0,q_1,q_2,q_3;
  
  while(ros::ok){

    flag=flag+1;

    // get frame from the video
    cap >> frame;
    // stop the program if no more images
    if(frame.rows==0 || frame.cols==0)
      break;
    // update the tracking result
    tracker->update(frame,roi);
    // draw the tracked object
    rectangle( frame, roi, Scalar( 255, 0, 0 ), 2, 1 );
    ros::spinOnce();
    // show image with the tracked object
    msg.header.stamp=ros::Time::now();

    msg.header.frame_id=1;
    //Z= ((msg2.pose.position.z-0.5) / (598.04 * cos(theta) - (roi.y + (roi.height/2) - 254.254455) * sin(theta) ));
    //Z= ((msg2.pose.position.z/cos(theta))-(((msg2.pose.position.z)*tan(theta)*(roi.y + (roi.height/2)- 254.254455)/(598.04*cos(theta)+(roi.y + (roi.height/2))*sin(theta)))));
    if(abs(msg2.pose.position.x - temp)<=1.5)
    {
      Z= ((msg2.pose.position.z-0.5) / (598.04 * cos(theta) - (roi.y + (roi.height/2) - 254.254455) * sin(theta) ));
      temp = -( Z * ( ( roi.y + (roi.height/2) - 254.254455 ) /cos(theta) ) )  + msg2.pose.position.x;
      msg.pose.position.x = temp;
    }
    else
    {
      msg.pose.position.x = temp;
    }

    /*
    if(abs((roi.y + (roi.height/2) - 254.254455)) <= factor_x*roi.height)
    {
      msg.pose.position.x=msg2.pose.position.x;
    }
    else
    {
      msg.pose.position.x = -( Z * ( ( roi.y + (roi.height/2) - 254.254455 ) /cos(theta) ) )  + msg2.pose.position.x;  
    }
    */

    //msg.pose.position.x = 0;//-( Z * ( ( roi.y + (roi.height/2) - 254.254455 ) /cos(theta) ) )  + msg2.pose.position.x;//-(msg2.pose.position.z*(roi.x + (roi.width/2) - 345.036)/599.03) + msg2.pose.position.x;
    
    if(abs((roi.x + (roi.width/2)) - 345.036) <= factor_y*roi.width)
    {
      msg.pose.position.y=msg2.pose.position.y;
    }
    else
    {
      msg.pose.position.y = -( ( roi.x + (roi.width/2) - 345.036 ) * Z ) + msg2.pose.position.y;  
    }
    //msg.pose.position.y = 0;
    //(msg2.pose.position.z*(roi.y + (roi.height/2) - 254.254455)/598.04)+ msg2.pose.position.y;
    msg.pose.position.z = 5;//msg2.pose.position.z;//roi.x;//((170*578.25)/roi.width);
    msg.pose.orientation.x = 0;
    msg.pose.orientation.y = 0;
    msg.pose.orientation.z = 0;
    msg.pose.orientation.w = 1;
    /*
    q_0 = msg2.pose.orientation.x; 
    q_1 = msg2.pose.orientation.y;
    q_2 = msg2.pose.orientation.z;
    q_3 = msg2.pose.orientation.w;
    
    msg1.pose.orientation.x = atan((2*(q_0*q_1+q_2*q_3))/(1-2*(q_1*q_1+q_2*q_2))) * 180/PI;
    msg1.pose.orientation.y = asin(2*(q_0*q_2-q_3*q_1))* 180/PI;
    msg1.pose.orientation.z = atan((2*(q_0*q_3+q_2*q_1))/(1-2*(q_3*q_3+q_2*q_2)))* 180/PI;
    msg1.pose.orientation.w = 0;
    */
    
   //msg1.twist.linear.x=0;//1.5*sin(count*PI/180);
   //msg1.twist.linear.y=0;//1.5*cos(count*PI/180);
   //msg1.twist.linear.z=1;
   /*msg1.twist.angular.x=0;
   msg1.twist.angular.y=0;
   msg1.twist.angular.z=0;*/
  
    p1.publish(msg);
    //p2.publish(msg1);
    //p2.publish(msg1);
    video.write(frame);
    //msg3 = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
    //pub.publish(msg3);
    imshow("tracker",frame);
    rate.sleep();
    //quit on ESC button
    if(waitKey(1)==27)break;
  }
  return 0;
}
