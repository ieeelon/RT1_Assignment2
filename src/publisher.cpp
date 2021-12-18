#include "ros/ros.h"
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>
#include <string>
#include <std_srvs/Empty.h>
#include <second_assignment/Service.h>


ros::Publisher pub;
std_srvs::Empty reset;

geometry_msgs::Twist transformation_msg;


float safe_distance = 0.9;
float speed = 1;
float orientation = 0;
float right[280];
float center[160];
float left[280];

//Function to find closest obstacle for specific side
float closest(float dist[], int resolution)
{
    float buf = 1.0;
    for(int i=0; i < resolution; i++) 
    {
      if(buf>dist[i]) 
      {
         buf=dist[i];
      }
   }
   return buf;
}
void decision_tree(float right, float center, float left)
{

    //Obstacle on the right
    if ( right < safe_distance and center > safe_distance and left > safe_distance)
    {
        speed=0.1;
        orientation=0.5;
        //cout << ("\nOBSTACLE ON THE RIGHT\n");
    }
    //Obstacle in front
    else if ( right > safe_distance and center < safe_distance and left > safe_distance)
    {
        speed=0;
        if (left < right)
        {
        orientation=-0.5;
        }
        else if (right < left)
        {
        orientation=0.5;
        }
        //cout << ("\nOBSTACLE IN FRONT\n");
    }
    //Obstacle on the left
    else if ( right > safe_distance and center > safe_distance and left < safe_distance)
    {
        speed=0.1;
        orientation=-0.5;
        //cout << ("\nOBSTACLE ON THE LEFT\n");
    }
    
    //Obstacle in front and right
    else if ( right < safe_distance and center < safe_distance and left > safe_distance)
    {
        speed=0;
        orientation=0.5;
    }
    //Obstacle in front and left
    else if ( right > safe_distance and center < safe_distance and left < safe_distance)
    {
        speed=0;
        orientation=-0.5;
    }    
    //Obstacle on the left and on the right
    else if (right < safe_distance-0.3 and center > safe_distance and left < safe_distance-0.3)
    {	
    	speed = 0.5;
        orientation=0;
        //cout << ("\nNO OBSTACLES\n");
    }

    transformation_msg.linear.x=speed;
    transformation_msg.angular.z=orientation;
    pub.publish(transformation_msg);
    
}

//Function to consider user input for speed and reset
bool setSpeed(second_assignment::Service::Request &request, second_assignment::Service::Response &response)

{
	if (request.letter == 'w' || request.letter == 'W')
	{
	speed +=0.5;
	request.letter == ' ';
	}
	else if (request.letter == 's' || request.letter == 'S' || speed >= 0.5)
	{
	speed +=-0.5;
	request.letter == ' ';
	}
	else if (request.letter == 'r' || request.letter == 'R')
	{
        speed = 1;
        ros::service::call("/reset_positions", reset);
        request.letter == ' ';
	}
	else
	{
		std::cout << "\nWrong button\n";
		request.letter == ' ';
		return false;
	}
	
	response.speed = speed;
	transformation_msg.linear.x=speed;
    	transformation_msg.angular.z=orientation;
    	pub.publish(transformation_msg);
	std::cout<<"\nTHE SPEED IS: \n"<< speed;
    	return true;
}

    
    void controller(const sensor_msgs::LaserScan::ConstPtr& msg)
{
    float right[280] , center[160] , left[280];
    for( int i=0 ; i < 720 ; i++)
    {
        if(i < 280)
        {
            right[i] = msg->ranges[i];
        }
        else if ((i >= 280) && (i < 440))
        {
            center[i-280] = msg->ranges[i];
        }
        else if ((i >= 440) && (i < 720))
        {
            left[i - 440] = msg->ranges[i];
        }        
    }
    float closest_right = closest(right , 280);
    float closest_center = closest(center , 160);
    float closest_left = closest(left , 280);

    decision_tree(closest_right , closest_center , closest_left);
    
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "publisher");
    ros::NodeHandle handler;

    pub = handler.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
    ros::Subscriber sub = handler.subscribe("/base_scan", 1, controller);
    ros::ServiceServer service = handler.advertiseService("/accelerator", setSpeed);
    ros::spin();
    return 0;
}

