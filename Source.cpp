#include <iostream>
#include <sstream>
#include <sys/stat.h> 
#include <k4a/k4a.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "datacollector.h"
#include <direct.h>
#include <Windows.h>

using namespace std;
using namespace k4a;
using namespace cv;
// hhh


class CapOneImg
{
public:
    CapOneImg(const int32_t& deviceID,
        k4a_device_configuration_t& config, VideoCapture& aWebcam,
        const string& outPath) :
        dc(deviceID, config), webcam(aWebcam), 
        outPath(outPath)
    {}
    ~CapOneImg() {}
    void capOneImg(int i)
    {
        Mat colorMat(720, 1280, CV_8UC4);
        Mat depthMat(720, 1280, CV_16UC1);
        Mat rgbdImage;

        dc.get_data(colorMat, depthMat);
        webcam >> rgbdImage; 

        stringstream colorName, depthName, rgbdName;
        rgbdName << outPath << "/rgbd/" << i << ".png";
        colorName << outPath << "/color/" << i << ".png";
        depthName << outPath << "/depth/" << i << ".png";

        imwrite(colorName.str(), colorMat);
        imwrite(depthName.str(), depthMat);
        imwrite(rgbdName.str(), rgbdImage);

        cout << i << " collected!" << endl;
    }
private:
    DataCollector dc;
    VideoCapture webcam; 
    string outPath;
};



int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "record_external [outputpath] [number of images]" << endl;
        return 1;
    }

    string outPath(argv[1]);
    int n = atoi(argv[2]);

    stringstream ssColor, ssDepth, ssRgbd;
    ssColor << outPath << "/color";
    ssDepth << outPath << "/depth";
    ssRgbd << outPath << "/rgbd";


    if (_mkdir(outPath.c_str()) == -1 ||
        _mkdir(ssColor.str().c_str()) == -1 ||
        _mkdir(ssDepth.str().c_str()) == -1 || _mkdir(ssRgbd.str().c_str()) == -1)
    {
        cout << "cannot create " << outPath << "!" << endl;
        cout << "it has already existed" << endl; 
    }


    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.camera_fps = K4A_FRAMES_PER_SECOND_30;
    config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution = K4A_COLOR_RESOLUTION_720P
        ;
    config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    config.synchronized_images_only = true;
    

    VideoCapture webcam(0);
    if (!webcam.isOpened()) {
        cerr << "ERROR: Could not open camera" << endl;
        return -1;
    }

    
    CapOneImg cap(K4A_DEVICE_DEFAULT, config, webcam, outPath);
    

    for (int i = 0; i < n; i++)
    {
        cap.capOneImg(i);
        Sleep(1000); 
    }
    return 0;
}

