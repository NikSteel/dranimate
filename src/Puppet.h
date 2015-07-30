#pragma once

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
#include "Global.h"

class CachedFrame {
    
public:
    
    CachedFrame(ofMesh m, ofVec3f p,float r){
        mesh = m;
        position = p;
        rotation = r;
    };
    
    ofMesh mesh;
    ofVec3f position;
    float rotation;
    
};

class Puppet {
    
public:
    
    void load(string path);
    void save(string path);
    void loadCachedFrames(string path);
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
    float getRotation();
    
    void setLayer(int l);
    int getLayer();
    
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
    void addFrame(ofMesh mesh, ofVec3f position, float rot);
    
    bool isControllable();
    
    bool isPointInside(int x, int y);
    
    enum PuppetMode {
        CONTROLLABLE,
        RECORDED
    };
    
private:
    
    ofVec3f position;
    float rotation;
    
    ofImage image;
    
    ofMesh mesh;
    ofMesh undeformedSubdivided; //used for z-fighting fix
    ofMesh subdivided;
    ofxButterfly butterfly;
    
    ofxPuppet meshDeformer;
    vector<ExpressionZone> expressionZones;
    
    void updateMeshDeformation();
    void regenerateSubdivisionMesh();
    void updateMeshVertexDepths();
    
    void nextFrame();
    
    void drawAsControllable(bool isSelected, bool isBeingRecorded);
    void drawAsRecording(bool isSelected);
    
    vector<CachedFrame> cachedFrames;
    int currentFrame;
    bool playingForwards;
    
    PuppetMode mode;
    
    int layer;
    
};
