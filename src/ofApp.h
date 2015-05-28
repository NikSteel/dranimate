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

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    
    void recieveOsc();
    void recieveLeap();
    
    void keyReleased(int key);
    void dragEvent(ofDragInfo dragInfo);
    
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    void selectClosestVertex();
    
    Puppet newPuppet;
    
    MeshGenerator mesher;
    
    // osc
    
    ofxOscReceiver receiver;
    
    // leap
    
    ofxLeapMotion leap;
	vector <ofxLeapMotionSimpleHand> simpleHands;
    
    // state
    
    enum State {
        LOAD_IMAGE,
        IMAGE_SETTINGS,
        MESH_GENERATED,
        PUPPET_STAGE
    };
    State state;
    
    // ui
    
    bool drawWireframe;
    
    int mouseX, mouseY;
    
    const float MIN_SELECT_VERT_DIST = 50.0f;
    
    int selectedVertexIndex = -1;
    
};
