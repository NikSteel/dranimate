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
    
    void exportCurrentPuppet();
    void loadPuppet(string fn);
    
    Puppet newPuppet;
    
    // mesh generation
    
    void findImageContours();
    void loadAndCleanupImage(string fn);
    
    int IMAGE_BASE_SIZE = 400;
    
    ofxCvGrayscaleImage cvImage;
    ofxCv::ContourFinder contourFinder;
    
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
    
    // gui
    
    bool drawGui;
    
    ofxPanel imageSettingsGui;
    ofxSlider<int> imageThreshold;
    ofxToggle invert;
    
    ofxPanel meshGeneratedGui;
    ofxToggle drawWireframe;
    
};
