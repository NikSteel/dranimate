#include "Puppet.h"

// public methods

void Puppet::load(string path) {
    
    mode = CONTROLLABLE;
    
    // load image
    image.loadImage(path + "/image.png");
    
    // load mesh
    mesh.load(path + "/mesh.ply");
    meshDeformer.setup(mesh);
    regenerateSubdivisionMesh();
    
    // load control points and their osc and leap mappings
    
    ofxXmlSettings expressionZones;
    if(expressionZones.loadFile(path + "/expressionZones.xml")) {
        
        int nexpressionZones = expressionZones.getNumTags("expressionZone");
        for(int i = 0; i < nexpressionZones; i++) {
            
            expressionZones.pushTag("expressionZone", i);
            int expressionZoneIndex = expressionZones.getValue("meshIndex", 0);
            addExpressionZone(expressionZoneIndex);
            
            // load this expressionzone's OSC namespaces
            
            int nNamespaces = expressionZones.getNumTags("oscNamespace");
            for(int j = 0; j < nNamespaces; j++) {
                
                expressionZones.pushTag("oscNamespace", j);
                
                string message = expressionZones.getValue("message", "");
                string controlType = expressionZones.getValue("controlType", "");
                
                OSCNamespace namesp;
                namesp.message = message;
                namesp.controlType = controlType;
                
                getExpressionZone(expressionZoneIndex)->oscNamespaces.push_back(namesp);
                
                expressionZones.popTag();
                
            }
            
            // load this expressionzone's leap mapping
            
            int fingerID = expressionZones.getValue("fingerID", -1);
            getExpressionZone(expressionZoneIndex)->leapFingerID = fingerID;
            
            // load this expressionzone's bone data
            
            int parentEzone = expressionZones.getValue("parentEzone", -1);
            getExpressionZone(expressionZoneIndex)->parentEzone = parentEzone;
            
            int nChildren = expressionZones.getNumTags("childrenEzones");
            for(int j = 0; j < nChildren; j++) {
                
                // something is broken here.
                
                expressionZones.pushTag("child", j);
                
                int childEzone = expressionZones.getValue("meshIndex", -1);
                getExpressionZone(expressionZoneIndex)->childrenEzones.push_back(childEzone);
                
                expressionZones.popTag();
                
            }
            
            expressionZones.popTag();
            
        }
        
    }
    
}
void Puppet::save(string path) {
    
    // the folder itself
    string mkdirCommandString = "mkdir " + path;
    system(mkdirCommandString.c_str());
    
    // mesh
    mesh.save(path + "/mesh.ply");
    
    // image
    image.saveImage(path + "/image.png");
    
    // control points
    ofxXmlSettings expressionZonesXML;
    for(int i = 0; i < expressionZones.size(); i++){
        
        expressionZonesXML.addTag("expressionZone");
        expressionZonesXML.pushTag("expressionZone",i);
        
        expressionZonesXML.addValue("meshIndex", expressionZones[i].meshIndex);
        
        // save OSC namespaces
        
        for(int j = 0; j < expressionZones[i].oscNamespaces.size(); j++) {
            
            expressionZonesXML.addTag("oscNamespace");
            expressionZonesXML.pushTag("oscNamespace",j);
            
            OSCNamespace namesp = expressionZones[i].oscNamespaces[j];
            expressionZonesXML.addValue("message", namesp.message);
            expressionZonesXML.addValue("controlType", namesp.controlType);
            
            expressionZonesXML.popTag();
            
        }
        
        // save leap finger control mapping
        
        expressionZonesXML.addValue("fingerID", expressionZones[i].leapFingerID);
        
        // save bones
        
        expressionZonesXML.addValue("parentEzone", expressionZones[i].parentEzone);
        
        for(int j = 0; j < expressionZones[i].childrenEzones.size(); j++) {
            
            expressionZonesXML.addTag("childrenEzones");
            expressionZonesXML.pushTag("childrenEzones",j);
            
            expressionZonesXML.addValue("child", expressionZones[i].childrenEzones[j]);
            
            expressionZonesXML.popTag();
            
        }
        
        expressionZonesXML.popTag();
        
        expressionZonesXML.popTag();
        
    }
    expressionZonesXML.saveFile(path + "/expressionZones.xml");
    
    
    // todo: save data that svd calculates to allow near-instant loading of puppets
    
    
    ofLog() << "puppet saved to  " << path << "!";
    
}
void Puppet::loadCachedFrames(string path) {
    
    mode = RECORDED;
    
    currentFrame = 0;
    playingForwards = true;
    
    image.loadImage(path+"/image.png");
    
    ofxXmlSettings info;
    if(info.loadFile(path + "/info.xml")) {
        
        int nFrames = info.getNumTags("frame");
        for(int i = 0; i < nFrames; i++) {
            
            info.pushTag("frame", i);
            float x = info.getValue("x", 0.0);
            float y = info.getValue("y", 0.0);
            float z = info.getValue("z", 0.0);
            float r = info.getValue("r", 0.0);
            info.popTag();
            
            ofMesh mesh;
            mesh.load(path + "/frame" + ofToString(i) + ".ply");
            
            cachedFrames.push_back(CachedFrame(mesh, ofVec3f(x,y,z), r));
            
        }
        
    }
    
    float x = info.getValue("x", 0.0);
    float y = info.getValue("y", 0.0);
    float z = info.getValue("z", 0.0);
    position = ofVec3f(x,y,z);
    
}
void Puppet::saveCachedFrames(string path) {
    
    // make a new dir for the recording
    string mkdirCommandString = "mkdir " + path;
    system(mkdirCommandString.c_str());
    
    ofxXmlSettings info;
    
    info.addValue("x", position.x);
    info.addValue("y", position.y);
    info.addValue("z", position.z);
    info.addValue("r", rotation);
    
    for(int i = 0; i < cachedFrames.size(); i++) {
        
        info.addTag("frame");
        info.pushTag("frame",i);
        info.addValue("x", cachedFrames[i].position.x);
        info.addValue("y", cachedFrames[i].position.y);
        info.addValue("z", cachedFrames[i].position.z);
        info.addValue("r", cachedFrames[i].rotation);
        info.popTag();
        
        cachedFrames[i].mesh.save(path + "/frame" + ofToString(i) + ".ply");
        
    }
    
    image.saveImage(path + "/image.png");
    
    info.save(path + "/info.xml");
    
    ofLog() << "recording saved to " << path;
    
}

