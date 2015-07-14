#ifndef __dranimate__PuppetsHandler__
#define __dranimate__PuppetsHandler__

#include <stdio.h>

#include "ofxOscReceiver.h"
#include "ofxClickDownMenu.h"

#include "Puppet.h"
#include "LeapDataHandler.h"

class PuppetsHandler {
    
public:
    
    void setup();
    void update(LeapDataHandler *leap,
                ofxOscReceiver *osc,
                ofxClickDownMenu *cdmenu);
    void draw(LeapDataHandler *leap);
    
    void addPuppet(Puppet p);
    
    void loadPuppet(string path);
    void loadRecording(string path);
    
    void exportScene();
    void loadScene();
    
    Puppet *selectedPuppet();
    bool isAPuppetSelected();
    
    string getSelectedVertexInfo();
    
    void clickMouseAt(int x, int y);
    
    void updateWhichVertexIsHoveredOver(int x, int y);
    bool emptyVertexHoveredOver();
    bool ezoneHoveredOver();
    
    void addExpressionZoneToCurrentPuppet();
    void addLeapMappingToCurrentPuppet(int i);
    void addOSCMappingToCurrentPuppet();
    void addBoneToCurrentPuppet();
    void removeEZoneFromCurrentPuppet();
    void setAnchorPointOnCurrentPuppet();
    void exportCurrentPuppet();
    void removeCurrentPuppet();
    void resetCurrentPuppet();
    
    void clearAllPupets();
    void removeAllPuppets();
    
    void togglePuppetRecording();
    
private:
    
    vector<Puppet> puppets;
    
    Puppet *getPuppet(int i);
    int getClosestPuppetIndex(int x, int y);
    
    // ui & state
    
    void updateLeapUIControls(LeapDataHandler *leap,
                              ofxClickDownMenu *cdmenu);
    
    bool addingBone = false;
    int boneRootVertexIndex;
    
    int hoveredVertexIndex;
    int selectedVertexIndex;
    int selectedPuppetIndex;
    
    bool enableLeapControls;
    int leapClickAgainTimer;
    
    enum CurrentUIControlType {
        LEAP,
        MOUSE
    };
    CurrentUIControlType currentUIControlType;
    
    Puppet newRecording;
    bool recording;
    
};

#endif