////////////////////////////////////////////////////////////////////////////////
// SoftKinetic DepthSense SDK
//
// COPYRIGHT AND CONFIDENTIALITY NOTICE - SOFTKINETIC CONFIDENTIAL
// INFORMATION
//
// All rights reserved to SOFTKINETIC SENSORS NV (a
// company incorporated and existing under the laws of Belgium, with
// its principal place of business at Boulevard de la Plainelaan 15,
// 1050 Brussels (Belgium), registered with the Crossroads bank for
// enterprises under company number 0811 341 454 - "Softkinetic
// Sensors").
//
// The source code of the SoftKinetic DepthSense Camera Drivers is
// proprietary and confidential information of Softkinetic Sensors NV.
//
// For any question about terms and conditions, please contact:
// info@softkinetic.com Copyright (c) 2002-2012 Softkinetic Sensors NV
////////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER
#include <windows.h>
#endif

#include <stdio.h>
#include <vector>
#include <exception>

using namespace std;

/* OpenCV */
#include <opencv2\\opencv.hpp>
using namespace cv;

#include "OmniTouch.h"
using namespace DepthSense;

#define DEPTH_WIDTH		320
#define DEPTH_HEIGHT	240
#define COLOR_WIDTH		640
#define COLOR_HEIGHT	480

Context g_context;
DepthNode g_dnode;
ColorNode g_cnode;

uint32_t g_cFrames = 0;
uint32_t g_dFrames = 0;

bool g_bDeviceFound = false;

ProjectionHelper* g_pProjHelper = NULL;
StereoCameraParameters g_scp;

void binaryDepth(const DepthSense::DepthNode::NewSampleReceivedData& data){
	int32_t w, h;
	FrameFormat_toResolution(data.captureConfiguration.frameFormat, &w, &h);
	auto dp = (const int16_t *)data.depthMap;
	cv::Mat depth_image(h, w, CV_16SC1, (void*)dp);
	cv::Mat dest;
	cv::threshold(depth_image, dest, 400, 0x7fff, cv::THRESH_BINARY);
	cv::Mat fill(h, w, CV_8UC1);
	for (int i = 0; i < dest.rows; i++){
		for (int j = 0; j < dest.cols; j++){
			auto n = dest.at<int16_t>(i, j);
			if (n == 0x7fff) {
				fill.at<uchar>(i, j) = 255;
			}
			else {
				fill.at<uchar>(i, j) = 0;
			}
		}

	}
	for (int i = 0; i < w; i++){
		if (fill.at<uchar>(0, i) != 0)
			cv::floodFill(fill, cv::Point(i, 0), 0);
		if (fill.at<uchar>(h-1, i)!=0)
			cv::floodFill(fill, cv::Point(i, h-1), 0);
	}
	for (int i = 0; i < h; i++){
		if (fill.at<uchar>(i, 0) != 0)
			cv::floodFill(fill, cv::Point(0, i), 0);
		if (fill.at<uchar>(i, w-1) != 0)
			cv::floodFill(fill, cv::Point(w - 1, i), 0);
	}
	/*
	std::vector<cv::Mat> contours;
	std::vector<Vec4i> hierarchy;
	cv::findContours(fill, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	int idx = 0;
	for (; idx >= 0; idx = hierarchy[idx][0])
	{
		Scalar color(rand() & 255, rand() & 255, rand() & 255);
		drawContours(fill, contours, idx, color, CV_FILLED, 8, hierarchy);
	}*/

	IplImage toShow = fill;
	cvSaveImage("threshold.ppm", &toShow);
}