void Puppet::update() {
    
    if(isControllable()) {
    
        updateMeshDeformation();
        
        // update mesh vertex depths (so that we avoid z-fighting issues)
        updateMeshVertexDepths();
    
    } else {
        
        nextFrame();
        
    }
    
}
void Puppet::draw(bool isSelectedPuppet, bool isBeingRecorded) {
    
    if(isControllable()) {
        
        drawAsControllable(isSelectedPuppet, isBeingRecorded);
        
    } else {
        
        drawAsRecording(isSelectedPuppet);
        
    }
    
}

void Puppet::setPosition(int x, int y) {
    
    position.x = x;
    position.y = y;
    
}
void Puppet::setImage(ofImage img) {
    
    image = img;
    
}
void Puppet::setMesh(ofMesh m) {
    
    mesh = m;
    
    meshDeformer.setup(mesh);
    regenerateSubdivisionMesh();
    
}

ofMesh Puppet::getMesh() {
    return mesh;
}
ofMesh Puppet::getDeformedMesh() {
    
    return meshDeformer.getDeformedMesh();
    
}
ofImage Puppet::getImage() {
    return image;
}
ofVec3f Puppet::getPosition() {
    
    return position;
    
}
float Puppet::getRotation() {
    
    return rotation;
    
}

void Puppet::addCenterpoint() {

    ofVec3f c = mesh.getCentroid();
    addExpressionZone(Utils::getClosestIndex(mesh, c.x,c.y, INT_MAX));
    
}

void Puppet::resetPose() {
    
    for(int i = 0; i < expressionZones.size(); i++) {
        expressionZones[i].userControlledDisplacement = ofVec2f(0,0);
    }
    
}

