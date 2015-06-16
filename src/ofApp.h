#pragma once

#include "ofMain.h"

#include "ofxPuppetInteractive.h"
#include "ofxButterfly.h"
#include "ofxTriangleMesh.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxOscReceiver.h"
#include "ofxXmlSettings.h"
#include "ofxLeapMotion.h"
#include "ofxClickDownMenu.h"

#include "Puppet.h"
#include "MeshGenerator.h"
#include "PuppetRecorder.h"
#include "Utils.h"

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    
    void recieveOsc();
    void recieveLeap();
    
    int getClosestPuppetIndex();
    Puppet *currentPuppet();
    
    string getSelectedVertexInfo();
    
    void keyReleased(int key);
    void mousePressed(int x, int y, int button);
    void dragEvent(ofDragInfo dragInfo);
    
    // puppets
    
    vector<Puppet> puppets;
    
    // puppet generation
    Puppet newPuppet;
    MeshGenerator mesher;
    
    bool recordingPuppet;
    PuppetRecorder puppetRecorder;
    vector<PuppetRecorder> recordedPuppets;
    
    // osc
    
    ofxOscReceiver receiver;
    
    // leap
    
    const float LEAP_SENSITIVITY = 1.5;
    
    ofxLeapMotion leap;
	vector <ofxLeapMotionSimpleHand> simpleHands;
    ofVec3f palmPosition;
    ofVec3f calibratedPalmPosition;
    vector<ofVec3f> leapFingersPositions;
    vector<ofVec3f> leapFingersCalibration;
    bool leapCalibrated;
    
    // state
    
    enum State {
        NEW_PUPPET_CREATION,
        PUPPET_STAGE,
        LEAP_CALIBRATION
    };
    State state;
    
    // ui
    
    bool wholeScenePaused = false;
    
    float backgroundBrightness;
    
    int hoveredVertexIndex;
    int selectedVertexIndex;
    int selectedPuppetIndex;
    
    // puppet animation recording & exporting
    
    ofFbo recorder;
    
    // clickdown menu
    
    void cmdEvent(ofxCDMEvent &ev);
    void updateClickDownMenu();
    ofxClickDownMenu clickDownMenu;
    
};
