#include "MeshGenerator.h"

ofMesh MeshGenerator::generateMesh(ofxCv::ContourFinder contourFinder) {
    
    // create a polyline with all of the contour points
    
    vector<cv::Point> contour = contourFinder.getContour(0);
    for(int i = 0; i < contour.size(); i++) {
        line.addVertex(contour[i].x,contour[i].y);
    }
    
    // use that polyline to generate a mesh with ofxTriangleMesh !!
    // (code from ofxTriangleMesh example)
    
    if (line.size() > 2){
        
        ofPolyline lineRespaced = line;
        
        // add the last point (so when we resample, it's a closed polygon)
        lineRespaced.addVertex(lineRespaced[0]);
        
        // resample
        lineRespaced = lineRespaced.getResampledBySpacing(20);
        
        // I want to make sure the first point and the last point are not the same, since triangle is unhappy:
        lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
        
        // if we have a proper set of points, mesh them:
        if (lineRespaced.size() > 5){
            
            // angle constraint = 28
            // size constraint = -1 (don't constraint triangles by size);
            triangleMesh.triangulate(lineRespaced, 20, -1);
            
        }
    }
    
    ofMesh mesh = triangleMesh.triangulatedMesh;
    
    // reset mesh texture coords to match with an image
    
    int len = mesh.getNumVertices();
    for(int i = 0; i < len; i++) {
        ofVec2f vec = mesh.getVertex(i);
        mesh.addTexCoord(vec);
    }
    
    return mesh;
    
}