void Puppet::addExpressionZone(int meshIndex) {
    
    // add control point to ofxPuppet
    meshDeformer.setControlPoint(meshIndex);
    
    // add expressionzone data to the list
    ExpressionZone newExpressionZone;
    newExpressionZone.meshIndex = meshIndex;
    newExpressionZone.leapFingerID = -1;
    newExpressionZone.isAnchorPoint = false;
    expressionZones.push_back(newExpressionZone);
    
}
void Puppet::removeExpressionZone(int meshIndex) {
    
    // remove the control point from the puppeteer
    meshDeformer.removeControlPoint(meshIndex);
    
    // find and erase expression zone
    for(int i = 0; i < expressionZones.size(); i++) {
        if(expressionZones[i].meshIndex == meshIndex) {
            expressionZones.erase(expressionZones.begin()+i);
            break;
        }
    }
    
}
void Puppet::removeAllExpressionZones() {
    
    for(int i = 0; i < meshDeformer.getDeformedMesh().getVertices().size(); i++) {
        removeExpressionZone(i);
    }
}
ExpressionZone* Puppet::getExpressionZone(int meshIndex) {
    
    for(int i = 0; i < expressionZones.size(); i++) {
        if(expressionZones[i].meshIndex == meshIndex) {
            return &expressionZones[i];
            break;
        }
    }
    
    return NULL;
    
}

void Puppet::recieveOSCMessage(ofxOscMessage message, float value) {
    
    ofLog() << message.getAddress();
    
    float wOffset = ofGetWidth()/2 - image.getWidth()/2;
    float hOffset = ofGetHeight()/2 - image.getHeight()/2;
    
    // this is a bit messy, should fix it
    
    for(int i = 0; i < expressionZones.size(); i++) {
        
        ExpressionZone expressionZone = expressionZones[i];
        vector<OSCNamespace> namespaces = expressionZone.oscNamespaces;
        
        ofVec3f expressionZonePosition = mesh.getVertex(expressionZone.meshIndex);
        
        for(int j = 0; j < namespaces.size(); j++) {
            
            OSCNamespace namesp = namespaces[j];
            
            if(namesp.message == message.getAddress()) {
                
                if(namesp.controlType == "x") {
                    expressionZones[i].userControlledDisplacement.x = -mesh.getVertex(expressionZones[i].meshIndex).x+value;
                } else if(namesp.controlType == "y") {
                    expressionZones[i].userControlledDisplacement.y = -mesh.getVertex(expressionZones[i].meshIndex).y+value;
                }
                
            }
            
        }
        
    }

}
void Puppet::recieveLeapData(LeapDataHandler *leap, bool isSelected) {
    
    int palmControlsPuppet = 1;
    
    for(int i = 0; i < expressionZones.size(); i++) {
        
        int fid = expressionZones[i].leapFingerID;
        
        // if there exists an expression zone controlled by the right hand,
        // make the right palm control the overall position of the puppet.
        if(fid != -1 && fid < 5) {
            palmControlsPuppet = 0;
        }
        
    }
    
    if(leap->calibrated && !isSelected) {
        
        ofVec3f calibratedPalm = leap->getCalibratedPalmPosition(palmControlsPuppet);
        position = ofVec3f(calibratedPalm.x,-calibratedPalm.y);
        
        rotation = leap->getPalmRotation(palmControlsPuppet)*-45;
    
        for(int i = 0; i < expressionZones.size(); i++) {
            
            ExpressionZone *ezone = &expressionZones[i];
            
            if(ezone->isAnchorPoint) {
              
                // ezone doesn't move because it's an anchor point
                
            } else if(ezone->leapFingerID != -1) {
            
                // this ezone has a finger mapping!
                // so set this ezone's displacement to that leap finger
                
                ofVec3f calibratedFinger = leap->getCalibratedFingerPosition(expressionZones[i].leapFingerID);
                
                ezone->userControlledDisplacement.x = calibratedFinger.x;
                ezone->userControlledDisplacement.y = -calibratedFinger.y;
                
            } else {
                
                // this ezone has no finger mapping.
                // so just set the displacement to zero
                
                ezone->userControlledDisplacement.x = 0;
                ezone->userControlledDisplacement.y = 0;
                
            }
            
        }
        
        for(int i = 0; i < expressionZones.size(); i++) {
            
            ExpressionZone *ezone = &expressionZones[i];
            
            // restrict points to parent (rigidity) (if they have a parent bone)
            
            if(ezone->parentEzone != -1) {
                
                ofVec3f ezoneVertexPosition = mesh.getVertex(ezone->meshIndex);
                ofVec3f ezoneAbsoulteVertexPosition = ezoneVertexPosition + ezone->userControlledDisplacement;
                
                ofVec3f ezoneParentVertexPosition = mesh.getVertex(ezone->parentEzone);
                ofVec3f ezoneParentAbsoluteVertexPosition = ezoneParentVertexPosition + getExpressionZone(ezone->parentEzone)->userControlledDisplacement;
                
                float d = ezoneVertexPosition.distance(ezoneParentVertexPosition);
                
                float angle = atan2(ezoneAbsoulteVertexPosition.y-ezoneParentAbsoluteVertexPosition.y,
                                    ezoneAbsoulteVertexPosition.x-ezoneParentAbsoluteVertexPosition.x);
                
                ofVec3f diff;
                diff = ofVec3f(cos(angle), sin(angle), 0);
                diff.normalize();
                diff = diff * d;
                
                ezone->userControlledDisplacement = diff;
                
            }
            
        }
    } else {
        
        // the leap isn't calibrated, or the puppet is being edited.
        // so return all the ezones to their original positions.
        // this returns the puppet to it's original pose.
        
        position = ofVec3f(0,0);
        rotation = 0;
        
        for(int i = 0; i < expressionZones.size(); i++) {
            
            ExpressionZone *ezone = &expressionZones[i];
            
            ezone->userControlledDisplacement.x = 0;
            ezone->userControlledDisplacement.y = 0;
                
            
        }
        
    }
}

