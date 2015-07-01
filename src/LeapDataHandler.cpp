#include "LeapDataHandler.h"

void LeapDataHandler::setup() {
    
    leap.open();
    
    fingersPositions.resize(10);
    fingersCalibration.resize(10);
    fingersTotalMovement.resize(10);
    
    fingersVelocities.resize(10);
    
    palmPositions.resize(2);
    calibratedPalmPositions.resize(2);
    
    calibrated = false;
    calibrationTimer = 0;
    
    
    l1.setPosition(200, 300, 50);
    l2.setPosition(-200, -200, 50);
    
    cam.setOrientation(ofPoint(0, 0, 0));
    
    
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
        
        // original
        leap.setMappingX(-460, 100, -ofGetWidth()/2, ofGetWidth()/2);
        leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-300, 0, 2, 6);
        
        // from example
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
        leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
        
        
        for(int h = 0; h < simpleHands.size(); h++) {
            
            ofxLeapMotionSimpleHand hand = simpleHands[h];
            
            palmPositions[h] = ofVec3f(hand.handPos.x,
                                       hand.handPos.y,
                                       hand.handPos.z);
            
            for(int i = 0; i < 5; i++) {
                
                fingersPositions[i+h*5] = ofVec3f(hand.fingers[i].pos.x,
                                                  hand.fingers[i].pos.y,
                                                  hand.fingers[i].pos.z);
                
                fingersVelocities[i+h*5] =ofVec3f(hand.fingers[i].vel.x,
                                                  hand.fingers[i].vel.y,
                                                  hand.fingers[i].vel.z);
                
            }
            
        }
        
    }
    
    leap.markFrameAsOld();
    
}


void LeapDataHandler::drawLeapCalibrationMenu() {
    
    if(leap.getLeapHands().size() > 0) {
        
    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    
    
    ofDisableLighting();
    //ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);
    
    ofSetColor(200);
    //ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(leap.isConnected()), 20, 20);
        
        cam.setPosition(ofPoint(0,0,1000));
        
    cam.begin();
    
    ofPushMatrix();
    ofRotate(90, 0, 0, 1);
    ofSetColor(20);
    //ofDrawGridPlane(800, 20, false);
    ofPopMatrix();
    
    ofEnableLighting();
    l1.enable();
    l2.enable();
    
    m1.begin();
    m1.setShininess(0.6);
    
        
    //cam.move(0, 0, 100);
    
    l2.disable();
    
    for(int i = 0; i < simpleHands.size(); i++){
        
        ofPushStyle();
        
        ofSetColor(190);
        ofSetLineWidth(5);
        
        ofEnableLighting();
        
        // fingers
        
        for(int f = 0; f < simpleHands[i].fingers.size(); f++){
            
            // fingertip
            ofSetColor(0, 200, 0);
            ofDrawSphere(simpleHands[i].fingers[f].pos, 10);
            
            ofSetColor(100, 100, 100);
            ofLine(simpleHands[i].fingers[f].base, simpleHands[i].fingers[f].pos);
            
            // finger base
            ofDrawBox(simpleHands[i].fingers[f].base, 20);
            
            ofSetColor(100, 100, 100);
            ofLine(simpleHands[i].handPos,         simpleHands[i].fingers[f].base);
            
        }
        
        // palm
        
        ofPushMatrix();
        ofTranslate(simpleHands[i].handPos);
        //rotate the hand by the downwards normal
        ofQuaternion q;
        q.makeRotate(ofPoint(0, -1, 0), simpleHands[i].handNormal);
        ofMatrix4x4 m;
        q.get(m);
        glMultMatrixf(m.getPtr());
        ofScale(1, 0.35, 1.0);
        ofDrawBox(0, 0, 0, 60);
        ofPopMatrix();
        
        
        ofDisableLighting();
        
        ofPopStyle();

        

    }
    
    
    
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_NORMALIZE);
    
    
    m1.end();
        //cam.move(0, 0, -100);
    cam.end();
    
    for(int i = 0; i < simpleHands.size(); i++){
        
        glDisable(GL_DEPTH_TEST);
        ofVec3f wp = cam.worldToScreen(simpleHands[i].fingers[1].pos);
        ofSetColor(255, 100, 0);
        //ofCircle(wp.x, wp.y, 10);
        pointerPosition = ofVec2f(wp.x,wp.y);
        
        
    }
    }
    
    
}