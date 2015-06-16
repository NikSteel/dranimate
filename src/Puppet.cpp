#include "Puppet.h"

void Puppet::load(string path) {
    
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
    
    meshDeformer.setup(mesh);
    regenerateSubdivisionMesh();
    
}

void Puppet::setContour(ofPolyline line){
    
    contour = line;

}

void Puppet::reset() {
    
    for(int i = 0; i < expressionZones.size(); i++) {
        meshDeformer.removeControlPoint(expressionZones[i].meshIndex);
    }
    
    expressionZones.clear();
    
}

void Puppet::update() {
    
    // add displacements to puppet control points
    for(int i = 0; i < expressionZones.size(); i++) {
        meshDeformer.setControlPoint(expressionZones[i].meshIndex,
                               mesh.getVertex(expressionZones[i].meshIndex)+
                               expressionZones[i].userControlledDisplacement);
    }
    
    meshDeformer.update();
    
    // attach the subdivided mesh to the mesh deformed by the puppet
    butterfly.fixMesh(meshDeformer.getDeformedMesh(), subdivided);
    
}

void Puppet::draw(bool isSelected) {
    
    // draw the subdivided mesh textured with our image
    
    if(isSelected) {
        ofSetColor(100,100,200);
    } else {
        ofSetColor(255,255,255);
    }
    
    image.bind();
    subdivided.drawFaces();
    image.unbind();
    
    // draw the wireframe & control points as well
    
    if(isSelected) {
        
        // draw wireframe
        glLineWidth(1.0);
        ofSetColor(0,255,50);
        meshDeformer.getDeformedMesh().drawWireframe();
        
        // draw control points
        for(int i = 0; i < expressionZones.size(); i++) {
            ofSetColor(255, 255, 0);
            ofCircle(meshDeformer.getDeformedMesh().getVertex(expressionZones[i].meshIndex), 5);
        }
        
        // draw contour
        contour.draw();
        
    }
    
}

void Puppet::regenerateSubdivisionMesh() {
    
    butterfly.topology_start(mesh);
    
    for(int i = 0; i < MESH_SMOOTH_SUBDIVISIONS; i++) {
        butterfly.topology_subdivide_boundary();
    }
    
    subdivided = butterfly.topology_end();
    
}

void Puppet::addExpressionZone(int meshIndex) {
    
    meshDeformer.setControlPoint(meshIndex);
    
    ExpressionZone newExpressionZone;
    newExpressionZone.meshIndex = meshIndex;
    newExpressionZone.leapFingerID = -1;
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
    
    // this is a bit messy, should fix it
    
    if(!isPaused) {
        
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

}

void Puppet::recieveLeapData(vector<ofVec3f> leapFingersPositions,
                             vector<ofVec3f> leapFingersCalibration,
                             ofVec3f palmPosition,
                             ofVec3f palmCalibration) {
    
    // this is a bit messy, should fix it
    
    if(!isPaused) {
    
        for(int i = 0; i < expressionZones.size(); i++) {
            
            if(expressionZones[i].leapFingerID != -1) {
            
                expressionZones[i].userControlledDisplacement.x =
                    ofGetWidth()/3+
                    leapFingersPositions[expressionZones[i].leapFingerID].x
                    -leapFingersCalibration[expressionZones[i].leapFingerID].x;
                
                expressionZones[i].userControlledDisplacement.y =
                    ofGetHeight()/3+
                    leapFingersPositions[expressionZones[i].leapFingerID].y
                    -leapFingersCalibration[expressionZones[i].leapFingerID].y;
                
                ofLog()<<leapFingersCalibration[expressionZones[i].leapFingerID].y;
                
            } else {
                
                expressionZones[i].userControlledDisplacement.x =
                ofGetWidth()/3+
                palmPosition.x
                -palmCalibration.x;
                
                expressionZones[i].userControlledDisplacement.y =
                ofGetHeight()/3+
                palmPosition.y
                -palmCalibration.y;
                
            }
            
        }
        
    }
    
}