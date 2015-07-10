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
#include "Utils.h"
#include "LeapDataHandler.h"
#include "Resources.h"

class Puppet {
    
public:
    
    void load(string path);
    void save(string path);
    
    void setImage(ofImage img);
    void setMesh(ofMesh m);
    
    void addCenterpoint();
    
    void reset();
    void update();
    void draw(bool isSelected, bool isBeingRecorded);
    
    void resetPose();
    
    void regenerateSubdivisionMesh();
    void updateMeshVertexDepths();
    
    void addExpressionZone(int index);
    void removeExpressionZone(int index);
    void removeAllExpressionZones();
    ExpressionZone* getExpressionZone(int index);
    
    void recieveOSCMessage(ofxOscMessage message, float value);
    void recieveLeapData(LeapDataHandler *leap);
    
    const static int MIN_SELECT_VERT_DIST = 20;
    const static int MESH_SMOOTH_SUBDIVISIONS = 2;
    
    // mesh & puppet
    
    ofMesh originalMesh;
    ofMesh mesh;
    ofMesh undeformedSubdivided;
    ofMesh subdivided;
    ofxButterfly butterfly;
    
    int palmControlsPuppet;
    
    // puppeteering
    
    ofxPuppet meshDeformer;
    vector<ExpressionZone> expressionZones;
    
    // image
    
    ofImage image;
    ofPolyline contour;
    
    // state
    
    bool isBeingEdited;
    
};

#endif