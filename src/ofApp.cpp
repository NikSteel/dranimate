#include "ofApp.h"

void ofApp::setup() {
    
    drawGui = true;
    
    gui.setup();
    gui.add(imageThreshold.setup("image threshold", 86, 0, 255));
    gui.add(invert.setup("invert", true));
    gui.add(drawWireframe.setup("draw wireframe", true));
    
    state = LOAD_IMAGE;
    
}

void ofApp::update() {
    
    ofShowCursor();
    
    if(state == IMAGE_SETTINGS) {
        
        // threshold image
        
        cvImage.setFromPixels(image.getPixelsRef().getChannel(1));
        cvImage.threshold(imageThreshold);
        if(invert) cvImage.invert();
        
        // find contours from thresholded image
        
        contourFinder.setMinArea(1000);
        contourFinder.setMaxArea(640*480);
        //contourFinder.setFindHoles(true);
        contourFinder.setSortBySize(true);
        
        //contourFinder.setThreshold(contourThreshold);
        contourFinder.findContours(ofxCv::toCv(cvImage));
        
    }
    
    if(state == MESH_GENERATED) {
        puppet.update();
    }
    
}

void ofApp::draw() {
    
    ofSetColor(255);
    ofBackground(0);
	
    if(state == LOAD_IMAGE) {
    
        ofSetColor(255,255,255);
        ofDrawBitmapString("Drag file into window or press 'l' to load an image", 300, 30);
        
    } else if(state == IMAGE_SETTINGS) {
        
        // draw thresholded image
        
        ofSetColor(255,255,255);
        cvImage.draw(0, 0);
        
        // draw contours found from the thresholded image
        
        ofSetColor(255, 0, 0);
        contourFinder.draw();
        
        ofSetColor(255,255,255);
        ofDrawBitmapString("Press 'm' to generate mesh when ready", 300, 30);
        
    } else if (state == MESH_GENERATED) {
        
        // fix the subdivided mesh to the mesh deformed by the puppet
        
        butterfly.fixMesh(puppet.getDeformedMesh(), subdivided);
        
        // draw the subdivided mesh textured with our image
        
        texture.bind();
        subdivided.drawFaces();
        texture.unbind();
        
        // draw the wireframe as well
        
        if (drawWireframe){
            glLineWidth(1.0);
            ofSetColor(0,255,50);
            subdivided.drawWireframe();
        }
        
        // debug stuff
        
        puppet.drawControlPoints();
        
        int xMargin = 300;
        ofSetColor(255,255,255);
        ofDrawBitmapString("# Subdivisions: " + ofToString(subs), xMargin, 30);
        ofDrawBitmapString("Press <Right Arrow> to increase the edge subdivisions", xMargin, 50);
        ofDrawBitmapString("Press <Left  Arrow> to decrease the edge subdivisions", xMargin, 70);
        ofDrawBitmapString("Press 'w' to show wireframe", xMargin, 90);
    }
    
    if(drawGui) gui.draw();
    
}

void ofApp::keyReleased(int key) {
    
    switch (state) {
            
    case LOAD_IMAGE:
        
        if(key == 'l') {
            
            ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an image:",true);
            
            if (openFileResult.bSuccess){
                
                loadAndCleanupImage(openFileResult.getPath());
                
            }
            
        }
        
        break;
        
    case IMAGE_SETTINGS:
        
        if(key == 'm') {
            generateMeshFromImage();
        }
        
        break;

    case MESH_GENERATED:
        
        // left/right controls how many subdivisions to use for smoothing the mesh
            
        if (key == OF_KEY_RIGHT) {
            subs++;
            updateSubdivisionMesh();
        }
        if (key == OF_KEY_LEFT && subs > 0) {
            subs--;
            updateSubdivisionMesh();
        }
        
        break;
        
    default:
        break;
            
    }
    
    if(key == 'g') {
        drawGui = !drawGui;
    }
    
}

void ofApp::updateSubdivisionMesh() {
    
    butterfly.topology_start(mesh);
    
    for(int i = 0; i < subs; i++) {
        butterfly.topology_subdivide_boundary();
    }
    
    subdivided = butterfly.topology_end();
    
}


void ofApp::generateMeshFromImage() {
    
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
            triangleMesh.triangulate(lineRespaced, 28, -1);
            
        }
    }
    
    mesh = triangleMesh.triangulatedMesh;
    
    // reset mesh texture coords to match with the image
    
    int len = mesh.getNumVertices();
    for(int i = 0; i < len; i++) {
        ofVec2f vec = mesh.getVertex(i);
        mesh.addTexCoord(vec);
    }
    
    // setup puppet
    
    puppet.setup(mesh);
    puppet.setControlPoint(1);
    
    // setup smooth subdivided mesh
    
    updateSubdivisionMesh();
    
    
    state = MESH_GENERATED;
    
}

void ofApp::loadAndCleanupImage(string fn) {
    
    image.loadImage(fn);
    
    // scale down image to a good size for the mesh generator
    
    float whRatio = (float)image.width/(float)image.height;
    image.resize(400*whRatio, 400);
    
    // replace alpha channel with white
    
    for(int x = 0; x < image.width; x++) {
        for(int y = 0; y < image.height; y++) {
            ofColor c = image.getColor(x, y);
            if(c.a == 0) {
                image.setColor(x, y, ofColor(255,255,255));
            }
        }
    }
    
    texture = image.getTextureReference();
    
    state = IMAGE_SETTINGS;
    
}

// lets us drag an image into the window to load it - very convenient

void ofApp::dragEvent(ofDragInfo info)
{
    
    if(info.files.size() > 0)
    {
        loadAndCleanupImage(info.files.at(0));
        
    }
    
}
