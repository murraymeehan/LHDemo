# pragma once

#ifndef _TEST_APP
#define _TEST_APP

//#define USE_XML_NODE_CONFIG // Uncomment this to use an XML config file to select Kinect's OpenNI node. Otherwise it is hardcoded.
//#define USE_IR // Uncomment this to use infra red instead of RGB cam...

#include "ofxOpenNI.h"
#include "ofMain.h"

#include <cstdio>
#include <string>

#include "ofxOpenCv.h"

class LHDemoApp : public ofBaseApp{

public:


	void setup();
	void update();
	void draw();

	void keyPressed  (int key);

	void	setupRecording(string _filename = "");

	string	generateFileName(string ending = ".oni");

	bool				isLive, isTracking, isRecording, isCloud, isCPBkgnd, isMasking;
	bool				isTrackingHands, isFiltering;

	ofxOpenNIContext	recordContext, playContext;
	ofxDepthGenerator	recordDepth, playDepth;

#ifdef USE_IR
	ofxIRGenerator		recordImage, playImage;
#else
	ofxImageGenerator	recordImage, playImage;
#endif

	ofxHandGenerator	recordHandTracker, playHandTracker;

	ofxUserGenerator	recordUser, playUser;
	ofxOpenNIRecorder	oniRecorder;

	int			nearThreshold, farThreshold;

	ofImage			allUserMasks, user1Mask, user2Mask, depthRangeMask;

	void                   detectStrum();

	ofImage                depthImage;
	ofImage                RGBImage;
//	xn::ImageMetaData            imd;
        ofstream                logFile, depthImageLog, RGBImageLog;
};

#endif
