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
#include "SceneRecorder.h"
#include "LeapDataHandler.h"
#include "Utils.h"
#include "ImageFromCamera.h"
#include "Resources.h"

class ofApp : public ofBaseApp {
    
public:
    
    void setup();
    void update();
    void draw();
    
    int getClosestPuppetIndex();
    int getClosestRecordingIndex();
    
    Puppet *selectedPuppet();
    
    string getSelectedVertexInfo();
    
    void keyReleased(int key);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void dragEvent(ofDragInfo dragInfo);
    
// puppets
    
    vector<Puppet> puppets;
    
    Puppet newPuppet;
    MeshGenerator mesher;
    
    bool recordingPuppet;
    PuppetRecorder puppetRecorder;
    vector<PuppetRecorder> recordedPuppets;
    
// leap
    
    LeapDataHandler leapHandler;
    
// state
    
    enum State {
        NEW_PUPPET_CREATION,
        PUPPET_STAGE,
        LEAP_CALIBRATION
    };
    State state;
    
// ui
    
    bool createPuppetLiveMode = false;
    
    bool wholeScenePaused = false;
    
    bool addingBone = false;
    int boneRootVertexIndex;
    
    int hoveredVertexIndex;
    int selectedVertexIndex;
    int selectedPuppetIndex;
    
    int selectedRecordingIndex;
    
    ofxButton b;
    
    bool controlsPaused;
    
// clickdown menu
    
    void cmdEvent(ofxCDMEvent &ev);
    void updateClickDownMenu();
    ofxClickDownMenu clickDownMenu;
    
// camera
    
    ImageFromCamera cam;
    
// scene recorder
    
    bool recordingScene;
    SceneRecorder sceneRecorder;
    
// osc
    
    ofxOscReceiver oscReceiver;
    
};
