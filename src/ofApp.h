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
#include "ofxLeapGestures.h"

#include "Puppet.h"
#include "PuppetsHandler.h"
#include "MeshGenerator.h"
#include "PuppetRecorder.h"
#include "LeapDataHandler.h"
#include "Utils.h"
#include "ImageFromCamera.h"
#include "Resources.h"

class ofApp : public ofBaseApp {
    
public:
    
    void setup();
    void update();
    void draw();
    
    void keyReleased(int key);
    
    void mousePressed(int x, int y, int button);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    
    void dragEvent(ofDragInfo dragInfo);
    
    void cmdEvent(ofxCDMEvent &ev);
    void updateClickDownMenu();
    
// all the handlers and stuff
    
    MeshGenerator mesher;
    PuppetsHandler puppetsHandler;
    LeapDataHandler leapHandler;
    ofxClickDownMenu clickDownMenu;
    ImageFromCamera cam;
    ofxOscReceiver oscReceiver;
    
    // UI & State
    
    enum State {
        NEW_PUPPET_CREATION,
        PUPPET_STAGE,
        LEAP_CALIBRATION
    };
    State state;
    
};
