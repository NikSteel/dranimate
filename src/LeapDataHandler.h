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
    
    vector<ofVec3f> palmPositions;
    vector<ofVec3f> calibratedPalmPositions;
    vector<ofVec3f> fingersPositions;
    vector<ofVec3f> fingersCalibration;
    
    float handRotation;
    
    int calibrationTimer;
    
    bool calibrated;
    
};

#endif