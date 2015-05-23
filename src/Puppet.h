#ifndef __dranimate__Puppet__
#define __dranimate__Puppet__

#include <iostream>

#include "ofMain.h"

#include "ofxPuppetInteractive.h"
#include "ofxButterfly.h"
#include "ofxCv.h"
#include "ofxTriangleMesh.h"

class Puppet {
    
public:
    
    void update();
    void draw(bool drawWireframe);
    
    void updateSubdivisionMesh();
    
    // mesh & puppet
    
    ofMesh mesh, subdivided;
   	ofxPuppetInteractive puppet;
    ofxButterfly butterfly;
    vector<int> puppetControlIndices;
    
    int subs = 2;
    
    // image
    
    ofImage image;
    
};

#endif