#include "ros/ros.h"
#include "sensor_msgs/Image.h"
#include <cv_bridge/cv_bridge.h>
#include <opencv/cv.h>


using namespace std;

class Img_Format
{
public:
  Img_Format(ros::NodeHandle nh) : nh_(nh), nhp_("~")
  {
    nhp_.param("format", format_, string("mono8"));
          
    // Topics
    pub_ = nhp_.advertise<sensor_msgs::Image>("/topic_out", 10);
    sub_ = nh_.subscribe("/topic_in", 10, &Img_Format::callback, this);
  }


  void callback(const sensor_msgs::Image::ConstPtr& msg)
  {
    cv_bridge::CvImageConstPtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvShare(msg);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    cv::Mat mono8_img = cv::Mat(cv_ptr->image.size(), CV_8UC1);
    cv::convertScaleAbs(cv_ptr->image, mono8_img, 100, 0.0);

    pub_.publish(cv_ptr->toImageMsg());
  }


private:
  ros::NodeHandle nh_; 
  ros::NodeHandle nhp_;

  ros::Publisher pub_;
  ros::Subscriber sub_;

  string format_;
};


int main(int argc, char **argv)
{
  ros::init(argc, argv, "Img_Format");

  ROS_INFO("Initialize node to reduce image freq");

  ros::NodeHandle nh;
  Img_Format imgSub(nh);

  ros::Rate r(10); // 10 hz
  while (ros::ok())
  {
    ros::spinOnce();
    r.sleep();
  }
}
