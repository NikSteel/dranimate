#ifndef __dranimate__Puppet__
#define __dranimate__Puppet__

#include <iostream>

#include "ofMain.h"

#include "ofxPuppetInteractive.h"
#include "ofxButterfly.h"
#include "ofxCv.h"
#include "ofxTriangleMesh.h"
#include "ofxXmlSettings.h"
#include "ofxOscReceiver.h"

#include "ExpressionZone.h"
#include "Utils.h"
#include "LeapDataHandler.h"
#include "Resources.h"

class CachedFrame {
    
public:
    
    ofMesh mesh;
    ofVec3f position;
    
};

class Puppet {
    
public:
    
    const static int MIN_SELECT_VERT_DIST = 20;
    const static int MESH_SMOOTH_SUBDIVISIONS = 2;
    
    void load(string path);
    void loadCachedFrames(string path);
    void save(string path);
    void saveCachedFrames(string path);
    
    void update();
    void draw(bool isSelectedPuppet, bool isBeingRecorded);
    
    void setImage(ofImage img);
    void setMesh(ofMesh m);
    void setPosition(int x, int y);
    
    ofMesh getMesh();
    ofMesh getDeformedMesh();
    
    ofImage getImage();
    
    ofVec3f getPosition();
    
    void addCenterpoint();
    
    void resetPose();
    
    void addExpressionZone(int index);
    void removeExpressionZone(int index);
    void removeAllExpressionZones();
    ExpressionZone* getExpressionZone(int index);
    
    void recieveOSCMessage(ofxOscMessage message, float value);
    void recieveLeapData(LeapDataHandler *leap, bool isSelected);
    
    void makeControllable();
    void makeRecording();
    
    void clearCachedFrames();
    void addFrame(ofMesh mesh, ofVec3f position);
    
    bool isControllable();
    
    bool isPointInside(int x, int y);
    
    enum PuppetMode {
        CONTROLLABLE,
        RECORDED
    };
    
private:
    
    ofVec3f position;
    
    ofImage image;
    
    ofMesh mesh;
    ofMesh undeformedSubdivided; //used for z-fighting fix
    ofMesh subdivided;
    ofxButterfly butterfly;
    
    ofxPuppet meshDeformer;
    vector<ExpressionZone> expressionZones;
    
    void updateMeshDeformation();
    void nextFrame();
    void regenerateSubdivisionMesh();
    void updateMeshVertexDepths();
    
    void drawAsControllable(bool isSelected, bool isBeingRecorded);
    void drawAsRecording(bool isSelected);
    
    vector<CachedFrame> cachedFrames;
    int currentFrame;
    bool playingForwards;
    
    PuppetMode mode;
    
};

#endif