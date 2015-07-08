#include "Puppet.h"

void Puppet::load(string path) {
    
    palmControlsPuppet = 0;
    
    isBeingEdited = false;
    
    // load image
    image.loadImage(path + "/image.png");
    
    // load mesh
    mesh.load(path + "/mesh.ply");
    originalMesh.load(path + "/mesh.ply");
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
    originalMesh.save(path + "/mesh.ply");
    
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

void Puppet::setImage(ofImage img, bool resizeImage) {
    
    image = img;
    
    // scale down image to a good size for the mesh generator
    
    //float whRatio = (float)image.width/(float)image.height;
    //image.resize(IMAGE_BASE_SIZE*whRatio, IMAGE_BASE_SIZE);
    
    if(resizeImage) {
        ofVec2f wh = ofVec2f(image.width,image.height);
        wh.normalize();
        image.resize(wh.x*IMAGE_BASE_SIZE, wh.y*IMAGE_BASE_SIZE);
    }
    
}

void Puppet::setMesh(ofMesh m) {
    
    mesh = m;
    originalMesh = m;
    
    meshDeformer.setup(mesh);
    regenerateSubdivisionMesh();
    
}

void Puppet::setContour(ofPolyline line) {
    
    contour = line;

}

void Puppet::addCenterpoint() {

    ofVec3f c = mesh.getCentroid();
    addExpressionZone(Utils::getClosestIndex(mesh, c.x,c.y, INT_MAX));
    
}

void Puppet::reset() {
    
    removeAllExpressionZones();
    setMesh(originalMesh);
    
}

void Puppet::update() {
    
    // update which palm controls this puppet
    
    palmControlsPuppet = 1;
    
    for(int i = 0; i < expressionZones.size(); i++) {
        
        int fid = expressionZones[i].leapFingerID;
        
        // if there exists an expression zone controlled by the right hand,
        // make the right palm control the overall position of the puppet.
        if(fid != -1 && fid < 5) {
            palmControlsPuppet = 0;
        }
        
    }
    
    // do ofxPuppet puppeteering stuff (if there is more than one point;
    // as rigid as possible freaks out with onely one control point.)
    
    if(expressionZones.size() > 1) {
        
        // add displacements to puppet control points
        for(int i = 0; i < expressionZones.size(); i++) {
            
            if(expressionZones[i].parentEzone == -1 || isBeingEdited) {
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
    
    // update mesh vertex depths (so that we avoid z-fighting issues)
    updateMeshVertexDepths();
    
}

void Puppet::draw(bool isSelected, bool isBeingRecorded) {
    
    // draw the subdivided mesh textured with our image
    
    if(isBeingRecorded) {
        ofSetColor(255,155,155);
    } else if(isSelected) {
        float flash = abs(sin(ofGetElapsedTimef()*3))*50+150;
        ofSetColor(255,255,255,flash);
    } else {
        ofSetColor(255,255,255);
    }
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    image.bind();
    glEnable(GL_DEPTH_TEST);
        subdivided.drawFaces();
    image.unbind();
    glDisable(GL_DEPTH_TEST);
    
    // draw the wireframe & control points as well
    
    if(isSelected && !isBeingRecorded && isBeingEdited) {
        
        // draw wireframe
        glLineWidth(1.0);
        //float flash = abs(sin(ofGetElapsedTimef()*2)*100)+100;
        //ofSetColor(100,255,100,flash);
        ofSetColor(ofColor::orangeRed);
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
        
        // draw contour
        //contour.draw();
        
    }
    
}

void Puppet::resetPose() {
    
    for(int i = 0; i < expressionZones.size(); i++) {
        expressionZones[i].userControlledDisplacement = ofVec2f(0,0);
    }
    setMesh(originalMesh);
    
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
        v.z = -(udsv.x*0.01);
        subdivided.setVertex(i, v);
        
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

void Puppet::recieveLeapData(LeapDataHandler *leap) {
    
    if(leap->calibrated && !isBeingEdited) {
    
        for(int i = 0; i < expressionZones.size(); i++) {
            
            ExpressionZone *ezone = &expressionZones[i];
            
            if(ezone->isAnchorPoint) {
              
                // ezone doesn't move
                
            } else if(ezone->leapFingerID != -1) {
            
                // this ezone has a finger mapping!
                // so set this ezone's displacement to that leap finger
                
                ofVec3f calibratedFinger = leap->getCalibratedFingerPosition(expressionZones[i].leapFingerID);
                
                ezone->userControlledDisplacement.x = calibratedFinger.x;
                ezone->userControlledDisplacement.y = -calibratedFinger.y;
                
            } else {
                
                // this ezone has no finger mapping.
                // so just set the displacement to the palm position
                
                ofVec3f calibratedPalm = leap->getCalibratedPalmPosition(palmControlsPuppet);
                
                ezone->userControlledDisplacement.x = calibratedPalm.x;
                ezone->userControlledDisplacement.y = -calibratedPalm.y;
                
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
        
        for(int i = 0; i < expressionZones.size(); i++) {
            
            ExpressionZone *ezone = &expressionZones[i];
            
            ezone->userControlledDisplacement.x = 0;
            ezone->userControlledDisplacement.y = 0;
                
            
        }
        
    }
}