#include <iostream>
#include <chrono>
#include <k4a/k4a.hpp>
#include <opencv2/core.hpp>

class DataCollector
{
public:
	DataCollector() {}
	DataCollector(const uint32_t& deviceID, k4a_device_configuration_t& config);
	~DataCollector();
	void get_data(cv::Mat& colorMat, cv::Mat& depthMat);
	void print_calib(); 

private:
	size_t deviceID;
	k4a::device device;
	k4a::calibration calib;
	k4a::transformation trans;
	k4a::image transImg; 
	uint16_t* depthBuffer; 
	k4a_device_configuration_t config;
};

void DataCollector::print_calib() {
	std::cout << "Color Camera Intrinsics Parameters: " << std::endl; 
	std::cout << "fx = " << calib.color_camera_calibration.intrinsics.parameters.param.fx << std::endl; 
	std::cout << "fy = " << calib.color_camera_calibration.intrinsics.parameters.param.fy << std::endl;
	std::cout << "cx = " << calib.color_camera_calibration.intrinsics.parameters.param.cx << std::endl;
	std::cout << "cy = " << calib.color_camera_calibration.intrinsics.parameters.param.cy << std::endl; 
	std::cout << "Depth Camera Intrinsics Parameters: " << std::endl;
	std::cout << "fx = " << calib.depth_camera_calibration.intrinsics.parameters.param.fx << std::endl;
	std::cout << "fy = " << calib.depth_camera_calibration.intrinsics.parameters.param.fy << std::endl;
	std::cout << "cx = " << calib.depth_camera_calibration.intrinsics.parameters.param.cx << std::endl;
	std::cout << "cy = " << calib.depth_camera_calibration.intrinsics.parameters.param.cy << std::endl;
}

DataCollector::DataCollector(const uint32_t& deviceID, k4a_device_configuration_t& config) :
	deviceID(deviceID),
	config(config),
	device(k4a::device::open(deviceID)),
	calib(device.get_calibration(config.depth_mode, config.color_resolution)),
	transImg(k4a::image::create(K4A_IMAGE_FORMAT_DEPTH16, 1280, 720, 1280 * 2)),
	depthBuffer((uint16_t*)transImg.get_buffer()),
	trans(calib)
{
	if (device.is_sync_in_connected())
	{
		config.wired_sync_mode = K4A_WIRED_SYNC_MODE_SUBORDINATE;
		std::cout << "Device " << deviceID << ": suboridnate" << std::endl;
	}
	else if (device.is_sync_out_connected())
	{
		config.wired_sync_mode = K4A_WIRED_SYNC_MODE_MASTER;
		std::cout << "Device " << deviceID << ": master" << std::endl;
	}
	else
	{
		config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
		std::cout << "Device " << deviceID << ": standalone" << std::endl;
	}
	device.start_cameras(&config);
}

void DataCollector::get_data(cv::Mat& colorMat, cv::Mat& depthMat)
{
	k4a::capture cap;
	if (!device.get_capture(&cap, std::chrono::milliseconds(10000)))
	{
		std::cout << "Image capture failed!" << std::endl;
		exit(EXIT_FAILURE);
	}
	k4a::image colorImg = cap.get_color_image();
	k4a::image depthImg = cap.get_depth_image();
	trans.depth_image_to_color_camera(depthImg, &transImg);
	uint8_t* colorBuffer = colorImg.get_buffer();
	std::memcpy(colorMat.data, colorBuffer, colorImg.get_size());
	std::memcpy(depthMat.data, depthBuffer, transImg.get_size());
}

DataCollector::~DataCollector()
{
	device.stop_cameras();
	device.close();
}
