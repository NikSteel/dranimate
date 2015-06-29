#ifndef __dranimate__LeapDataHandler__
#define __dranimate__LeapDataHandler__

#include <stdio.h>

#include "ofMain.h"
#include "ofxLeapMotion.h"
#include "ofxStrip.h"

class LeapDataHandler {
    
public:
    
    void setup();
    void update();
    void calibrate();
    void recieveNewData();
    
    void drawLeapCalibrationMenu();
    
    ofxLeapMotion leap;
    vector <ofxLeapMotionSimpleHand> simpleHands;
    
    vector<ofVec3f> palmPositions;
    vector<ofVec3f> calibratedPalmPositions;
    
    vector<ofVec3f> fingersPositions;
    vector<ofVec3f> fingersCalibration;
    vector<ofVec3f> fingersTotalMovement;
    
    vector<ofVec3f> fingersVelocities;
    
    float handRotation;
    
    int calibrationTimer;
    bool calibrated;
    
    ofFbo handFbo;
    ofEasyCam cam;
    ofLight l1;
    ofLight l2;
    ofMaterial m1;
    
    ofVec2f pointerPosition;
    
};

#endif