#include "LeapDataHandler.h"

void LeapDataHandler::setup() {
    
    leap.open();
    
    fingersPositions.resize(10);
    fingersCalibration.resize(10);
    
    palmPositions.resize(2);
    calibratedPalmPositions.resize(2);
    
    calibrated = false;
    calibrationTimer = 0;
    
}

void LeapDataHandler::calibrate() {
    
    for(int i = 0; i < 10; i++) {
        fingersCalibration[i] = fingersPositions[i];
    }
    
    calibratedPalmPositions[0] = palmPositions[0];
    calibratedPalmPositions[1] = palmPositions[1];
    
    calibrated = true;
    
}

void LeapDataHandler::update() {
    
    recieveNewData();
    
    if(calibrationTimer > 0) {
        
        calibrationTimer--;
        
        if(calibrationTimer == 0) {
            calibrate();
        }
        
    }
    
}

void LeapDataHandler::recieveNewData() {
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
        
        handRotation = simpleHands[0].handNormal.x;
        
        leap.setMappingX(-460, 100, -ofGetWidth()/2, ofGetWidth()/2);
        leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-300, 0, 2, 6);
        
        palmPositions[0] = ofVec3f(
            simpleHands[0].handPos.x*sensitivity*MAX_SENSITIVITY,
            simpleHands[0].handPos.y*sensitivity*MAX_SENSITIVITY,
            simpleHands[0].handPos.z*sensitivity*MAX_SENSITIVITY);
        
        for(int i = 0; i < 5; i++) {
            fingersPositions[i] = ofVec3f(
                simpleHands[0].fingers[i].pos.x*sensitivity*MAX_SENSITIVITY,
                simpleHands[0].fingers[i].pos.y*sensitivity*MAX_SENSITIVITY,
                simpleHands[0].fingers[i].pos.z*sensitivity*MAX_SENSITIVITY);
        }
        if(simpleHands.size() > 1) {
            for(int i = 0; i < 5; i++) {
                fingersPositions[i+5] = ofVec3f(
                    simpleHands[1].fingers[i].pos.x*sensitivity*MAX_SENSITIVITY,
                    simpleHands[1].fingers[i].pos.y*sensitivity*MAX_SENSITIVITY,
                    simpleHands[1].fingers[i].pos.z*sensitivity*MAX_SENSITIVITY);
                
                palmPositions[1] = ofVec3f(
                                           simpleHands[1].handPos.x*sensitivity*MAX_SENSITIVITY,
                                           simpleHands[1].handPos.y*sensitivity*MAX_SENSITIVITY,
                                           simpleHands[1].handPos.z*sensitivity*MAX_SENSITIVITY);
            }
        }
        
    }
    
    leap.markFrameAsOld();
    
}


void LeapDataHandler::drawLeapCalibrationMenu() {
    
    for(int i = 0; i < 10; i++) {
        
        int x = fingersPositions[i].x;
        int y = -fingersPositions[i].y;
        int z = fingersPositions[i].z;
        
        ofSetColor(255,255,0);
        ofCircle(x, y, z);
        ofDrawBitmapString(ofToString(i), x+z, y+z);
        
    }
    
    ofCircle(palmPositions[0].x, -palmPositions[0].y, palmPositions[0].z);
    ofDrawBitmapString("Palm 0", palmPositions[0].x+palmPositions[0].z, -palmPositions[0].y+palmPositions[0].z);
    
    ofCircle(palmPositions[1].x, -palmPositions[1].y, palmPositions[1].z);
    ofDrawBitmapString("Palm 1", palmPositions[1].x+palmPositions[1].z, -palmPositions[1].y+palmPositions[1].z);
    
    if(calibrated) {
        for(int i = 0; i < 10; i++) {
            
            ofSetColor(0,255,255);
            int x =  fingersCalibration[i].x;
            int y = -fingersCalibration[i].y;
            int z = fingersCalibration[i].z;
            ofCircle(x, y, z);
            
            ofDrawBitmapString(ofToString(i), x+z, y+z);
        }
        ofCircle( calibratedPalmPositions[0].x,
                 -calibratedPalmPositions[0].y,
                  calibratedPalmPositions[0].z);
        ofCircle( calibratedPalmPositions[1].x,
                 -calibratedPalmPositions[1].y,
                  calibratedPalmPositions[1].z);
        ofDrawBitmapString("Palm 0",
                           calibratedPalmPositions[0].x+calibratedPalmPositions[0].z,
                           -calibratedPalmPositions[0].y+calibratedPalmPositions[0].z);
        ofDrawBitmapString("Palm 1",
                           calibratedPalmPositions[1].x+calibratedPalmPositions[1].z,
                           -calibratedPalmPositions[1].y+calibratedPalmPositions[1].z);
    }
    
    if(!calibrated) {
        
        ofSetColor(255,255,255);
        ofDrawBitmapString("Place hand above the leap controller and position fingers in a resting position.", ofGetWidth()/2-300, ofGetHeight()-100);
    }
    
}