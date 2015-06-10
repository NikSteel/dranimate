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

#include "Puppet.h"
#include "MeshGenerator.h"
#include "PuppetRecorder.h"

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    
    void recieveOsc();
    void recieveLeap();
    
    void keyReleased(int key);
    void mousePressed(int x, int y, int button);
    
    void dragEvent(ofDragInfo dragInfo);
    
    void selectClosestVertex();
    
    // puppet
    
    Puppet newPuppet;
    MeshGenerator mesher;
    
    bool recordingPuppet;
    PuppetRecorder puppetRecorder;
    
    // osc
    
    ofxOscReceiver receiver;
    
    // leap
    
    bool recievingLeap;
    ofxLeapMotion leap;
	vector <ofxLeapMotionSimpleHand> simpleHands;
    ofVec3f palmPosition;
    ofVec3f calibratedPalmPosition;
    vector<ofVec3f> leapFingersPositions;
    vector<ofVec3f> leapFingersCalibration;
    bool leapCalibrated;
    
    // state
    
    enum State {
        LOAD_IMAGE,
        IMAGE_SETTINGS,
        MESH_GENERATED,
        PUPPET_STAGE,
        LEAP_CALIBRATION
    };
    State state;
    
    // ui
    
    bool drawWireframe;
    
    const float MIN_SELECT_VERT_DIST = 50.0f;
    int hoveredVertexIndex = -1;
    int selectedVertexIndex = -1;
    
    enum TransformState {
        NONE,
        SCALE,
        ROTATE
    };
    TransformState transformState;
    
    ofVec2f scaleFromPoint;
    
};
