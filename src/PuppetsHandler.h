#ifndef __dranimate__PuppetsHandler__
#define __dranimate__PuppetsHandler__

#include <stdio.h>

#include "ofxOscReceiver.h"
#include "ofxClickDownMenu.h"
#include "ofxSyphon.h"
#include "ofxXmlSettings.h"

#include "Puppet.h"
#include "LeapDataHandler.h"
#include "Global.h"

class PuppetsHandler {
    
public:
    
    void setup();
    void update(LeapDataHandler *leap,
                ofxOscReceiver *osc,
                ofxClickDownMenu *cdmenu);
    void draw(LeapDataHandler *leap);
    
    void publishSyphonOutput();
    
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
    
    void setActiveLayer(int l);
    int getActiveLayer();
    int getNumLayers();
    
private:
    
    vector<Puppet> puppets;
    
    Puppet *getPuppet(int i);
    int getClosestPuppetIndex(int x, int y);
    
    void updateLeapUIControls(LeapDataHandler *leap,
                              ofxClickDownMenu *cdmenu);
    
    ofImage hand;
    
    bool addingBone = false;
    int boneRootVertexIndex;
    
    int hoveredVertexIndex;
    int selectedVertexIndex;
    int selectedPuppetIndex;
    int recordingPuppetIndex;
    
    bool enableLeapControls;
    int leapClickAgainTimer;
    
    enum CurrentUIControlType { LEAP, MOUSE };
    CurrentUIControlType currentUIControlType;
    
    Puppet newRecording;
    bool recording;
    
    int numLayers;
    int activeLayer;
    
    vector<ofxSyphonServer> layerOutputSyphonServers;
    
};

#endif