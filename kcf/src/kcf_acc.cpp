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
   // ros::Publisher p2=nh.advertise<geometry_msgs::PoseStamped>("/hi",5);

  //ros::Publisher p2=nh.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel",10);

  //image_transport::Publisher pub = it.advertise("image", 10);
  ros::Subscriber sub = nh.subscribe("/mavros/local_position/pose", 5 ,&poseMessageReceived); //It was 10.
   ros::Rate rate(20);
  geometry_msgs::PoseStamped msg;
  geometry_msgs::PoseStamped msg1;

  //geometry_msgs::TwistStamped msg1;

  // sensor_msgs::ImagePtr msg3;
  //cout<<sub<<endl;
  double  alpha, u, v, theta, psi, X_C;
  alpha = 2*0.52359877559;
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
  float q_0,q_1,q_2,q_3, d;
  d=0.09;
  while(ros::ok){

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
  
    q_0 = msg2.pose.orientation.x; 
    q_1 = msg2.pose.orientation.y;
    q_2 = msg2.pose.orientation.z;
    q_3 = msg2.pose.orientation.w;
    
    psi = 0;//atan((2*(q_0*q_1+q_2*q_3))/(1-2*(q_1*q_1+q_2*q_2))) ;
    theta = 0;//asin(2*(q_0*q_2-q_3*q_1));
    u = (roi.x + roi.width/2 - 345.036);
    v = -(roi.y + roi.height/2 - 254.25445);
    // = atan((2*(q_0*q_3+q_2*q_1))/(1-2*(q_3*q_3+q_2*q_2)))* 180/PI;
    

    X_C = ( 598 * ( (-msg2.pose.position.z - d*cos(psi)*sin(theta) ) / (u*sin(psi) + 598 * cos(psi) * sin(theta - alpha) + v* cos(psi) * cos(alpha - theta))));

    msg.pose.position.x =  0;//( d * 598 * cos(theta) + X_C * 598 * cos(alpha - theta) + X_C * v* sin(alpha - theta)) / 598.0 - msg2.pose.position.z /tan(alpha) + msg2.pose.position.x ;
    msg.pose.position.y =  ( -X_C * u* cos(psi) - X_C * 598 * sin(alpha - theta) * sin(psi) + X_C * v* sin(psi)* cos(alpha-theta) + d*598*sin(psi) * sin(theta) )/598.0 + msg2.pose.position.y;
    msg.pose.position.z = 5;//msg2.pose.position.z;//roi.x;//((170*578.25)/roi.width);
    msg.pose.orientation.x = 0;
    msg.pose.orientation.y = 0;
    msg.pose.orientation.z = 0;
    msg.pose.orientation.w = 1;
 
    //msg1.pose.orientation.x = psi;
   
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
