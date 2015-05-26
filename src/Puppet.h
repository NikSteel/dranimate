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

#include "ControlPoint.h"
#include "OSCNamespace.h"

class Puppet {
    
public:
    
    void load(string path);
    void save(string path);
    
    void setMesh(ofMesh m);
    
    void update();
    void draw(bool drawWireframe);
    
    void regenerateSubdivisionMesh();
    
    void addControlPoint(int index, vector<OSCNamespace> namespaces);
    
    // mesh & puppet
    
    ofMesh mesh, subdivided;
   	ofxPuppet puppet;
    ofxButterfly butterfly;
    vector<ControlPoint> controlPoints;
    
    int subs = 2;
    
    // image
    
    ofImage image;
    ofImage noAlphaImage;
    
};

#endif