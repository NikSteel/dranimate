#include "LeapDataHandler.h"

// public methods

void LeapDataHandler::setup() {
    
    open();
    
    fingersPositions.resize(10);
    fingersCalibration.resize(10);
    fingersScreenPositions.resize(10);
    fingersTotalMovement.resize(10);
    
    fingersVelocities.resize(10);
    
    palmPositions.resize(2);
    calibratedPalmPositions.resize(2);
    palmVelocities.resize(2);
    palmRotations.resize(2);
    
    calibrated = false;
    calibrationTimer = 0;
    
    
    l1.setPosition(200, 300, 50);
    l2.setPosition(-200, -200, 50);
    
    cam.setOrientation(ofPoint(-20, 0, 0));
    
    leapGestures.setup();
    leapGestures.setClassifyMode();
    
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
void LeapDataHandler::draw(bool drawCalibration) {
    
    if(!calibrated) {
        Utils::drawWarning("Leap not calibrated! Press 'c' to calibrate");
    }
    
    if(getLeapHands().size() == 2) {
        
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE);
        
        cam.begin();
        
        ofEnableLighting();
        l1.enable();
        l2.enable();
        
        m1.begin();
        m1.setShininess(0.6);
        
        l2.disable();
        
        vector<ofxLeapMotionSimpleHand> simpleHands = getSimpleHands();
        
        for(int i = 0; i < simpleHands.size(); i++){
            
            if(i < getLeapHands().size()) {
                bool isRightHand = getLeapHands()[i].isRight();
                bool isPointerHand = (isRightHand && pointingFinger == 0) ||
                (!isRightHand && pointingFinger == 5);
                
                ofPushStyle();
                
                ofSetColor(190);
                ofSetLineWidth(5);
                
                ofEnableLighting();
                
                // fingers
                
                for(int f = 0; f < simpleHands[i].fingers.size(); f++){
                    
                    ofSetColor(255);
                    ofLine(simpleHands[i].fingers[f].base, simpleHands[i].fingers[f].pos);
                    
                    // finger base
                    ofSetColor(150,150,150,50);
                    ofDrawSphere(simpleHands[i].fingers[f].base, 10);
                    
                    ofSetColor(255);
                    ofLine(simpleHands[i].handPos,         simpleHands[i].fingers[f].base);
                    
                }
                
                if(!isPointerHand) {
                    
                    for(int f = 0; f < simpleHands[i].fingers.size(); f++){
                        
                        // fingertip
                        ofSetColor(0, 50, 200);
                        ofDrawSphere(simpleHands[i].fingers[f].pos, 10);
                        
                    }
                    
                } else {
                    
                    // fingertip
                    ofSetColor(200, 100, 0);
                    
                    // make the pointer finger cone point in the right direction
                    ofVec3f pos = simpleHands[i].fingers[1].pos;
                    ofVec3f base = simpleHands[i].fingers[1].base;
                    ofQuaternion q;
                    q.makeRotate(ofPoint(0, -1, 0), pos-base);
                    ofMatrix4x4 m;
                    q.get(m);
                    
                    ofPushMatrix();
                    ofTranslate(pos.x, pos.y, pos.z);
                    glMultMatrixf(m.getPtr());
                    ofDrawCone(0,0,0,10,20);
                    ofPopMatrix();
                    
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
                
                // calibration
                
                if(drawCalibration) {
                    for(int f = 0; f < 10; f++){
                        
                        ofSetColor(155,155,255,155);
                        ofDrawSphere(fingersCalibration[f], 10);
                        
                    }
                }
                
            }
            
        }
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_NORMALIZE);
        
        m1.end();
        cam.end();
        
        
        if(drawCalibration) {
            
            Utils::drawControls("c   -   Set calibration\nt   -   Calibrate on timer\nb   -   Back to puppet stage");
            
            if(calibrationTimer != 0) {
                string timeLeftString = ofToString(calibrationSecondsLeft());
                ofSetColor(0,0,0);
                Resources::verdana54.drawString(timeLeftString, ofGetWidth()/2, ofGetHeight()/2);
            }
            
        }
        
    }
    
}

void LeapDataHandler::calibrate() {
    
    for(int i = 0; i < 10; i++) {
        fingersCalibration[i] = fingersPositions[i];
    }
    
    calibratedPalmPositions[0] = palmPositions[0];
    calibratedPalmPositions[1] = palmPositions[1];
    
    calibrated = true;
    
}

