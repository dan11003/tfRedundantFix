/*##############################################################################################################################
### BY DANIEL ADOLFSSON
### USAGE: head to the folder containing target rosbag   "rosrun ndt_offline tf_writer --bag-file-path 2012-01-08_vel_odom.bag --data-set michigan --odom-topic /robot_odom" make sure the odometry message topic is specified by the command --odom-topic
<<<<<<< HEAD
###
*/
#include <ros/ros.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <dirent.h>
#include <ros/package.h>
#include "pcl_conversions/pcl_conversions.h"
#include "pcl_conversions/pcl_conversions.h"
#include "tf/tf.h"
#include "tf/tfMessage.h"
#include "tf/transform_datatypes.h"
#include "map"
using namespace std;
namespace po = boost::program_options;

class FixTf{
public:
  FixTf(const string &input,const std::string &output){

    cout << "open:   " << input << endl;
    cout << "output: " << output << endl;
    rosbag::Bag bagOut_(output, rosbag::bagmode::Write);
    rosbag::Bag bagIn_(input);
    view_ = new rosbag::View(bagIn_);
    cout << "start copy" << endl;
    for( rosbag::View::iterator itr = view_->begin() ; itr != view_->end() ; itr++){
      rosbag::MessageInstance const m = *itr;
      if(m.getTopic() != "/tf"){

        bagOut_.write(m.getTopic(), m.getTime(), m);
      }else{


        tf::tfMessage msgTf = *m.instantiate<tf::tfMessage>();

        for(auto&& tf : msgTf.transforms){
          std::string key = tf.header.frame_id + " - " + tf.child_frame_id;
          /*if(tf.header.frame_id == "odom" && tf.child_frame_id == "base_link"){
              std::uint64_t time;
              pcl_conversions::toPCL(tf.header.stamp, time);
              cout <<"tf     message time: " << time << endl;
              cout <<"msg    message time: " << m.getTime() << endl;
          }*/
          std::uint64_t time;
          pcl_conversions::toPCL(tf.header.stamp, time);
          if(transforms_.find(key) != transforms_.end()){
              if(transforms_[key].find(time) != transforms_[key].end()){
                  if(redundancies_.find(key) != redundancies_.end()){
                      redundancies_[key]++;
                  }
                  else{
                      redundancies_[key] = 1;
                  }
              }
          }
          transforms_[key][time] = tf; // removes doubles here
          countBefore++;

        }
      }
    }
    for (auto itr = transforms_.begin() ; itr != transforms_.end() ; itr++){
      for(auto frameItr = itr->second.begin() ; frameItr != itr->second.end() ; frameItr++){
        tf::tfMessage message;
        message.transforms.push_back(frameItr->second);
        bagOut_.write("tf",message.transforms.front().header.stamp,message);
        countAfter++;
      }
    }

    bagIn_.close();
    bagOut_.close();
    cout << "removed redundancies, from: " << countBefore << " to " <<countAfter << endl;
    for(auto& [key, count] : redundancies_){
        cout << key << ": " << count << endl;
    }

  }


private:
  rosbag::View *view_;

  typedef std::map<std::uint64_t,geometry_msgs::TransformStamped> stampsMap_;
  std::map<std::string,stampsMap_> transforms_;
  int countBefore = 0;
  int countAfter = 0;
  std::map<std::string,int> redundancies_;
};

int main(int argc, char **argv){

  ros::Time::init();
  string inbag_name1,inbag_name2,directory_name,output_filename;
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("input", po::value<std::string>(&inbag_name1)->default_value(""), "bag file 1 to be merged")
      ("output", po::value<std::string>(&inbag_name2)->default_value(""), "bag file 2 to be merged")
      ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }
  cout << "input: " << inbag_name1 << " output: " << inbag_name2 << endl;
  FixTf fix(inbag_name1, inbag_name2);

  return 0;
}
