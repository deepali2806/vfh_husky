#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <stdlib.h>
#include <geometry_msgs/Pose2D.h>
#include "sensor_msgs/LaserScan.h"
#include "std_msgs/String.h"
#include <string>
#include <math.h>
#include <nav_msgs/Odometry.h>      
#include "sensor_msgs/Imu.h"
#include <tf/transform_datatypes.h>
#include "tf/transform_listener.h"
#include <angles/angles.h>


void processLaserScan(const sensor_msgs::LaserScan::ConstPtr& scan){
    //Ranges is the unbounded array i.e. vector and it has size of 720
        ROS_INFO("Printig....%lu", scan->ranges.size());

        for(int i = 0 ; i < scan->ranges.size(); i++){
            ROS_INFO("Value at angle %d is : %f", i%270, scan->ranges[i]);
            
        }
    }

void chatterCallback(const sensor_msgs::Imu::ConstPtr& msg)
 {

  // ROS_INFO("Imu Seq: [%d]", msg->header.seq);
// ROS_INFO("Imu Orientation x:@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ [%f], y: [%f]", msg->orientation.x,msg->orientation.y);
 
   }
   //Initializing the positions and z axis orientation
   double x=0.0;
   double y=0.0;
   double theta=0.0;
   
void odomCallback(const nav_msgs :: Odometry::ConstPtr& odom){
    //rostopic show Odometry
    x=odom->pose.pose.position.x;
    y=odom->pose.pose.position.y;
    double quat_x=odom->pose.pose.orientation.x;
    double quat_y=odom->pose.pose.orientation.y;
    double quat_z=odom->pose.pose.orientation.z;
    double quat_w=odom->pose.pose.orientation.w;

    tf::Quaternion q(quat_x, quat_y, quat_z, quat_w);       //Quaternion gives the rotation 
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);                             //Changing queternion to Euler which gives 3 values 
                                                            //yaw is rotation around z axis
    //std::cout << "Roll: " << roll << ", Pitch: " << pitch << ", Yaw: " << yaw << std::endl;
    theta=yaw;
    //theta=angles::normalize_angle_positive(yaw);
    std::cout<<"Rotation around z axis is"<<theta;
    
}


int main(int argc, char **argv) {
    //Initializes ROS, and sets up a node
    ros::init(argc, argv, "random_husky_commands");
    ros::NodeHandle nh;
    ros::Subscriber scanSub;


    //Ceates the publisher, and tells it to publish
    //to the husky_velocity_controller/cmd_vel topic, with a queue size of 100
    ros::Publisher pub=nh.advertise<geometry_msgs::Twist>("/husky_velocity_controller//cmd_vel", 100);
    //scanSub=nh.subscribe<sensor_msgs::LaserScan>("scan",10,processLaserScan);
        
    ros::Subscriber sub = nh.subscribe("/odometry/filtered", 10, odomCallback);

    geometry_msgs::Point goal;              //rostopic show Odometry
    //say goal is (5,5)
    goal.x=5;
    goal.y=5;

    geometry_msgs::Twist deep;              ///cmd_vel

    //Sets up the random number generator
    srand(time(0));

    //Sets the loop to publish at a rate of 10Hz
    ros::Rate rate(10);

      while(ros::ok()) {

            double inc_x=goal.x-x;
            double inc_y=goal.y-y;
            
            
            double angle_to_goal=atan2(inc_y,inc_x);        //atan2 returns in radians and it is inverse tan.

            if (abs(angle_to_goal - theta) > 0.1){          //if the vehicle is not facing to goal then change angle
                deep.linear.x=0.0;
                deep.angular.z=0.3;
            }

            else{                                           //if vehicle is facing to goal then go straight and dont change angle
                deep.linear.x=0.5;
                deep.angular.z=0.0;

            }

            //Publish the message
            pub.publish(deep);
            //Delays until it is time to send another message
            rate.sleep();

            ros::spinOnce();

        }

}
