#include "MeshGenerator.h"

// public methods

void MeshGenerator::setup() {
    
    ofxXmlSettings launch; launch.load(LAUNCH_SETTINGS_PATH);
    if(launch.getValue("cameraInputEnabled", false)) {
        cam.setup();
    }
    
    grid.loadImage("resources/grid.png");
    
    // setup gui
    
    gui.setup();
    
    gui.add(rotation.setup("rotation", 0, 0, 3));
    gui.add(flipHorizontal.setup("flip horizontally", false));
    gui.add(flipVertical.setup("flip vertically", false));
    gui.add(invertImage.setup("invert image", false));
    
    gui.add(imageThreshold.setup("image threshold", 0, 0, 255));
    gui.add(useAdaptiveThreshold.setup("use adaptive threshold", false));
    gui.add(invertThresholdImage.setup("invert threshold image", false));
    
    gui.add(contourResampleAmt.setup("contour resample amt", 0, 15, 60));
    gui.add(triangleAngleConstraint.setup("angle constraint", 0, 0, 28));
    gui.add(triangleSizeConstraint.setup("size constraint", -1, -1, 100));
    
    // load default values from settings xml
    
    ofxXmlSettings settings; settings.load(MESHGEN_SETTINGS_PATH);
    
    rotation = settings.getValue("rotation", 0);
    flipHorizontal = settings.getValue("flipHorizontal", false);
    flipVertical = settings.getValue("flipVertical", false);
    invertImage = settings.getValue("invertImage", false);
    
    imageThreshold = settings.getValue("imageThreshold", 0);
    useAdaptiveThreshold = settings.getValue("useAdaptiveThreshold", false);
    invertThresholdImage = settings.getValue("invertThresholdImage", false);
    
    contourResampleAmt = settings.getValue("contourResampleAmt", 0);
    triangleAngleConstraint = settings.getValue("triangleAngleConstraint", 0);
    triangleSizeConstraint = settings.getValue("triangleSizeConstraint", 0);
    
    
}
void MeshGenerator::update() {
    
    // if we're using a live feed image, update the image
    // by grabbing the latest frame from the camera
    if(imageType == FROM_LIFE_FEED) {
        cam.update();
        setImage(cam.image);
    }
    
    findImageContours();
    
}
void MeshGenerator::draw() {
    
    Utils::drawGrid(grid);
    
    // draw image
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2,
                ofGetHeight()/2);
    
    if(!meshGenerated) {
        ofSetColor(255,255,255);
    } else {
        ofSetColor(100,100,100);
    }
    noAlphaImage.draw(-noAlphaImage.width/2,-noAlphaImage.height/2);
    
    // draw mesh
    
    if(meshGenerated) {
        ofSetColor(WIREFRAME_COLOR);
        mesh.drawWireframe();
    }
    
    // draw user-added extra verts
    
    for(int i = 0; i < extraVerts.size(); i++) {
        int blinkColor = round(sin(ofGetElapsedTimef())+1)*255/2;
        ofSetColor(blinkColor, 0, 0);
        ofCircle(extraVerts[i].x, extraVerts[i].y, 2);
    }
    
    // draw contours found from the thresholded image
    
    if(!meshGenerated) {
        
        ofPushMatrix();
        ofTranslate(-image.width/2,
                    -image.height/2);
        
        ofPushStyle();
        ofNoFill();
        
        ofSetLineWidth(CONTOUR_LINE_WIDTH);
        
        int nPolylines = (int)contourFinder.getPolylines().size();
        
        // largest contour
        ofSetColor(CONTOUR_COLOR);
        if(nPolylines > 0) {
            contourFinder.getPolyline(0).draw();
        }
        
        // other contours
        ofSetColor(UNUSED_CONTOUR_COLOR);
        for(int i = 1; i < nPolylines; i++) {
            contourFinder.getPolyline(i).draw();
        }
        
        ofPopStyle();
        
        ofPopMatrix();
        
    }
    
    ofPopMatrix();
    
    gui.draw();
    
}

void MeshGenerator::reset(bool liveMode) {
    
    meshGenerated = false;
    contourLine.clear();
    extraVerts.clear();
    
    if(liveMode) {
        imageType = FROM_LIFE_FEED;
    } else {
        imageType = FROM_FILE;
    }
    
}

void MeshGenerator::setImage(ofImage img) {
    
    if(invertImage) {
        for(int x = 0; x < img.width; x++) {
            for(int y = 0; y < img.height; y++) {
                ofColor c = img.getColor(x, y);
                c.r = 255 - c.r;
                c.g = 255 - c.g;
                c.b = 255 - c.b;
                img.setColor(x, y, c);
            }
        }
    }
    
    // resize the image if it's from a file (because some image files are really large ofc)
    if(imageType == FROM_FILE) {
        ofVec2f wh = ofVec2f(img.width,img.height);
        wh.normalize();
        img.resize(wh.x*IMAGE_BASE_SIZE,
                   wh.y*IMAGE_BASE_SIZE);
    }
    
    img.rotate90(rotation);
    img.mirror(flipVertical, flipHorizontal);
    
    image = img;
    noAlphaImage = img;
    
    // replace alpha channel with white
    
    for(int x = 0; x < noAlphaImage.width; x++) {
        for(int y = 0; y < noAlphaImage.height; y++) {
            ofColor c = noAlphaImage.getColor(x, y);
            if(c.a == 0) {
                noAlphaImage.setColor(x, y, ofColor(255,255,255));
            }
        }
    }
    
}
void MeshGenerator::addExtraVertex(int x, int y) {
    
    int nx = x - (ofGetWidth()/2  - cvImage.width/2);
    int ny = y - (ofGetHeight()/2 - cvImage.height/2);
    
    //temporarily disabled
    //extraVerts.push_back(ofPoint(nx,ny));
    
}

