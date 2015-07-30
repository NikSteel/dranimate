#pragma once

#include <stdio.h>

#include "ofMain.h"
#include "ofxLeapMotion.h"

#include "Utils.h"

class LeapDataHandler : public ofxLeapMotion {
    
private:
    
    void recieveNewData();
    
    vector<ofVec3f> palmPositions;
    vector<ofVec3f> calibratedPalmPositions;
    vector<ofVec3f> palmVelocities;
    
    vector<float> palmRotations;
    
    vector<ofVec3f> fingersPositions;
    vector<ofVec3f> fingersCalibration;
    vector<ofVec3f> fingersScreenPositions;
    vector<ofVec3f> fingersTotalMovement;
    
    vector<ofVec3f> fingersVelocities;
    
    ofFbo handFbo;
    ofEasyCam cam;
    ofLight light1;
    ofLight light2;
    ofMaterial material;
    
public:
    
    void setup();
    void update();
    void draw(bool drawCalibration);
    
    void calibrate();
    
    ofVec3f getFingerPosition(int i);
    ofVec3f getCalibratedFingerPosition(int i);
    ofVec3f getFingerScreenPosition(int i);
    ofVec3f getCalibratedPalmPosition(int i);
    ofVec3f getFingerVelocity(int i);
    float getPalmRotation(int i);
    
    int getHandCount();
    
    int calibrationSecondsLeft();
    
    bool fingerFlicked(int i);
    
    void swapHandControls();
    
    int calibrationTimer;
    int calibrationTimerLength = 60*3;
    bool calibrated;
    
    int pointingFinger = 5;
    int puppetFinger = 0;
    
    ofTrueTypeFont font;
    
};
