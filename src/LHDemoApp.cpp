#include "LHDemoApp.h"
#include <fstream>

using namespace std;

void LHDemoApp::setup() {

	isRecording		= false;

	nearThreshold = 0;
	farThreshold  = 5000;

	setupRecording();
	ofBackground(0, 0, 0);

	logDepthImage.open ("depth_video.m");
	logDepthImage << "depth = [\n";
	logRGBImage.open("rgb_video.m");
        logRGBImage << "rgb = [\n";

}

void LHDemoApp::setupRecording(string _filename) {

#ifdef USE_XML_NODE_CONFIG
	recordContext.setupUsingXMLFile();
#else
	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
#endif
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);

	recordUser.setup(&recordContext);
	recordUser.setMaxNumberOfUsers(2);// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)

	recordHandTracker.setup(&recordContext, 4);

	recordContext.toggleRegisterViewport();

	oniRecorder.setup(&recordContext, ONI_STREAMING);

}

//--------------------------------------------------------------
void LHDemoApp::update(){

    // update all nodes
    recordContext.update();
    recordDepth.update();
    recordImage.update();

    // Print a depth image per frame (working, super-inefficient)
//    for (int x=0;x<recordDepth.getHeight();x++) {
//          for (int y=0;y<recordDepth.getWidth();y++) {
//            logDepthImage << recordDepth.getPixelDepth(x,y) << " ";
//          }
//          logDepthImage << endl;
//        }
//    logDepthImage << endl;


    // Print an RGB image per frame
    unsigned char* RGBPixelArray = recordImage.getPixels();
    cout << RGBPixelArray[20,30];
//    ofImage RGBImage;

    // method 1, invalid syntax
//    RGBImage.ofImage_(RGBPixelArray);
    // method 2, no errors but spits out an image of zeros.
    RGBImage.setFromPixels(RGBPixelArray,
                    recordDepth.getWidth(),
                    recordDepth.getHeight(),
                    OF_IMAGE_GRAYSCALE);
    // method 3, invalid syntax
//    RGBImage.setFromPixels(RGBPixelArray);

    RGBImage.saveImage("RGBImage.png");
    sleep(1); // delay to avoid making too much data to process

    // All 3 of these spit out zero-filled images
//    RGBImage.setFromPixels(recordImage.getPixels(),
//        recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);
//    RGBImage.saveImage("RGBImage.png");
//
//    depthRangeMask.setFromPixels(recordDepth.getDepthPixels(nearThreshold, farThreshold),
//        recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);
//    depthRangeMask.saveImage("depthRangeMask.png");
//
//    depthImage.allocate(recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);
//    depthImage.setFromPixels(recordDepth.getDepthPixels(0,10000),
//        recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);
//    depthImage.saveImage("depthImage.png");

    // update tracking/recording nodes
    recordUser.update();
    if (isRecording)
        oniRecorder.update();

}

//--------------------------------------------------------------
void LHDemoApp::detectStrum(){

    if (recordHandTracker.getNumTrackedHands() > 0) {

        // if there are 1 or more hands detected, this is the first hand
        ofxTrackedHand MurrayHand = *recordHandTracker.getHand(0);

        float x = MurrayHand.projectPos.x;
        float y = MurrayHand.projectPos.y;
        float z = MurrayHand.projectPos.z;
        int t = ofGetElapsedTimeMillis();
        printf(" %f , %f , %f , %i \n,",x,y,z,t);

//        Write values to text file
        if (isRecording) {
            logHandPosition << x << " , " << y << " , " << z << " , " << t << " \n";
        }
    }
}

//--------------------------------------------------------------
void LHDemoApp::draw(){

	ofSetColor(255, 255, 255);
	glPushMatrix();
	glScalef(0.75, 0.75, 0.75);

    recordDepth.draw(0,0,640,480);
    recordImage.draw(640, 0, 640, 480);

//    depthRangeMask.draw(0, 480, 320, 240);	// can use this with openCV to make masks, find contours etc when not dealing with openNI 'User' like objects

//    recordUser.draw(); // uncomment this to draw a silly stickman skeleton
    recordHandTracker.drawHands();
    detectStrum();

	glPopMatrix();
	ofSetColor(255, 255, 0);

	string statusRec		= (string)(!isRecording ? "READY" : "RECORDING");
	stringstream msg;

	msg
	<< "    s : start/stop recording  : " << statusRec << endl
	<< endl
	<< "File  : " << oniRecorder.getCurrentFileName() << endl;

	ofDrawBitmapString(msg.str(), 20, 560);

}

//--------------------------------------------------------------
void LHDemoApp::keyPressed(int key){

	switch (key) {
		case 's':
		case 'S':
			if (isRecording) {
				oniRecorder.stopRecord();
				isRecording = false;
				logHandPosition << "\n ]; \n";
				logHandPosition.close();
				break;
			} else {
				oniRecorder.startRecord(generateFileName());
				isRecording = true;
//                                logHandPosition.open(generateFileName(".txt"));
                                logHandPosition.open("log.m");
                                logHandPosition << "data = [\n";
				break;
			}
			break;
		case 'q':
		default:
            if (isRecording) {
				oniRecorder.stopRecord();
            }
            logHandPosition << "\n ]; \n";
            logHandPosition.close();
            logDepthImage << "\n ]; \n";
            logDepthImage.close();
            logRGBImage << "\n ]; \n";
            logRGBImage.close();

            break;
	}
}


string LHDemoApp::generateFileName(string suffix) {

	string _root = "kinectRecord";

	string _timestamp = ofToString(ofGetDay()) +
	ofToString(ofGetMonth()) +
	ofToString(ofGetYear()) +
	ofToString(ofGetHours()) +
	ofToString(ofGetMinutes()) +
	ofToString(ofGetSeconds());

//	string _filename = (_root + _timestamp + ".oni");
	string _filename = (_root + _timestamp + suffix);

	return _filename;

}
