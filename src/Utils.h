#ifndef __dranimate__Utils__
#define __dranimate__Utils__

#include <iostream>
#include "ofVec3f.h"
#include "ofMain.h"
#include "Puppet.h"

class Utils {
    
public:
    static bool hasEnding (std::string const &fullString, std::string const &ending);
    
    static float sign(ofVec3f p1, ofVec3f p2, ofVec3f p3);
    
    static bool PointInTriangle (ofVec3f pt, ofVec3f v1, ofVec3f v2, ofVec3f v3);
    
    static bool isPointInsideMesh(ofMesh mesh, int x, int y);

    static int getClosestIndex(ofMesh mesh, int x, int y);
    
    static void loadImages();
    
    static ofImage hand;
};

#endif