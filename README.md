# MC_DataCollection

Source.cpp grabs the external depth image and color image from Azure Kinect SDK camera and the internal image from RGB web camera in real-time. The three images at the same time have the same serial number for the convenience of matching in subsequent data processing. The file should be executed under the Windows 10 OS. 

Required packages: 
OpenCV 4.0
Microsoft.Azure.Kinect.Sensor v1.1.1
