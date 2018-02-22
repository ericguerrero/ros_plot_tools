#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include "sensor_msgs/CameraInfo.h"

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

using namespace std;

class Img_Sub
{
public:
  Img_Sub(ros::NodeHandle nh) : nh_(nh), nhp_("~")
  {
    // Topics
    left_img_pub_ = nhp_.advertise<sensor_msgs::Image>("new_left_img", 10);
    right_img_pub_ = nhp_.advertise<sensor_msgs::Image>("new_right_img", 10);
    left_info_pub_ = nhp_.advertise<sensor_msgs::CameraInfo>("new_left_info", 10);
    right_info_pub_ = nhp_.advertise<sensor_msgs::CameraInfo>("new_right_info", 10);
  }


  void callback(const sensor_msgs::Image::ConstPtr& left_img,
                const sensor_msgs::Image::ConstPtr& right_img,
                const sensor_msgs::CameraInfo::ConstPtr& left_info,
                const sensor_msgs::CameraInfo::ConstPtr& right_info)
  {
    left_img_pub_.publish(left_img);
    right_img_pub_.publish(right_img);
    left_info_pub_.publish(left_info);
    right_info_pub_.publish(right_info);
  }


private:
  ros::NodeHandle nh_; 
  ros::NodeHandle nhp_;

  ros::Publisher left_img_pub_;
  ros::Publisher right_img_pub_;
  ros::Publisher left_info_pub_;
  ros::Publisher right_info_pub_;

};


int main(int argc, char **argv)
{
  ros::init(argc, argv, "img_sub");

  ROS_INFO("Initialize node to reduce image freq");

  ros::NodeHandle nh;
  Img_Sub imgSub(nh);

  // Message sync
  message_filters::Subscriber<sensor_msgs::Image> left_img_sub(nh, "/left_img", 50);
  message_filters::Subscriber<sensor_msgs::Image> right_img_sub(nh, "/right_img", 50);
  message_filters::Subscriber<sensor_msgs::CameraInfo> left_info_sub(nh, "/left_cam_info", 50);
  message_filters::Subscriber<sensor_msgs::CameraInfo> right_info_sub(nh, "/right_cam_info", 50);

  // Define syncs
  typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::Image,
                                                          sensor_msgs::Image,
                                                          sensor_msgs::CameraInfo,
                                                          sensor_msgs::CameraInfo> sync_pool;
  message_filters::Synchronizer<sync_pool> sync(sync_pool(50), left_img_sub, right_img_sub, left_info_sub, right_info_sub);
  sync.registerCallback(boost::bind(&Img_Sub::callback, &imgSub, _1, _2, _3, _4));

  ros::Rate r(2); // 10 hz
  while (ros::ok())
  {
    ros::spinOnce();
    r.sleep();
  }
}
