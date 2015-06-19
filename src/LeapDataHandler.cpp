#include "LeapDataHandler.h"

void LeapDataHandler::setup() {
    
    leap.open();
    fingersPositions.resize(5);
    fingersCalibration.resize(5);
    calibrated = false;
    
}

void LeapDataHandler::calibrate() {
    
    for(int i = 0; i < 5; i++) {
        fingersCalibration[i] = fingersPositions[i];
    }
    calibratedPalmPosition = palmPosition;
    calibrated = true;
    
}

void LeapDataHandler::recieveNewData() {
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-460, 100, -ofGetWidth()/2, ofGetWidth()/2);
        leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-300, 0, 2, 6);
        
        palmPosition = ofVec3f(
            simpleHands[0].handPos.x*sensitivity*MAX_SENSITIVITY,
            simpleHands[0].handPos.y*sensitivity*MAX_SENSITIVITY,
            simpleHands[0].handPos.z*sensitivity*MAX_SENSITIVITY);
        
        for(int i = 0; i < 5; i++) {
            fingersPositions[i] = ofVec3f(
                simpleHands[0].fingers[i].pos.x*sensitivity*MAX_SENSITIVITY,
                simpleHands[0].fingers[i].pos.y*sensitivity*MAX_SENSITIVITY,
                simpleHands[0].fingers[i].pos.z*sensitivity*MAX_SENSITIVITY);
        }
        
    }
    
    leap.markFrameAsOld();
    
}


void LeapDataHandler::drawLeapCalibrationMenu() {
    
    for(int i = 0; i < 5; i++) {
        
        int x = fingersPositions[i].x;
        int y = -fingersPositions[i].y;
        int z = fingersPositions[i].z;
        
        ofSetColor(255,255,0);
        ofCircle(x, y, z);
        ofDrawBitmapString(ofToString(i), x+z, y+z);
        
    }
    ofCircle(palmPosition.x, -palmPosition.y, palmPosition.z);
    ofDrawBitmapString("Palm", palmPosition.x+palmPosition.z, -palmPosition.y+palmPosition.z);
    
    if(calibrated) {
        for(int i = 0; i < 5; i++) {
            
            ofSetColor(0,255,255);
            int x =  fingersCalibration[i].x;
            int y = -fingersCalibration[i].y;
            int z = fingersCalibration[i].z;
            ofCircle(x, y, z);
            
            ofDrawBitmapString(ofToString(i), x+z, y+z);
        }
        ofCircle( calibratedPalmPosition.x,
                 -calibratedPalmPosition.y,
                  calibratedPalmPosition.z);
        ofDrawBitmapString("Palm",
                           calibratedPalmPosition.x+calibratedPalmPosition.z,
                           -calibratedPalmPosition.y+calibratedPalmPosition.z);
    }
    
    if(!calibrated) {
        
        ofSetColor(255,255,255);
        ofDrawBitmapString("Place hand above the leap controller and position fingers in a resting position.", ofGetWidth()/2-300, ofGetHeight()-100);
    }
    
}