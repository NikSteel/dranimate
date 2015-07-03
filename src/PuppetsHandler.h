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
    void draw(LeapDataHandler *leap);
    
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
    
    int hoveredVertexIndex;
    int selectedVertexIndex;
    int selectedPuppetIndex;
    
    int selectedRecordingIndex;
    
    bool controlsPaused;
    
    bool cdMenuOpen;
    
    bool enableLeapControls;
    int leapClickAgainTimer;
    
    // recording
    
    bool recordingPuppet;
    PuppetRecorder puppetRecorder;
    vector<PuppetRecorder> recordedPuppets;
    
    bool recordingScene;
    SceneRecorder sceneRecorder;
    
    // (temp hack) line connecting fingers to ezones
    int connectTimer;
    int connectedFinger = -1;
    int connectedEzone = -1;
    int connectedPuppet = -1;
    
};

#endif