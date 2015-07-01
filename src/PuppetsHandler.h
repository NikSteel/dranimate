#ifndef __dranimate__PuppetsHandler__
#define __dranimate__PuppetsHandler__

#include <stdio.h>

#include "Puppet.h"
#include "PuppetRecorder.h"
#include "SceneRecorder.h"
#include "LeapDataHandler.h"
#include "ofxOscReceiver.h"
#include "ofxClickDownMenu.h"

class PuppetsHandler {
    
private:
    
    vector<Puppet> puppets;
    
public:
    
    void setup();
    void update(LeapDataHandler *leap,
                ofxOscReceiver *osc,
                ofxClickDownMenu *cdmenu);
    void draw();
    
    void mousePressed(ofMouseEventArgs& mouse);
    void mouseDragged(ofMouseEventArgs& mouse);
    void mouseMoved(ofMouseEventArgs& mouse);
    void mouseReleased(ofMouseEventArgs& mouse);
    void keyPressed(ofKeyEventArgs  &key);
    void keyReleased(ofKeyEventArgs &key);
    
    void recieveMenuCommand(string command);
    
    void addPuppet(Puppet p);
    Puppet *getPuppet(int i);
    
    int getClosestPuppetIndex(int x, int y);
    int getClosestRecordingIndex(int x, int y);
    
    Puppet *selectedPuppet();
    
    string getSelectedVertexInfo();
    
    // ui & state
    
    bool addingBone = false;
    int boneRootVertexIndex;
    
    enum ControlType {
        CONTROLS_MOUSE,
        CONTROLS_LEAP
    };
    ControlType controlType;
    
    int hoveredVertexIndex;
    int selectedVertexIndex;
    int selectedPuppetIndex;
    
    int selectedRecordingIndex;
    
    bool controlsPaused;
    
    bool cdMenuOpen;
    
    // recording
    
    bool recordingPuppet;
    PuppetRecorder puppetRecorder;
    vector<PuppetRecorder> recordedPuppets;
    
    bool recordingScene;
    SceneRecorder sceneRecorder;
    
};

#endif