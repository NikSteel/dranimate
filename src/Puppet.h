//NOTE:
//this should be called something other than Puppet.
//an ofxPuppet is already called a puppet.
//it should probably be called something like ControllablePuppet or DranimateCharacter or Actor or something...

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
#include "OSCNamespace.h"
#include "Skeleton.h"

class Puppet {
    
public:
    
    void load(string path);
    void save(string path);
    
    void setImage(ofImage img);
    void setMesh(ofMesh m);
    
    void reset();
    void update();
    void draw(bool drawWireframe, bool transformActive);
    
    void beginScale();
    void beginRotate();
    void scaleMesh(ofVec2f origin, ofVec2f mouse);
    void rotateMesh(ofVec2f origin, ofVec2f mouse);
    
    void regenerateSubdivisionMesh();
    
    void addExpressionZone(int index);
    ExpressionZone* getExpressionZone(int index);
    void removeExpressionZone(int index);
    
    void makeExpressionZoneRoot(int meshIndex);
    
    void recieveOSCMessage(ofxOscMessage message, float value);
    void recieveLeapData(vector<ofVec3f> leapFingersPositions,
                         vector<ofVec3f> leapFingersCalibration,
                         ofVec3f palmPosition,
                         ofVec3f palmCalibration);
    
    // mesh & puppet
    
    ofMesh mesh;
    ofMesh subdivided;
    ofxButterfly butterfly;
    
    ofMesh untransformedMesh;
    
    const int MESH_SMOOTH_SUBDIVISIONS = 2;
    
    // puppeteering
    
    ofxPuppet puppet;
    vector<ExpressionZone> expressionZones;
    Skeleton skeleton;
    
    // image
    
    const int IMAGE_BASE_SIZE = 400;
    ofImage image;
    
};

#endif