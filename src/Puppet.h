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
    
    ofMesh getMesh();
    ofMesh getDeformedMesh();
    
    void addCenterpoint();
    
    void reset();
    void update();
    void draw();
    
    void resetPose();
    
    void addExpressionZone(int index);
    void removeExpressionZone(int index);
    void removeAllExpressionZones();
    ExpressionZone* getExpressionZone(int index);
    
    void recieveOSCMessage(ofxOscMessage message, float value);
    void recieveLeapData(LeapDataHandler *leap);
    
    void setEditMode(bool beingEdited);
    bool isInEditMode();
    
    const static int MIN_SELECT_VERT_DIST = 20;
    
private:
    
    ofImage image;
    ofPolyline contour;
    
    ofxPuppet meshDeformer;
    vector<ExpressionZone> expressionZones;
    
    bool inEditMode;
    
    const static int MESH_SMOOTH_SUBDIVISIONS = 2;
    
    // mesh & puppet
    
    ofMesh originalMesh;
    ofMesh mesh;
    ofMesh undeformedSubdivided;
    ofMesh subdivided;
    ofxButterfly butterfly;
    
    int palmControlsPuppet;
    
    void regenerateSubdivisionMesh();
    void updateMeshVertexDepths();
    
};

#endif