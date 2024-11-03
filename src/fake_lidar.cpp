#include <chrono>
#include <memory>
#include <cmath>
// to read csv file
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "lidar_config.hpp"

class FakeLidar : public rclcpp::Node
{
public:
	FakeLidar();

private:
	LidarConfig _config;
	rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr _scan_publisher;
	rclcpp::TimerBase::SharedPtr _scan_timer;

	std::string _filename = "/home/kiryl/ros_ws/src/fake_sensors/sample_lidar_data/lidar_data_noisy.csv";
	std::vector<std::vector<std::string>> _lidar_data_str;
	std::vector<std::vector<float>> _lidar_data_float;
	unsigned long _vector_index = 0;

private:
	void _publish_fake_scan();
	void _read_sample_lidar_data();
};

FakeLidar::FakeLidar() : rclcpp::Node("fake_lidar")
{

	_config.declare_parameters(this);
	_config.update_parameters(this);
	_config.print_config(this);

	_scan_publisher = this->create_publisher<sensor_msgs::msg::LaserScan>("/scan", 10);
	_scan_timer = this->create_wall_timer(
		std::chrono::milliseconds(_config.get_scan_period_ms()),
		std::bind(&FakeLidar::_publish_fake_scan, this));

	_read_sample_lidar_data();
}

void FakeLidar::_read_sample_lidar_data()
{

	// read csv file
	std::string line;
	// Open the file
	std::ifstream file(_filename);

	// Check if the file is open
	if (!file.is_open())
	{
		std::cerr << "Could not open the file: " << _filename << std::endl;
	}
	while (std::getline(file, line))
	{
		std::vector<std::string> row;
		std::stringstream ss(line);
		std::string cell;

		// Split line by commas
		while (std::getline(ss, cell, ','))
		{
			row.push_back(cell);
		}

		// Add row to data
		_lidar_data_str.push_back(row);
	}
	file.close();

	// convert vector str to vector float
	for (const auto &row : _lidar_data_str)
	{
		std::vector<float> float_row;
		for (const auto &str : row)
		{
			try
			{
				float_row.push_back(std::stof(str));
			}
			catch (const std::invalid_argument &e)
			{
				std::cerr << "Invalid argument: " << str << " is not a valid float." << std::endl;
			}
			catch (const std::out_of_range &e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		_lidar_data_float.push_back(float_row);
	}
}

void FakeLidar::_publish_fake_scan()
{
	auto msg = sensor_msgs::msg::LaserScan();

	msg.header.stamp = this->now();
	msg.header.frame_id = "laser_frame";

	msg.angle_min = _config.angle.first;
	msg.angle_max = _config.angle.second;
	msg.range_min = _config.range.first;
	msg.range_max = _config.range.second;
	msg.angle_increment = _config.get_scan_step();
	msg.time_increment = 0;
	msg.scan_time = _config.get_scan_period_ms() / 1000.0;

	bool is_valid_frame = true;

	if (_vector_index < _lidar_data_float.size())
	{
		// check if scans fits in range
		// if not inform with ERROR
		for (float value : _lidar_data_float.at(_vector_index))
		{
			if (value < _config.range.first || value > _config.range.second)
			{
				RCLCPP_ERROR(this->get_logger(), "Value %f out of range", value);
				is_valid_frame = false;
			}
		}
		msg.ranges = _lidar_data_float.at(_vector_index);
		_vector_index++;
	}
	else if (_vector_index >= _lidar_data_float.size())
	{
		// check if scans fits in range
		// if not inform with ERROR
		_vector_index = 0;
		for (float value : _lidar_data_float.at(_vector_index))
		{
			if (value < _config.range.first || value > _config.range.second)
			{
				RCLCPP_ERROR(this->get_logger(), "Value %f out of range", value);
				is_valid_frame = false;
			}
		}
		msg.ranges = _lidar_data_float.at(_vector_index);
	}

	if (is_valid_frame)
	{
		_scan_publisher->publish(msg);
		RCLCPP_INFO(this->get_logger(), "Published scan");
	}
}

int main(int argc, char **argv)
{
	rclcpp::init(argc, argv);

	auto node = std::make_shared<FakeLidar>();
	rclcpp::spin(node);
	rclcpp::shutdown();
	return 0;
}
