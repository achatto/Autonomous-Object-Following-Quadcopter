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
#include "boost/date_time/posix_time/posix_time.hpp"

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
  ros::Publisher p1 = nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",10);
  ros::Publisher p2 = nh.advertise<geometry_msgs::Point>("/Pixel_error", 10);
  //  ros::Publisher p2=nh.advertise<geometry_msgs::PoseStamped>("/hi",5);

  //ros::Publisher p2=nh.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel",10);

  //image_transport::Publisher pub = it.advertise("image", 10);
  ros::Subscriber sub = nh.subscribe("/mavros/local_position/pose", 10 ,&poseMessageReceived); //It was 10.
   ros::Rate rate(15);
  geometry_msgs::PoseStamped msg;
  geometry_msgs::Point msg1;

  //geometry_msgs::TwistStamped msg1;

  // sensor_msgs::ImagePtr msg3;
  //cout<<sub<<endl;
  double Z, theta;
  float factor_x,factor_y, E_x, E_y, temp = 0;
  int flag = 0;
  factor_x = 0.05;
  factor_y = 0.05;
  theta = 2*0.52359877559;
  Rect2d roi;
  Mat frame;
  // create a tracker object
  Ptr<Tracker> tracker = Tracker::create( "KCF" );
  // set input video
  //std::string video = argv[1];
  VideoCapture cap(0);//video);
  
  int frame_width =   cap.get(CV_CAP_PROP_FRAME_WIDTH);
   int frame_height =   cap.get(CV_CAP_PROP_FRAME_HEIGHT);
   std::string rtime = boost::lexical_cast<std::string>(ros::Time::now());
   rtime += ".avi";
   VideoWriter video("/home/karthikeya/Desktop/Tests/videos/"+ rtime, CV_FOURCC('M','J','P','G'), 30, Size(frame_width,frame_height), true);

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

   // flag = flag + 1 ;

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
    msg.header.stamp = ros::Time::now();
    //boost::posix_time::ptime my_posix_time = ros::Time::now().toBoost();
    //std::string iso_time_str = boost::posix_time::to_iso_extended_string(my_posix_time);
    std::string str = boost::lexical_cast<std::string>(ros::Time::now());
    putText(frame, str, cv::Point(5,15), cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, cv::Scalar(0 ,255, 0), 0.7, 8);

    msg.header.frame_id = 1;

    E_y = roi.y + (roi.height/2) - 254.254455;
    E_x = roi.x + (roi.width/2) - 345.036 ;



    Z = ((msg2.pose.position.z - 0.5) / (598.04 * cos(theta) - (E_y) * sin(theta) ));
    //Z= ((msg2.pose.position.z/cos(theta))-(((msg2.pose.position.z)*tan(theta)*(roi.y + (roi.height/2)- 254.254455)/(598.04*cos(theta)+(roi.y + (roi.height/2))*sin(theta)))));
    


    if(abs(msg2.pose.position.x - temp) <= 1)

    {
      //Z = ((msg2.pose.position.z-0.5) / (598.04 * cos(theta) - (roi.y + (roi.height/2) - 254.254455) * sin(theta) ));
      temp = -0.75* ( ( Z *  E_y ) / cos(theta)  )  + msg2.pose.position.x;
      msg.pose.position.x = temp;
    }

    else
    {
      msg.pose.position.x = temp;
    }

/*
    
    if( abs(E_y) <= (factor_x * frame_height))
    	//(abs((roi.y + (roi.height/2) - 254.254455)) <= factor_x * frame_height)

    {
      msg.pose.position.x = msg2.pose.position.x;
    }

    else

    {
      if(abs(msg2.pose.position.x - temp) <= 1)

      {
      //Z = ((msg2.pose.position.z-0.5) / (598.04 * cos(theta) - (roi.y + (roi.height/2) - 254.254455) * sin(theta) ));
        temp = -( ( Z *  E_y ) / cos(theta)  )  + msg2.pose.position.x;
        msg.pose.position.x = temp;
      }

      else
      {
        msg.pose.position.x = temp;
      }
  
    }
    */

    //msg.pose.position.x = 0;//-( Z * ( ( roi.y + (roi.height/2) - 254.254455 ) /cos(theta) ) )  + msg2.pose.position.x;//-(msg2.pose.position.z*(roi.x + (roi.width/2) - 345.036)/599.03) + msg2.pose.position.x;
   
    if( abs(E_x) <= factor_y * frame_width )
    
    {
      msg.pose.position.y = msg2.pose.position.y;
    }
    
    else
    
    {
      msg.pose.position.y = -( E_x ) * Z  + msg2.pose.position.y;  
    }
    
    //(msg2.pose.position.z*(roi.y + (roi.height/2) - 254.254455)/598.04)+ msg2.pose.position.y;
   // msg.pose.position.y = 0;
    msg.pose.position.z = 6;//msg2.pose.position.z;//roi.x;//((170*578.25)/roi.width);

    msg.pose.orientation.x = 0;
    msg.pose.orientation.y = 0;
    msg.pose.orientation.z = 0;
    msg.pose.orientation.w = 1;

    msg1.x = E_x;
    msg1.y = E_y;
    msg1.z = 0;

    p1.publish(msg);
    p2.publish(msg1);
    
    video.write(frame);
    //msg3 = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
    //pub.publish(msg3);
    imshow("tracker",frame);
    rate.sleep();
    //quit on ESC button
    if(waitKey(1) == 27)break;
  }
  return 0;
}