void Puppet::clearCachedFrames() {
    
    cachedFrames.clear();
    
}
void Puppet::addFrame(ofMesh mesh, ofVec3f position, float rot) {
    
    cachedFrames.push_back(CachedFrame(mesh,position,rot));
    
}

void Puppet::makeControllable() {
    
    mode = CONTROLLABLE;
    
}
void Puppet::makeRecording() {
    
    mode = RECORDED;
    
}

bool Puppet::isControllable() {
    
    return mode == CONTROLLABLE;
    
}

bool Puppet::isPointInside(int x, int y) {
    
    if(isControllable()) {
        
        return Utils::isPointInsideMesh(getDeformedMesh(),
                                        x-getPosition().x,
                                        y-getPosition().y);
        
    } else {
        
        return Utils::isPointInsideMesh(cachedFrames[currentFrame].mesh,
                                        x - getPosition().x - cachedFrames[currentFrame].position.x,
                                        y - getPosition().y - cachedFrames[currentFrame].position.y);
        
    }
    
}

// private methods

void Puppet::updateMeshDeformation() {

    // do ofxPuppet puppeteering stuff (if there is more than one point;
    // as rigid as possible freaks out with onely one control point.)
    
    if(expressionZones.size() > 1) {
        
        // add displacements to puppet control points
        for(int i = 0; i < expressionZones.size(); i++) {
            
            if(expressionZones[i].parentEzone == -1) {
                // no parent, this ezone moves independently
                meshDeformer.setControlPoint(expressionZones[i].meshIndex,
                                             mesh.getVertex(expressionZones[i].meshIndex)+
                                             expressionZones[i].userControlledDisplacement);
            } else {
                meshDeformer.setControlPoint(expressionZones[i].meshIndex,
                                             mesh.getVertex(expressionZones[i].parentEzone)+
                                             getExpressionZone(expressionZones[i].parentEzone)->userControlledDisplacement+
                                             expressionZones[i].userControlledDisplacement);
            }
            
        }
        
        meshDeformer.update();
        
    }
    
    // attach the subdivided mesh to the mesh deformed by the puppet
    butterfly.fixMesh(meshDeformer.getDeformedMesh(), subdivided);

}
void Puppet::regenerateSubdivisionMesh() {
    
    butterfly.topology_start(mesh);
    
    for(int i = 0; i < MESH_SMOOTH_SUBDIVISIONS; i++) {
        butterfly.topology_subdivide_boundary();
    }
    
    subdivided = butterfly.topology_end();
    
    undeformedSubdivided = subdivided;
    
}
void Puppet::updateMeshVertexDepths() {
    
    for(int i = 0; i < subdivided.getVertices().size(); i++) {
        
        ofVec3f v = subdivided.getVertex(i);
        ofVec3f udsv = undeformedSubdivided.getVertex(i);
        ofVec3f c = undeformedSubdivided.getCentroid();
        
        v.z = (udsv.distance(c)*0.01);
        
        subdivided.setVertex(i, v);
        
    }
    
}

