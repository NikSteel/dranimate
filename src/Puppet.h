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

#include "ExpressionZone.h"
#include "OSCNamespace.h"

class Puppet {
    
public:
    
    void load(string path);
    void save(string path);
    
    void setImage(ofImage img);
    void setMesh(ofMesh m);
    
    void update();
    void draw(bool drawWireframe);
    
    void regenerateSubdivisionMesh();
    
    void addExpressionZone(int index);
    void removeExpressionZone(int index);
    void addNamespaceToExpressionZone(int meshIndex, OSCNamespace namesp);
    void addFingerControllerToExpressionZone(int meshIndex, LeapFingerController fingerController);
    
    // mesh & puppet
    
    ofMesh mesh, subdivided;
   	ofxPuppet puppet;
    ofxButterfly butterfly;
    vector<ExpressionZone> expressionZones;
    
    int subs = 2;
    
    // image
    
    int IMAGE_BASE_SIZE = 400;
    ofImage image;
    
};

#endif