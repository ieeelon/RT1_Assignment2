#include "ros/ros.h"
#include <ros/console.h>
#include "sensor_msgs/LaserScan.h"
#include "std_msgs/Float32.h"
#include "geometry_msgs/Twist.h"
#include "second_assignment/Service.h"
#include "geometry_msgs/Twist.h"
#include "std_srvs/Empty.h"

ros::ServiceClient ServiceClient;

//This part defines the basic UI. It then sends the letter user input to subscriber to change the speed of robot or reset it.
void message(const sensor_msgs::LaserScan::ConstPtr& msg){
	second_assignment::Service Service;
	
	std::cout << "\n\n[W] - increase speed, [S] - decrease speed\n\n";
	std::cout << "\n\n[R] - reset the race\n\n";
	
	char letter;
	std::cin >> letter;
	if (letter == 'W' || letter == 'w' || letter == 's' || 
	letter == 'S' ||  letter == 'R' || letter == 'r')
    	{
        Service.request.letter=letter;        
        ServiceClient.waitForExistence();
        ServiceClient.call(Service);
        
        std::cout<<"\nCOMMAND: \n" << Service.request.letter;
        std::cout << "\nCurrent speed: "<< Service.response.speed<<"\n\n";
    	}
        else
        {
        std::cout <<"\n\nUndefined input\n\n";
        }
}

//Main is for communcation
int main(int argc, char **argv){
	ros::init(argc,argv,"subscriber");
	ros::NodeHandle handler;
	ServiceClient = handler.serviceClient<second_assignment::Service>("/accelerator");
	ros::Subscriber subscriber = handler.subscribe("/base_scan", 1, message);  
	ros::spin();
	
	return 0;
}
