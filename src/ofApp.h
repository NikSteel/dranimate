/*possible things to try:
  - proper depth testing for when limbs move
    on top of each other
  - restrict control point movement to avoid jelly 
  -
*/

#pragma once

#include "ofMain.h"

#include "ofxPuppetInteractive.h"
#include "ofxButterfly.h"
#include "ofxTriangleMesh.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxOscReceiver.h"

#include "Puppet.h"

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    
    void recieveOsc();
    void keyReleased(int key);
    void dragEvent(ofDragInfo dragInfo);
    void saveCurrentPuppet();
    
    Puppet newPuppet;
    vector<Puppet> puppets;
    
    // mesh generation
    
    void findImageContours();
    void generateMeshFromImage();
    void loadAndCleanupImage(string fn);
    
    int IMAGE_BASE_SIZE = 400;
    
    ofPolyline line;
    ofxTriangleMesh triangleMesh;
    ofxCvGrayscaleImage cvImage;
    ofxCv::ContourFinder contourFinder;
    
    // osc
    
    ofxOscReceiver receiver;
    
    // state
    
    enum State {
        LOAD_IMAGE,
        IMAGE_SETTINGS,
        MESH_GENERATED
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