void Puppet::nextFrame() {
    
    if(playingForwards) {
        
        currentFrame++;
        
        if(currentFrame >= cachedFrames.size()) {
            playingForwards = false;
            currentFrame = cachedFrames.size()-1;
        }
        
    } else {
        
        currentFrame--;
        
        if(currentFrame < 0) {
            playingForwards = true;
            currentFrame = 0;
        }
        
    }
    
}

void Puppet::drawAsControllable(bool isSelected, bool isBeingRecorded) {
    
    // draw the subdivided mesh textured with our image
    
    ofPushMatrix();
    ofRotate(rotation, 0, 0, 1);
    ofTranslate(position.x, position.y);
    
    if(isBeingRecorded) {
        ofSetColor(255,0,0);
    } else if(isSelected) {
        float flash = abs(sin(ofGetElapsedTimef()*3))*50+150;
        ofSetColor(255,255,255,flash);
    } else {
        ofSetColor(255,255,255);
    }
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    image.bind();
    glEnable(GL_DEPTH_TEST);
    subdivided.drawFaces();
    image.unbind();
    glDisable(GL_DEPTH_TEST);
    
    if(isSelected) {
        
        // draw wireframe
        glLineWidth(1.0);
        ofSetColor(ofColor(30,200,255));
        meshDeformer.getDeformedMesh().drawWireframe();
        
        // draw control points
        for(int i = 0; i < expressionZones.size(); i++) {
            ofVec3f v = meshDeformer.getDeformedMesh().getVertex(expressionZones[i].meshIndex);
            
            if(expressionZones[i].isAnchorPoint) {
                
                ofSetColor(255, 0, 255);
                ofCircle(v, 7);
                
            } else if(expressionZones[i].leapFingerID != -1) {
                
                ofSetColor(ofColor::orangeRed);
                ofCircle(v, 10);
                
                ofSetColor(255, 255, 0);
                ofCircle(v, 7);
                
                string s = ofToString(expressionZones[i].leapFingerID);
                ofSetColor(0, 0, 0);
                ofDrawBitmapString(s, v.x-4, v.y+5);
                
            } else {
                
                ofSetColor(255, 255, 0);
                ofCircle(v, 7);
                
            }
            
            // draw bones
            if(expressionZones[i].parentEzone != -1) {
                ofVec3f fromVertex = meshDeformer.getDeformedMesh().getVertex(expressionZones[i].meshIndex);
                ofVec3f toVertex = meshDeformer.getDeformedMesh().getVertex(expressionZones[i].parentEzone);
                
                ofSetColor(255, 255, 0);
                ofSetLineWidth(2);
                ofLine(fromVertex.x, fromVertex.y, toVertex.x, toVertex.y);
                ofSetLineWidth(1);
            }
        }
    }
    
    ofPopMatrix();
    
}
void Puppet::drawAsRecording(bool isSelected) {
    
    ofPushMatrix();
    ofVec3f p = cachedFrames[currentFrame].position + position;
    ofRotate(cachedFrames[currentFrame].rotation, 0, 0, 1);
    ofTranslate(p.x,p.y);
    
    ofSetColor(255,255,255);
    image.bind();
    cachedFrames[currentFrame].mesh.drawFaces();
    image.unbind();
    
    if(isSelected) {
        ofSetColor(ofColor(30,200,255));
        cachedFrames[currentFrame].mesh.drawWireframe();
    }
    
    ofSetColor(255,255,255);
    
    ofPopMatrix();
    
}

