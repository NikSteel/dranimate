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

class Puppet {
    
public:
    
    void load(string path);
    void save(string path);
    
    void setImage(ofImage img);
    void setMesh(ofMesh m);
    void setContour(ofPolyline line);
    
    void reset();
    void update();
    void draw(bool isSelected);
    
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
    const static int IMAGE_BASE_SIZE = 400;
    
    // mesh & puppet
    
    ofMesh originalMesh;
    ofMesh mesh;
    ofMesh undeformedSubdivided;
    ofMesh subdivided;
    ofxButterfly butterfly;
    
    float scale = 1;//todo fix this...
    float oldScale;
    
    // puppeteering
    
    ofxPuppet meshDeformer;
    vector<ExpressionZone> expressionZones;
    
    bool isPaused;
    
    // image
    
    ofImage image;
    ofPolyline contour;
    
};

#endif