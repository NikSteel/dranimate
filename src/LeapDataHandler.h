#ifndef __dranimate__LeapDataHandler__
#define __dranimate__LeapDataHandler__

#include <stdio.h>

#include "ofMain.h"
#include "ofxLeapMotion.h"

class LeapDataHandler {
    
public:
    
    void setup();
    void update();
    void calibrate();
    void recieveNewData();
    
    void drawLeapCalibrationMenu();
    
    const float MAX_SENSITIVITY = 3.5;
    float sensitivity = 0.5;
    
    ofxLeapMotion leap;
    vector <ofxLeapMotionSimpleHand> simpleHands;
    
    ofVec3f palmPosition;
    ofVec3f calibratedPalmPosition;
    vector<ofVec3f> fingersPositions;
    vector<ofVec3f> fingersCalibration;
    
    int calibrationTimer;
    
    bool calibrated;
    
};

#endif