#include "Utils.h"

//http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c

bool Utils::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

//http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle

float Utils::sign(ofVec3f p1, ofVec3f p2, ofVec3f p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool Utils::PointInTriangle (ofVec3f pt, ofVec3f v1, ofVec3f v2, ofVec3f v3)
{
    bool b1, b2, b3;
    
    b1 = Utils::sign(pt, v1, v2) < 0.0f;
    b2 = Utils::sign(pt, v2, v3) < 0.0f;
    b3 = Utils::sign(pt, v3, v1) < 0.0f;
    
    return ((b1 == b2) && (b2 == b3));
}

bool Utils::isPointInsideMesh(ofMesh mesh, int x, int y) {
    
    for(int f = 0; f < mesh.getUniqueFaces().size(); f++) {
        
        ofMeshFace meshFace = mesh.getUniqueFaces()[f];
        ofVec3f p0 = meshFace.getVertex(0);
        ofVec3f p1 = meshFace.getVertex(1);
        ofVec3f p2 = meshFace.getVertex(2);
        
        if(Utils::PointInTriangle(ofVec3f(x,y,0), p0, p1, p2)) {
            return true;
        }
        
    }
    
    return false;
    
}

int Utils::getClosestIndex(ofMesh mesh, int x, int y) {
    
    float closestDistance = MAXFLOAT;
    int closestIndex = -1;
    
    for(int i = 0; i < mesh.getVertices().size(); i++) {
        ofVec3f v = mesh.getVertex(i);
        float d = v.distance(ofVec3f(x,y,0));
        if(d < closestDistance && d < Puppet::MIN_SELECT_VERT_DIST) {
            closestDistance = d;
            closestIndex = i;
        }
    }
    
    return closestIndex;
    
}

ofImage Utils::hand;

void Utils::loadImages() {
    
    Utils::hand.loadImage("ui/hand.png");
    
}