/*----------------------------------------------------------------------------*/
// New color sample event handler
void onNewColorSample(ColorNode node, ColorNode::NewSampleReceivedData data)
{
    int memsize = COLOR_WIDTH * COLOR_HEIGHT * 3 * sizeof(char); // RGB  画像バッファメモリサイズ
	unsigned char *rgbimage = (unsigned char *)malloc(memsize);
	for(int i=0; i<memsize; i++) rgbimage[i] = 0;

	Mat mat( COLOR_HEIGHT, COLOR_WIDTH, CV_8UC3, (void*)(const uint8_t*)data.colorMap );
	
	/*------ start of YUY2 -> RGB ------*/
	int i, j;
	int Y0, V0, U0, Y1;
	int R, G, B;
	int img_addr;
	unsigned int nCount = 0;

	for (i=0; i<(COLOR_HEIGHT/2); i++){ for (j=0; j<COLOR_WIDTH; j++)
	{
		img_addr = (i * COLOR_WIDTH + j) * 4;
		Y0 = mat.data[img_addr  ] & 0xff;
		V0 = mat.data[img_addr+1] & 0xff; /* Cr 色差成分(赤) */
		Y1 = mat.data[img_addr+2] & 0xff;
		U0 = mat.data[img_addr+3] & 0xff; /* Cb 色差成分(青) */

		// V0, U0 横、2 dot につき、1 つの情報となる
		R = (int)(1.164 * (Y0-16) + 1.567 * (V0-128));
		G = (int)(1.164 * (Y0-16) - 0.798 * (V0-128) - 0.384 * (U0-128));
		B = (int)(1.164 * (Y0-16) + 1.980 * (U0-128));

		if (R < 0) R= 0; if (R > 255) R= 255;
		if (G < 0) G= 0; if (G > 255) G= 255;
		if (B < 0) B= 0; if (B > 255) B= 255;

		// R, G, B をバッファーに書込む
		rgbimage[nCount++] = R;
		rgbimage[nCount++] = G;
		rgbimage[nCount++] = B;
		
		R = (int)(1.164 * (Y1-16) + 1.567 * (V0-128));
		G = (int)(1.164 * (Y1-16) - 0.798 * (V0-128) - 0.384 * (U0-128));
		B = (int)(1.164 * (Y1-16) + 1.980 * (U0-128));

		if (R < 0) R= 0;
		if (R > 255) R= 255;

		if (G < 0) G= 0;
		if (G > 255) G= 255;

		if (B < 0) B= 0;
		if (B > 255) B= 255;

		// R, G, B をバッファーに書込む
		rgbimage[nCount++] = R;
		rgbimage[nCount++] = G;
		rgbimage[nCount++] = B;
	}}
	/*------ end of YUY2 -> RGB ------*/
	
	IplImage *colorImage8U = cvCreateImage(cvSize(COLOR_WIDTH, COLOR_HEIGHT), IPL_DEPTH_8U, 3); // 画像バッファ
	memcpy(colorImage8U->imageData, rgbimage, memsize);
	cvShowImage("color", colorImage8U);
	cvReleaseImage(&colorImage8U);
	free(rgbimage);

	char c = waitKey( 1 );
	if(c == 'q') g_context.quit();
}

/*----------------------------------------------------------------------------*/
// New depth sample event handler
const int SATUATED = 32000;

void onNewDepthSample(DepthNode node, DepthNode::NewSampleReceivedData data)
{
	std::cout << "vs: " << data.vertices.size() << " depths: " << data.depthMap.size() << std::endl;
	auto ints = data.stereoCameraParameters.depthIntrinsics;
	std::cout << "cx: " << ints.cx << " cy: " << ints.cy << " height: " << ints.height << " width: " << ints.width << 
		" fx: " << ints.fx << " fy: " << ints.fy << std::endl;
	
	//detectTouch(data);
	binaryDepth(data);
    int32_t w, h;
    FrameFormat_toResolution(data.captureConfiguration.frameFormat,&w,&h);
	auto dp = (const int16_t *)data.depthMap;
	Mat mat( h, w, CV_8UC1), raw(h, w, CV_16SC1, (void*)dp);

	auto size = data.depthMap.size();
	int16_t largest = 0;
	for (int i = 0; i < size; i++)
	{
		if (dp[i] < SATUATED && dp[i] > largest) largest = dp[i];
	}
	for (int i = 0; i < size; i++)
	{
		uint8_t normalized;
		if (dp[i] >= SATUATED) normalized = 0;
		else normalized = 0xff - (dp[i] * 0xf0) / largest;
		mat.at<uint8_t>(i) = normalized;
	}
	IplImage toShow = mat;
	cvShowImage("normalized", &toShow);
	waitKey(1);
}

/*----------------------------------------------------------------------------*/
void configureDepthNode()
{
    g_dnode.newSampleReceivedEvent().connect(&onNewDepthSample);

    DepthNode::Configuration config = g_dnode.getConfiguration();
    config.frameFormat = FRAME_FORMAT_QVGA;
    config.framerate = 25;
    config.mode = DepthNode::CAMERA_MODE_CLOSE_MODE;
    config.saturation = true;

    g_dnode.setEnableVertices(true);

	g_dnode.setEnableDepthMap( true );	// enable

    try 
    {
        g_context.requestControl(g_dnode,0);

        g_dnode.setConfiguration(config);
    }
    catch (ArgumentException& e)
    {
        printf("Argument Exception: %s\n",e.what());
    }
    catch (UnauthorizedAccessException& e)
    {
        printf("Unauthorized Access Exception: %s\n",e.what());
    }
    catch (IOException& e)
    {
        printf("IO Exception: %s\n",e.what());
    }
    catch (InvalidOperationException& e)
    {
        printf("Invalid Operation Exception: %s\n",e.what());
    }
    catch (ConfigurationException& e)
    {
        printf("Configuration Exception: %s\n",e.what());
    }
    catch (StreamingException& e)
    {
        printf("Streaming Exception: %s\n",e.what());
    }
    catch (TimeoutException&)
    {
        printf("TimeoutException\n");
    }

}

