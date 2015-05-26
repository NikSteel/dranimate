#ifndef __dranimate__MeshGenerator__
#define __dranimate__MeshGenerator__

#include <iostream>

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxTriangleMesh.h"

class MeshGenerator {
    
private:
    
    ofPolyline line;
    ofxTriangleMesh triangleMesh;
    
public:
    
    ofMesh generateMesh(ofxCv::ContourFinder contourFinder);
    
};

#endif
