#include "Puppet.h"

void Puppet::load(string path) {
    
    // load image
    image.loadImage(path + "/image.png");
    
    // load mesh
    mesh.load(path + "/mesh.ply");
    puppet.setup(mesh);
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
        
        expressionZonesXML.popTag();
        
        expressionZonesXML.popTag();
        
    }
    expressionZonesXML.saveFile(path + "/expressionZones.xml");
    
    
    // todo: save data that svd calculates to allow near-instant loading of puppets
    
    
    ofLog() << "puppet saved to  " << path << "!";
    
}

void Puppet::setImage(ofImage img) {
    
    image = img;
    
    // scale down image to a good size for the mesh generator
    
    float whRatio = (float)image.width/(float)image.height;
    if(image.width > image.height) {
        image.resize(IMAGE_BASE_SIZE*whRatio, IMAGE_BASE_SIZE);
    } else {
        whRatio = (float)image.height/(float)image.width;
        image.resize(IMAGE_BASE_SIZE, IMAGE_BASE_SIZE*whRatio);
    }
    
}

void Puppet::setMesh(ofMesh m) {
    
    mesh = m;
    
    puppet.setup(mesh);
    regenerateSubdivisionMesh();
    
}

void Puppet::reset() {
    
    for(int i = 0; i < expressionZones.size(); i++) {
        puppet.removeControlPoint(expressionZones[i].meshIndex);
    }
    
    expressionZones.clear();
    
}

void Puppet::update() {
    
    // add displacements to puppet control points
    for(int i = 0; i < expressionZones.size(); i++) {
        puppet.setControlPoint(expressionZones[i].meshIndex,
                               mesh.getVertex(expressionZones[i].meshIndex)+
                               expressionZones[i].userControlledDisplacement);
    }
    
    puppet.update();
    
    // attach the subdivided mesh to the mesh deformed by the puppet
    butterfly.fixMesh(puppet.getDeformedMesh(), subdivided);
    
}

void Puppet::draw(bool drawWireframe, bool transformActive) {
    
    // draw the subdivided mesh textured with our image
    
    image.bind();
    if(drawWireframe) ofSetColor(100,100,200);
    subdivided.drawFaces();
    image.unbind();
    
    // draw the wireframe & control points as well
    
    if(drawWireframe) {
        
        glLineWidth(1.0);
        ofSetColor(0,255,50);
        puppet.getDeformedMesh().drawWireframe();
        
        for(int i = 0; i < expressionZones.size(); i++) {
            ofSetColor(255, 255, 0);
            ofCircle(puppet.getDeformedMesh().getVertex(expressionZones[i].meshIndex), 5);
        }
        
    }
    
    if(transformActive) {
        
        ofSetColor(255,255,0);
        ofCircle(untransformedMesh.getCentroid().x,
                 untransformedMesh.getCentroid().y, 5);
    }
    
}

void Puppet::beginScale() {
    
    // make a copy of the origial mesh so we can reference those
    // original vertex positions when scaling
    
    untransformedMesh = mesh;
    
}

void Puppet::beginRotate() {
    
    //todo
    
}

void Puppet::scaleMesh(ofVec2f origin, ofVec2f mouse) {
    
    // scale mesh
    
    float scaleDistance = mouse.distance(origin);
    ofVec3f centroid = untransformedMesh.getCentroid();
    
    ofVec3f mouseFromCentroid = mouse - centroid;
    ofVec3f originFromCentroid = origin - centroid;
    float scaleRatio = (mouseFromCentroid/originFromCentroid).length();
    
    
    for(int i = 0; i < mesh.getVertices().size(); i++) {
        ofVec3f v = untransformedMesh.getVertex(i);
        ofVec3f distFromCentroid = v - origin;
        float d = distFromCentroid.length();
        ofVec3f newPos = distFromCentroid.normalize() * (d * scaleRatio);
        mesh.setVertex(i, origin+newPos);
    }
    
    ofLog() << origin.distance(mouse);
    
    // recalc puppet
    puppet.setup(mesh);
    update();
    
}

void Puppet::rotateMesh(ofVec2f origin, ofVec2f mouse) {
 
    // todo
    
}

void Puppet::regenerateSubdivisionMesh() {
    
    butterfly.topology_start(mesh);
    
    for(int i = 0; i < MESH_SMOOTH_SUBDIVISIONS; i++) {
        butterfly.topology_subdivide_boundary();
    }
    
    subdivided = butterfly.topology_end();
    
}

void Puppet::addExpressionZone(int meshIndex) {
    
    puppet.setControlPoint(meshIndex);
    
    ExpressionZone newExpressionZone;
    newExpressionZone.meshIndex = meshIndex;
    newExpressionZone.leapFingerID = -1;
    expressionZones.push_back(newExpressionZone);
    
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

void Puppet::removeExpressionZone(int meshIndex) {
    
    // todo
    
}

void Puppet::makeExpressionZoneRoot(int meshIndex) {
    
    // get rid of current root
    for(int i = 0; i < expressionZones.size(); i++) {
        expressionZones[i].isRoot = false;
    }
    
    // set new root
    getExpressionZone(meshIndex)->isRoot = true;
    
}

void Puppet::recieveOSCMessage(ofxOscMessage message, float value) {
    
    for(int i = 0; i < expressionZones.size(); i++) {
        
        ExpressionZone expressionZone = expressionZones[i];
        vector<OSCNamespace> namespaces = expressionZone.oscNamespaces;
        
        ofVec3f expressionZonePosition = mesh.getVertex(expressionZone.meshIndex);
        
        for(int j = 0; j < namespaces.size(); j++) {
            
            OSCNamespace namesp = namespaces[j];
            
            if(namesp.message == message.getAddress()) {
                
                if(namesp.controlType == "x") {
                    expressionZones[i].userControlledDisplacement.y = value;
                } else if(namesp.controlType == "y") {
                    expressionZones[i].userControlledDisplacement.x = value;
                }
                
            }
            
        }
        
    }
    
}

void Puppet::recieveLeapData(vector<ofVec3f> leapFingersPositions,
                             vector<ofVec3f> leapFingersCalibration,
                             ofVec3f palmPosition,
                             ofVec3f palmCalibration) {
    
    for(int i = 0; i < expressionZones.size(); i++) {
        
        if(expressionZones[i].leapFingerID != -1) {
        
            expressionZones[i].userControlledDisplacement.x =
                ofGetWidth()/3+
                leapFingersPositions[expressionZones[i].leapFingerID].x
                -leapFingersCalibration[expressionZones[i].leapFingerID].x;
            
            expressionZones[i].userControlledDisplacement.y =
                -ofGetHeight()/3+
                leapFingersPositions[expressionZones[i].leapFingerID].y;
                -leapFingersCalibration[expressionZones[i].leapFingerID].y;
            
        } else {
            
            expressionZones[i].userControlledDisplacement.x =
            ofGetWidth()/3+
            palmPosition.x
            -palmCalibration.x;
            
            expressionZones[i].userControlledDisplacement.y =
            -ofGetHeight()/3+
            palmPosition.y;
            -palmCalibration.y;
            
        }
        
    }
    
}