/******************************************************************************
 *
 * Author: Simon Appel (simonappel62@gmail.com)
 * Date: 23/04/2018
 *
 */

// Include guard
#ifndef dbscan_H
#define dbscan_H

// Includes
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <queue>
#include <perception_msgs/tools.h>
#include <perception_msgs/ObjectArray.h>
#include <tf/transform_listener.h>
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>

// Namespaces
namespace detection{

using namespace sensor_msgs;
using namespace message_filters;
using namespace perception_msgs;

struct Parameter{

	float grid_range_max;
	float grid_cell_size;

	float ped_side_min;
	float ped_side_max;
	float ped_height_min;
	float ped_height_max;
	float ped_semantic_min;

	float car_side_min;
	float car_side_max;
	float car_height_min;
	float car_height_max;
	float car_semantic_min;
};

// Semantic information of a cluster
struct Semantic{

	std::map<int,int> classes;
	float confidence;
	int id;
	std::string name;
	int diff_counter;
};

// Geometric information of a cluster
struct Geometric{

	float x;
	float y;
	float width;
	float length;
	float ground_level;
	float height;
	float orientation;

	std::vector<cv::Point> cells;
	int num_cells;
};

// Information of a cluster
struct Cluster{

	// ID
	int id;

	// Kernel size
	int kernel;

	// Semantic information
	Semantic semantic;

	// Geometric information
	Geometric geometric;

	// Misc stuff
	cv::RotatedRect rect;
	cv::Scalar color;
	bool is_new_track;

};

class DbScan{

public:

	// Default constructor
	DbScan(ros::NodeHandle nh, ros::NodeHandle private_nh);

	// Virtual destructor
	virtual ~DbScan();

	virtual void process(const Image::ConstPtr & image_detection_grid,
		const Image::ConstPtr & image_raw_left);


private:

	// Node handle
	ros::NodeHandle nh_, private_nh_;

	// Class member
	std::vector<Cluster> clusters_;
	ObjectArray object_array_;
	int number_of_clusters_;
	int time_frame_;
	Parameter params_;
	Tools tools_;
	tf::TransformListener listener_;

	// Subscriber
	Subscriber<Image> image_detection_grid_sub_;
	Subscriber<Image> image_raw_left_sub_;
	typedef sync_policies::ExactTime<Image, Image> MySyncPolicy;
	Synchronizer<MySyncPolicy> sync_;

	// Publisher
	ros::Publisher object_array_pub_;
	ros::Publisher image_detection_pub_;

	// Class functions
	void runDbScan(cv::Mat grid);
	void getClusterDetails(const cv::Mat grid);
	void createObjectList();
	void addObject(const Cluster & c);
	void createDetectionImage(cv::Mat image_raw_left);
	bool hasShapeOfPed(const Cluster & c);
	bool hasShapeOfCar(const Cluster & c);
	bool isValidSemantic(const int semantic_class);
	bool isKittiValidSemantic(const int semantic_class);
	void printCluster(const Cluster & c);

};

} // namespace detection

#endif // dbscan_H
