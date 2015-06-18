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
            ofLog() << expressionZones[i].userControlledDisplacement;
        }
        
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
            ofVec3f v = meshDeformer.getDeformedMesh().getVertex(expressionZones[i].meshIndex);
            
            ofSetColor(255, 255, 0);
            ofCircle(v, 7);
            
            if(expressionZones[i].leapFingerID != -1) {
                ofSetColor(255, 255, 255);
                Utils::hand.draw(v.x-7, v.y-7, 14, 14);
            }
            
            // draw bones
            if(expressionZones[i].parentEzone != -1) {
                ofVec3f fromVertex = meshDeformer.getDeformedMesh().getVertex(expressionZones[i].meshIndex);
                ofVec3f toVertex = meshDeformer.getDeformedMesh().getVertex(expressionZones[i].parentEzone);
                
                ofSetColor(255, 255, 0);
                ofSetLineWidth(3);
                ofLine(fromVertex.x, fromVertex.y, toVertex.x, toVertex.y);
                ofSetLineWidth(1);
            }
        }
        
        // draw contour
        //contour.draw();
        
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
            
            ExpressionZone *ezone = &expressionZones[i];
            
            if(ezone->leapFingerID != -1) {
            
                // this ezone has a finger mapping!
                // so set this ezone's displacement to that leap finger
                
                ezone->userControlledDisplacement.x =
                    ofGetWidth()/3+
                    leapFingersPositions[expressionZones[i].leapFingerID].x
                    -leapFingersCalibration[expressionZones[i].leapFingerID].x;
                
                ezone->userControlledDisplacement.y =
                    ofGetHeight()/3+
                    leapFingersPositions[expressionZones[i].leapFingerID].y
                    -leapFingersCalibration[expressionZones[i].leapFingerID].y;
                
            } else {
                
                // this ezone has no finger mapping.
                // so just set the displacement to the palm position
                
                ezone->userControlledDisplacement.x =
                ofGetWidth()/3+
                palmPosition.x
                -palmCalibration.x;
                
                ezone->userControlledDisplacement.y =
                ofGetHeight()/3+
                palmPosition.y
                -palmCalibration.y;
                
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
                
                ofLog() << ezoneParentVertexPosition;
                ofLog() << ezoneVertexPosition;
                
                float d = ezoneVertexPosition.distance(ezoneParentVertexPosition);
                
                /*
                ofVec3f diff = ezoneAbsoulteVertexPosition - ezoneParentAbsoluteVertexPosition;
                diff = diff.normalized() * d;
                diff = ezoneParentAbsoluteVertexPosition + diff;
                diff = diff - ezoneVertexPosition;
                 */
                
                ofVec3f diff;
                //diff = ezoneAbsoulteVertexPosition - ezoneParentAbsoluteVertexPosition;
                float angle = ezone->userControlledDisplacement.y*0.005;
                diff = ofVec3f(cos(angle), sin(angle), 0);
                diff.normalize();
                diff = diff * d;
                
                ezone->userControlledDisplacement = diff;
                
            }
            
        }
        
    }
    
}