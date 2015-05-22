#pragma once

#include "ofxTriangleMesh.h"
#include "ofMain.h"
#include "ofMesh.h"
#include "ofxPuppetInteractive.h"
#include "ofxButterfly.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxCv.h"

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyReleased(int key);
    void dragEvent(ofDragInfo dragInfo);
    
    void updateSubdivisionMesh();
    void findImageContours();
    void generateMeshFromImage();
    void loadAndCleanupImage(string fn);
    void saveCurrentPuppet();
    
    // mesh & puppet
    
    ofMesh mesh, subdivided;
   	ofxPuppetInteractive puppet;
    ofxButterfly butterfly;
    
    int subs = 0;
    
    // image
    
    ofImage image;
    ofTexture texture;
    ofxCvGrayscaleImage cvImage;
    ofxCv::ContourFinder contourFinder;
    
    int IMAGE_BASE_SIZE = 400;
    
    // triangulation
    
    ofPolyline line;
    ofxTriangleMesh triangleMesh;
    
    // state
    
    enum State {
        LOAD_IMAGE,
        IMAGE_SETTINGS,
        MESH_GENERATED
    };
    State state;
    
    // gui
    
    bool drawGui;
    
    ofxPanel gui;
    ofxSlider<int> imageThreshold;
    ofxToggle invert;
    ofxToggle drawWireframe;
    ofxToggle wiggleAllPoints;
    
};
