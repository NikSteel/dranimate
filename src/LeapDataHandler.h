#ifndef __dranimate__LeapDataHandler__
#define __dranimate__LeapDataHandler__

#include <stdio.h>

#include "ofMain.h"
#include "ofxLeapMotion.h"
#include "ofxStrip.h"
#include "ofxLeapGestures.h"

#include "Utils.h"

class LeapDataHandler : public ofxLeapMotion {
    
private:
    
    vector<ofVec3f> palmPositions;
    vector<ofVec3f> calibratedPalmPositions;
    vector<ofVec3f> palmVelocities;
    
    vector<ofVec3f> fingersPositions;
    vector<ofVec3f> fingersCalibration;
    vector<ofVec3f> fingersScreenPositions;
    vector<ofVec3f> fingersTotalMovement;
    
    vector<ofVec3f> fingersVelocities;
    
    ofFbo handFbo;
    ofEasyCam cam;
    ofLight l1;
    ofLight l2;
    ofMaterial m1;
    
public:
    
    void setup();
    void update();
    void draw(bool drawCalibration);
    
    void calibrate();
    void recieveNewData();
    
    ofVec3f getFingerPosition(int i);
    ofVec3f getCalibratedFingerPosition(int i);
    ofVec3f getFingerScreenPosition(int i);
    ofVec3f getCalibratedPalmPosition(int i);
    ofVec3f getFingerVelocity(int i);
    
    int getHandCount();
    
    int calibrationSecondsLeft();
    
    bool fingerFlicked(int i);
    
    void swapHandControls();
    
    float handRotation;
    
    int calibrationTimer;
    int calibrationTimerLength = 60*3;
    bool calibrated;
    
    bool renderHands;
    
    int pointingFinger = 5;
    int puppetFinger = 0;
    
    ofxLeapGestures leapGestures;
    
};

#endif