void MeshGenerator::generateMesh() {
    
    // create a polyline with all of the contour points
    
    contourLine.clear();
    
    vector<cv::Point> contour = contourFinder.getContour(0);
    for(int i = 0; i < contour.size(); i++) {
        contourLine.addVertex(contour[i].x,contour[i].y);
    }
    
    // use that polyline to generate a mesh with ofxTriangleMesh
    // (code from ofxTriangleMesh example)
    
    if (contourLine.size() > 2){
        
        ofPolyline lineRespaced = contourLine;
        
        // add the last point (so when we resample, it's a closed polygon)
        lineRespaced.addVertex(lineRespaced[0]);
        
        // resample
        lineRespaced = lineRespaced.getResampledBySpacing(contourResampleAmt);
        
        // make sure the first point and the last point are not the same, since triangle is unhappy
        lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
        
        for(int i = 0; i < extraVerts.size(); i++) {
            lineRespaced.addVertex(extraVerts[i]);
        }
        
        // if we have a proper set of points, mesh them:
        if (lineRespaced.size() > 5){
            
            // note: size constraint = -1 means don't constrain by size
            triangleMesh.triangulate(lineRespaced, triangleAngleConstraint, triangleSizeConstraint);
            
        }
    }
    
    mesh = triangleMesh.triangulatedMesh;
    
    if(isMeshBroken()) {
        
        ofLog() << "warning: mesh is broken. this will crash ofxPuppet.";
        
        meshGenerated = false;
        
    } else {
        
        meshGenerated = true;
        
        // reset mesh texture coords to match with image
        int len = mesh.getNumVertices();
        for(int i = 0; i < len; i++) {
            ofVec2f vec = mesh.getVertex(i);
            mesh.addTexCoord(vec);
        }
        
        // center mesh
        for(int i = 0; i < len; i++) {
            ofVec3f v = mesh.getVertex(i);
            v.x -= image.width/2;
            v.y -= image.height/2;
            mesh.setVertex(i, v);
        }
        
    }
    
    // somehow fix broken meshes
    // i.e., there exist whole pieces of the mesh that are
    // completely disconnected from the rest of the mesh (islands)
    // (this causes ofxPuppet to disappear or even crash)
    
    //todo
    
}

void MeshGenerator::saveXMLSettings() {
    
    ofxXmlSettings settings;
    
    settings.setValue("rotation", rotation);
    settings.setValue("flipHorizontal", flipHorizontal);
    settings.setValue("flipVertical", flipVertical);
    settings.setValue("invertImage", invertImage);
    
    settings.setValue("imageThreshold", imageThreshold);
    settings.setValue("useAdaptiveThreshold", useAdaptiveThreshold);
    settings.setValue("invertThresholdImage", invertThresholdImage);
    
    settings.setValue("contourResampleAmt", contourResampleAmt);
    settings.setValue("triangleAngleConstraint", triangleAngleConstraint);
    settings.setValue("triangleSizeConstraint", triangleSizeConstraint);
    
    settings.save(MESHGEN_SETTINGS_PATH);
    
}

ofImage MeshGenerator::getImage() {
    
    return image;
    
}
ofMesh MeshGenerator::getMesh() {
    
    return mesh;
    
}

// private methods

void MeshGenerator::findImageContours() {
    
    // create an image with brightness as red channel
    ofImage lumiImg = noAlphaImage;
    for(int x = 0; x < lumiImg.width; x++) {
        for(int y = 0; y < lumiImg.height; y++) {
            ofColor c = lumiImg.getColor(x,y);
            c.r = (c.r + c.g + c.b) / 3;
            lumiImg.setColor(x, y, c);
        }
    }
    
    // threshold image
    cvImage.setFromPixels(lumiImg.getPixelsRef().getChannel(0));
    
    if(useAdaptiveThreshold) {
        cvImage.adaptiveThreshold(imageThreshold);
    } else {
        cvImage.threshold(imageThreshold);
    }
    
    if(invertThresholdImage) cvImage.invert();
    
    // find contours from thresholded image
    
    contourFinder.setMinArea(1000);
    contourFinder.setMaxArea(noAlphaImage.width*noAlphaImage.height);
    //contourFinder.setFindHoles(true);
    contourFinder.setSortBySize(true);
    
    //contourFinder.setThreshold(100);
    contourFinder.findContours(ofxCv::toCv(cvImage));
    
}

bool MeshGenerator::isMeshBroken() {
    
    bool isBroken = false;
    
    // find loosely connected faces
    // i.e., if two triangles are connected by one point.
    // (this causes ofxPuppet to freak out.)
    
    for(int i = 0; i < mesh.getVertices().size(); i++) {
        
        vector<int> connectedFaces = Utils::getFacesConnectedToVertex(mesh,i);
        
        /*
         ofLog() << i;
         for(int f = 0; f < connectedFaces.size(); f++) {
         ofLog() << "     " << connectedFaces[f];
         }
         */
        
        if(connectedFaces.size() == 2) {
            
            ofMeshFace face1 = mesh.getUniqueFaces()[connectedFaces[0]];
            ofMeshFace face2 = mesh.getUniqueFaces()[connectedFaces[1]];
            
            if(Utils::facesOnlyShareOneVertex(face1,face2)) {
                isBroken = true;
                ofLog() << "loose vertex at " << i;
            }
            
            
        }
        
    }
    
    return isBroken;
    
}
