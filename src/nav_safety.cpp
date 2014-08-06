#include <carl_safety/nav_safety.h>

navSafety::navSafety()
{
  // a private handle for this ROS node (allows retrieval of relative parameters)
  ros::NodeHandle private_nh("~");
  
  // read in parameters
  std::string str;
  private_nh.param<std::string>("controller_type", str, "digital");
  if (str.compare("digital") == 0)
    controllerType = DIGITAL;
  else
    controllerType = ANALOG;

  // ROS topics
  baseCommandPublisher = node.advertise<geometry_msgs::Twist>("cmd_vel", 1);
  safeBaseCommandSubscriber = node.subscribe("cmd_vel_safe", 1, &navSafety::safeBaseCommandCallback, this);
  joySubscriber = node.subscribe("joy", 1, &navSafety::joyCallback, this);
  robotPoseSubscriber = node.subscribe("robot_pose", 1, &navSafety::poseCallback, this);
  
  //initialization
  stopped = false;
  x = 0.0;
  y = 0.0;
  theta = 0.0;
}

void navSafety::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  if (controllerType == DIGITAL)
  {
    if (joy->buttons.at(8) == 1)
      stopped = true;
    else if (joy->buttons.at(9) == 1)
      stopped = false;
  }
  else
  {
    if (joy->buttons.at(6) == 1)
      stopped = true;
    else if (joy->buttons.at(7) == 1)
      stopped = false;
  }
}

void navSafety::safeBaseCommandCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
  //TODO: get CARL's localization and prevent commands that drive over a boundary line
  
}

bool navSafety::isStopped()
{
  return stopped;
}

void navSafety::cancelNavGoals()
{
  //TODO: cancel all goals on the navigation action server
  
}

void navSafety::poseCallback(const geometry_msgs::Pose::ConstPtr& msg)
{
  x = msg->position.x;
  y = msg->position.y;
  
  //convert quaternion to yaw
  float q0 = msg->orientation.w;
  float q1 = msg->orientation.x;
  float q2 = msg->orientation.y;
  float q3 = msg->orientation.z;
  theta = -atan2(2*(q0*q3 + q1*q2), 1 - 2*(q2*q2 + q3*q3));
  
  ROS_INFO("theta: %f", theta);
}

int main(int argc, char **argv)
{
  // initialize ROS and the node
  ros::init(argc, argv, "nav_safety");

  navSafety n;

  ros::Rate loop_rate(60);
  while (ros::ok())
  {
    if (n.isStopped())
      n.cancelNavGoals();
  }
  ros::spin();

  return EXIT_SUCCESS;
}
