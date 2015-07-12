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

class Puppet {
    
public:
    
    void load(string path);
    void save(string path);
    
    void update();
    void draw(bool isSelectedPuppet);
    
    void setImage(ofImage img);
    void setMesh(ofMesh m);
    
    ofMesh getMesh();
    ofMesh getDeformedMesh();
    
    void addCenterpoint();
    
    void resetPose();
    
    void addExpressionZone(int index);
    void removeExpressionZone(int index);
    void removeAllExpressionZones();
    ExpressionZone* getExpressionZone(int index);
    
    void recieveOSCMessage(ofxOscMessage message, float value);
    void recieveLeapData(LeapDataHandler *leap, bool isSelected);
    
    const static int MIN_SELECT_VERT_DIST = 20;
    
private:
    
    ofImage image;
    ofPolyline contour;
    
    ofxPuppet meshDeformer;
    vector<ExpressionZone> expressionZones;
    
    const static int MESH_SMOOTH_SUBDIVISIONS = 2;
    
    // mesh & puppet
    
    ofMesh mesh;
    ofMesh undeformedSubdivided; //used for z-fighting fix
    ofMesh subdivided;
    ofxButterfly butterfly;
    
    void regenerateSubdivisionMesh();
    void updateMeshVertexDepths();
    
};

#endif