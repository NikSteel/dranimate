#include "Utils.h"

bool Utils::hasEnding (std::string const &fullString, std::string const &ending) {
    //http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool Utils::filenameIsImage(std::string fn) {
    
    return  Utils::hasEnding(fn, ".png")  ||
            Utils::hasEnding(fn, ".psd")  ||
            Utils::hasEnding(fn, ".gif")  ||
            Utils::hasEnding(fn, ".bmp")  ||
            Utils::hasEnding(fn, ".jpg")  ||
            Utils::hasEnding(fn, ".jpeg") ||
            Utils::hasEnding(fn, ".JPG")  ||
            Utils::hasEnding(fn, ".JPEG");
    
}

float Utils::sign(ofVec3f p1, ofVec3f p2, ofVec3f p3) {
    //http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool Utils::PointInTriangle (ofVec3f pt, ofVec3f v1, ofVec3f v2, ofVec3f v3) {
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

int Utils::getClosestIndex(ofMesh mesh, int x, int y, int min) {
    
    float closestDistance = MAXFLOAT;
    int closestIndex = -1;
    
    for(int i = 0; i < mesh.getVertices().size(); i++) {
        ofVec3f v = mesh.getVertex(i);
        float d = v.distance(ofVec3f(x,y,0));
        if(d < closestDistance && d < min) {
            closestDistance = d;
            closestIndex = i;
        }
    }
    
    return closestIndex;
    
}

void Utils::drawControls(string s) {
    
    /*
    ofSetColor(0,0,0);
    for(int x = -1; x <= 1; x++) { for(int y = -1; y <= 1; y++) {
    Resources::verdana12.drawString(s, ofGetWidth()-350-x, 30+y); }}
    ofSetColor(255,255,255);
    Resources::verdana12.drawString(s, ofGetWidth()-350, 30);
     */
    
    ofSetColor(0,0,0,200);
    ofRect(ofGetWidth()-300, 10, 290, 70);
    
    ofSetColor(255,255,255);
    ofDrawBitmapString(s, ofGetWidth()-290, 30);
    
}

void Utils::drawWarning(string s) {
    
    /*
    float flash = abs(sin(ofGetElapsedTimef()*10)*100);
    ofSetColor(255,flash,flash);
    Resources::verdana16.drawString(s, ofGetWidth()-350, 90);
     */
    
    float flash = abs(sin(ofGetElapsedTimef()*10)*200);
    ofSetColor(255,flash,flash);
    ofDrawBitmapString(s, 50, 50);
    ofDrawBitmapString(s, 51, 50);
    
}

void Utils::drawState(string s) {
    
    /*
    ofSetColor(0,0,0);
    for(int x = -1; x <= 1; x++) { for(int y = -1; y <= 1; y++) {
    Resources::verdana16.drawString(s, 30-x, ofGetHeight()-30+y); }}
    ofSetColor(255,255,255);
    Resources::verdana16.drawString(s, 30,   ofGetHeight()-30);
     */
    
    ofSetColor(0,0,0);
    ofDrawBitmapString(s, 30, ofGetHeight()-30);
    
}

void Utils::convertImagesToMovie(string fn) {
    
    ofLog() << "creating movie from frames in "+fn+"...";
    ofSystem("./../../../data/movies/ffmpeg -vcodec png -framerate 60 -i ../../../data/temp/frame%d.png -c:v libx264 -r 30 -pix_fmt yuv420p ../../../data/movies/"+fn+"/movie.mov");
    
}

std::vector<int> Utils::getFacesConnectedToVertex(ofMesh mesh, int i) {
    
    vector<int> results;
    
    ofVec3f targetVertex = mesh.getVertex(i);
    
    for(int i = 0; i < mesh.getUniqueFaces().size(); i++) {
        
        for(int f = 0; f < 3; f++) {
            
            ofVec3f faceVertex = mesh.getUniqueFaces()[i].getVertex(f);
            if(faceVertex == targetVertex) {
                results.push_back(i);
            }
            
        }
        
    }
    
    return results;
    
}

bool Utils::facesOnlyShareOneVertex(ofMeshFace faceA, ofMeshFace faceB) {
    
    int vertexShareCount = 0;
    
    for(int a = 0; a < 3; a++) {
        for(int b = 0; b < 3; b++) {
            
            if(faceA.getVertex(a) == faceB.getVertex(b)) {
                vertexShareCount++;
            }
            
        }
    }
    
    return vertexShareCount == 1;
    
}

void Utils::drawGrid() {
    
    int nHorizontalDraws = ofGetWidth() / Resources::grid.width;
    int nVerticalDraws   = ofGetHeight() / Resources::grid.height;
    
    for(int x = 0; x <= nHorizontalDraws; x++) {
        for(int y = 0; y <= nVerticalDraws; y++) {
            
            Resources::grid.draw(x*Resources::grid.width,y*Resources::grid.height);
            
        }
    }
    
}

