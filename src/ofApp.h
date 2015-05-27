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

#include "Puppet.h"
#include "MeshGenerator.h"

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    
    void recieveOsc();
    void keyReleased(int key);
    void dragEvent(ofDragInfo dragInfo);
    
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    Puppet newPuppet;
    
    MeshGenerator mesher;
    
    // osc
    
    ofxOscReceiver receiver;
    
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
    
    ofVec3f selectedVertexPosition;
    int selectedVertexIndex;
    
};