/*----------------------------------------------------------------------------*/
void configureColorNode()
{
    // connect new color sample handler
    g_cnode.newSampleReceivedEvent().connect(&onNewColorSample);

    ColorNode::Configuration config = g_cnode.getConfiguration();
    config.frameFormat = FRAME_FORMAT_VGA;
	config.compression = COMPRESSION_TYPE_YUY2;
//	config.compression = COMPRESSION_TYPE_MJPEG;
    config.powerLineFrequency = POWER_LINE_FREQUENCY_50HZ;
    config.framerate = 25;

    g_cnode.setEnableColorMap(true);

    try 
    {
        g_context.requestControl(g_cnode,0);

        g_cnode.setConfiguration(config);
    }
    catch (ArgumentException& e)
    {
        printf("Argument Exception: %s\n",e.what());
    }
    catch (UnauthorizedAccessException& e)
    {
        printf("Unauthorized Access Exception: %s\n",e.what());
    }
    catch (IOException& e)
    {
        printf("IO Exception: %s\n",e.what());
    }
    catch (InvalidOperationException& e)
    {
        printf("Invalid Operation Exception: %s\n",e.what());
    }
    catch (ConfigurationException& e)
    {
        printf("Configuration Exception: %s\n",e.what());
    }
    catch (StreamingException& e)
    {
        printf("Streaming Exception: %s\n",e.what());
    }
    catch (TimeoutException&)
    {
        printf("TimeoutException\n");
    }
}

/*----------------------------------------------------------------------------*/
void configureNode(Node node)
{
    if ((node.is<DepthNode>())&&(!g_dnode.isSet()))
    {
        g_dnode = node.as<DepthNode>();
        configureDepthNode();
        g_context.registerNode(node);
    }

    if ((node.is<ColorNode>())&&(!g_cnode.isSet()))
    {
        g_cnode = node.as<ColorNode>();
        configureColorNode();
        g_context.registerNode(node);
    }
}

/*----------------------------------------------------------------------------*/
void onNodeConnected(Device device, Device::NodeAddedData data)
{
    configureNode(data.node);
}

/*----------------------------------------------------------------------------*/
void onNodeDisconnected(Device device, Device::NodeRemovedData data)
{
    if (data.node.is<ColorNode>() && (data.node.as<ColorNode>() == g_cnode))
        g_cnode.unset();
    if (data.node.is<DepthNode>() && (data.node.as<DepthNode>() == g_dnode))
        g_dnode.unset();
    printf("Node disconnected\n");
}

/*----------------------------------------------------------------------------*/
void onDeviceConnected(Context context, Context::DeviceAddedData data)
{
    if (!g_bDeviceFound)
    {
        data.device.nodeAddedEvent().connect(&onNodeConnected);
        data.device.nodeRemovedEvent().connect(&onNodeDisconnected);
        g_bDeviceFound = true;
    }
}

/*----------------------------------------------------------------------------*/
void onDeviceDisconnected(Context context, Context::DeviceRemovedData data)
{
    g_bDeviceFound = false;
    printf("Device disconnected\n");
}

/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{

    g_context = Context::create("localhost");

    g_context.deviceAddedEvent().connect(&onDeviceConnected);
    g_context.deviceRemovedEvent().connect(&onDeviceDisconnected);

    // Get the list of currently connected devices
    vector<Device> da = g_context.getDevices();

    // We are only interested in the first device
    if (da.size() >= 1)
    {
        g_bDeviceFound = true;

        da[0].nodeAddedEvent().connect(&onNodeConnected);
        da[0].nodeRemovedEvent().connect(&onNodeDisconnected);

        vector<Node> na = da[0].getNodes();
        
        printf("Found %u nodes\n",na.size());
        
        for (int n = 0; n < (int)na.size();n++)
            configureNode(na[n]);
    }

    g_context.startNodes();

    g_context.run();

    g_context.stopNodes();

    if (g_cnode.isSet()) g_context.unregisterNode(g_cnode);
    if (g_dnode.isSet()) g_context.unregisterNode(g_dnode);

    if (g_pProjHelper)
        delete g_pProjHelper;

    return 0;
}