ofVec3f LeapDataHandler::getFingerPosition(int i) {
    
    return fingersPositions[i];
    
}
ofVec3f LeapDataHandler::getCalibratedFingerPosition(int i) {
    
    return fingersPositions[i]-fingersCalibration[i];
    
}
ofVec3f LeapDataHandler::getFingerScreenPosition(int i) {
    
    return fingersScreenPositions[i];
    
}
ofVec3f LeapDataHandler::getCalibratedPalmPosition(int i) {
    
    return palmPositions[i]-calibratedPalmPositions[i];
    
}
ofVec3f LeapDataHandler::getFingerVelocity(int i) {
    
    ofVec3f palmVel;
    if(i < 5) {
        palmVel = palmVelocities[0];
    } else {
        palmVel = palmVelocities[1];
    }
    
    return fingersVelocities[i] - palmVel;
    
}
float LeapDataHandler::getPalmRotation(int i) {
    
    return palmRotations[i];
    
}

int LeapDataHandler::getHandCount() {
    
    return getLeapHands().size();
    
}

bool LeapDataHandler::fingerFlicked(int finger) {
    
    ofVec3f avgVelocity = ofVec3f(0,0,0);
    if(finger < 5) {
        for(int i = 0; i < 5; i++) {
            if(i != finger) avgVelocity += getFingerVelocity(i);
        }
    } else {
        for(int i = 5; i < 10; i++) {
            if(i != finger) avgVelocity += getFingerVelocity(i);
        }
    }
    avgVelocity /= 4;
    
    float fingerVel = abs(getFingerVelocity(finger).y)+abs(getFingerVelocity(finger).x);
    float avgVel = abs(avgVelocity.y)+abs(avgVelocity.x);
    
    return fingerVel - avgVel > 1400 && fingerVel > 1500;
    
}

int LeapDataHandler::calibrationSecondsLeft() {
    
    return calibrationTimer/60+1;
    
}

void LeapDataHandler::swapHandControls() {
    
    int temp = pointingFinger;
    pointingFinger = puppetFinger;
    puppetFinger = temp;
    
}

// private methods

void LeapDataHandler::recieveNewData() {
    
    vector<ofxLeapMotionSimpleHand> simpleHands = getSimpleHands();
    
    bool rightHandOnScreen = false;
    bool leftHandOnScreen = false;
    
    if( isFrameNew() && simpleHands.size() && getLeapHands().size() ){
        
        // disabled because we need to put this on its own thread beacuse it's laggin everything when we classify the gesture
        //leapGestures.recieveNewLeapData(leapHandler.getLeapHands());
        
        // original
        setMappingX(-460, 100, -ofGetWidth()/2, ofGetWidth()/2);
        setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        setMappingZ(-300, 0, 2, 6);
        
        // from example
        setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
        setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        setMappingZ(-150, 150, -200, 200);
        
        
        for(int h = 0; h < simpleHands.size(); h++) {
            
            ofxLeapMotionSimpleHand hand = simpleHands[h];
            Hand handWithAllData = getLeapHands()[h];
            
            // the right hand is always at index 0, the left is always at 1
            int handIndex = -1;
            if(handWithAllData.isRight()) {
                handIndex = 0;
                rightHandOnScreen = true;
            } else if(handWithAllData.isLeft()) {
                handIndex = 1;
                leftHandOnScreen = true;
            }
            
            // set the 'pointer' position to the left hand's index finger
            for(int i = 0; i < 5; i++) {
                ofVec3f wp = cam.worldToScreen(simpleHands[h].fingers[i].pos);
                wp -= ofVec3f(ofGetWidth()/2,ofGetHeight()/2);
                fingersScreenPositions[i+handIndex*5] = ofVec2f(wp.x,wp.y);
            }
            
            // get palm position
            palmPositions[handIndex] = ofVec3f(hand.handPos.x,
                                               hand.handPos.y,
                                               hand.handPos.z);
            
            // note: rotation disabled for now
            //palmRotations[handIndex] = hand.handNormal.x;
            
            // get fingers positions
            for(int i = 0; i < 5; i++) {
                
                fingersPositions[i+handIndex*5] = ofVec3f(hand.fingers[i].pos.x-hand.handPos.x,
                                                          hand.fingers[i].pos.y-hand.handPos.y,
                                                          hand.fingers[i].pos.z-hand.handPos.z);
                
                fingersVelocities[i+handIndex*5] =ofVec3f(hand.fingers[i].vel.x,
                                                          hand.fingers[i].vel.y,
                                                          hand.fingers[i].vel.z);
                
            }
            
        }
        
    }
    
    if(!rightHandOnScreen) {
        for(int i = 0; i < 5; i++) {
            fingersVelocities[i] = ofVec3f(0,0,0);
        }
    }
    if(!leftHandOnScreen) {
        for(int i = 0; i < 5; i++) {
            fingersVelocities[i+5] = ofVec3f(0,0,0);
        }
    }
    
    markFrameAsOld();
    
}