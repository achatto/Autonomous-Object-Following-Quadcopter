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
  ros::Publisher p1=nh.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local",8);
  //ros::Publisher p2=nh.advertise<geometry_msgs::TwistStamped>("/mavros/setpoint_velocity/cmd_vel",10);

  //image_transport::Publisher pub = it.advertise("image", 10);
  ros::Subscriber sub = nh.subscribe("/mavros/local_position/pose", 5 ,&poseMessageReceived); //It was 10.
   ros::Rate rate(20);
  geometry_msgs::PoseStamped msg;
  geometry_msgs::TwistStamped msg1;

  // sensor_msgs::ImagePtr msg3;
  //cout<<sub<<endl;
  double theta, e_x = 0,e_y = 0,e_x_dot ,e_y_dot, E_x = 0, E_y = 0, t_s , t_ns , dt;
  float K_p = 0.012, K_d =0, K_i = 0.0;
  // theta = 1.04719755;


  Rect2d roi;
  Mat frame;
  // create a tracker object
  Ptr<Tracker> tracker = Tracker::create( "KCF" );
  // set input video
  //std::string video = argv[1];
  VideoCapture cap(0);//video);
  
  int frame_width=   cap.get(CV_CAP_PROP_FRAME_WIDTH);
   int frame_height=   cap.get(CV_CAP_PROP_FRAME_HEIGHT);
   VideoWriter video("ouTa.avi",CV_FOURCC('M','J','P','G'),30, Size(frame_width,frame_height),true);

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
  t_s = ros::Time::now().sec; t_ns = ros::Time::now().nsec;
  //ros::Duration(5).sleep();
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
    
    msg.header.frame_id=1;

    dt = ( (ros::Time::now().sec - t_s) + 0.000000001 * (ros::Time::now().nsec  - t_ns) );

    t_s = ros::Time::now().sec; t_ns = ros::Time::now().nsec;
    

    e_x_dot= (-( roi.y + (roi.height/2) - 254.254455 ) - e_x   )/ dt;
    e_y_dot= (-( roi.x + (roi.width/2) - 345.036 ) - e_y   ) / dt;
    
    E_x = E_x + (-( roi.y + (roi.height/2) - 254.254455 )* dt ) ;
    E_y = E_y + (-( roi.x + (roi.width/2) - 345.036  )* dt  ) ;

    e_x = -( roi.y + (roi.height/2) - 254.254455 ) ; 
    e_y = -( roi.x + (roi.width/2)  - 345.036    ) ;
    //Z= ((msg2.pose.position.z-0.5) / (598.04 * cos(theta) - (roi.y + (roi.height/2) - 254.254455) * sin(theta) ));
    //Z= ((msg2.pose.position.z/cos(theta))-(((msg2.pose.position.z)*tan(theta)*(roi.y + (roi.height/2)- 254.254455)/(598.04*cos(theta)+(roi.y + (roi.height/2))*sin(theta)))));
    msg.header.stamp = ros::Time::now();
    if(roi.x) {
    msg.pose.position.x = 0;//K_p * (e_x)  + K_d * (e_x_dot) + K_i * (E_x) + msg2.pose.position.x;//-(msg2.pose.position.z*(roi.x + (roi.width/2) - 345.036)/599.03) + msg2.pose.position.x;
    msg.pose.position.y = K_p * (e_y)  + K_d * (e_y_dot) + K_i * (E_y) + msg2.pose.position.y;// + msg2.pose.position.y;//(msg2.pose.position.z*(roi.y + (roi.height/2) - 254.254455)/598.04)+ msg2.pose.position.y;
    }

    msg.pose.position.z = 5;//msg2.pose.position.z;//roi.x;//((170*578.25)/roi.width);
    //msg.pose.orientation.x = 0;
    //msg.pose.orientation.y = 0;
    //msg.pose.orientation.z = 0;
    //msg.pose.orientation.w = 1;


   //msg1.twist.linear.x=0;//1.5*sin(count*PI/180);
   //msg1.twist.linear.y=0;//1.5*cos(count*PI/180);
   //msg1.twist.linear.z=1;
   /*msg1.twist.angular.x=0;
   msg1.twist.angular.y=0;
   msg1.twist.angular.z=0;*/
  
    p1.publish(msg);
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
