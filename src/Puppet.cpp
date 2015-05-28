#include "Puppet.h"

void Puppet::load(string path) {
    
    // load image
    image.loadImage(path + "/image.png");
    
    // load mesh
    mesh.load(path + "/mesh.ply");
    puppet.setup(mesh);
    regenerateSubdivisionMesh();
    
    // load control points and their oscnamespaces and leapfingercontollers
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
                
                addNamespaceToExpressionZone(expressionZoneIndex, namesp);
                
                expressionZones.popTag();
                
            }
            
            // load this expressionzone's leapfingerconrollers
            
            int nFingerControllers = expressionZones.getNumTags("leapFingerController");
            for(int j = 0; j < nFingerControllers; j++) {
                
                expressionZones.pushTag("leapFingerController", j);
                
                int fingerID = expressionZones.getValue("fingerID", -1);
                
                LeapFingerController fingerController;
                fingerController.fingerID = fingerID;
                
                addFingerControllerToExpressionZone(expressionZoneIndex, fingerController);
                
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
        
        for(int j = 0; j < expressionZones[i].oscNamespaces.size(); j++) {
            
            expressionZonesXML.addTag("oscNamespace");
            expressionZonesXML.pushTag("oscNamespace",j);
            
            OSCNamespace namesp = expressionZones[i].oscNamespaces[j];
            expressionZonesXML.addValue("message", namesp.message);
            expressionZonesXML.addValue("controlType", namesp.controlType);
            
            expressionZonesXML.popTag();
            
        }
        
        expressionZonesXML.popTag();
        
    }
    expressionZonesXML.saveFile(path + "/expressionZones.xml");
    
    
    // todo: save leapControl mappings
    
    // todo: save matrices that svd calculates to allow near-instant loading of puppets
    
    
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

void Puppet::draw(bool drawWireframe) {
    
    // draw the subdivided mesh textured with our image
    
    image.bind();
    subdivided.drawFaces();
    image.unbind();
    
    // draw the wireframe as well
    
    if(drawWireframe) {
        glLineWidth(1.0);
        ofSetColor(0,255,50);
        puppet.getDeformedMesh().drawWireframe();
    }

    // debug stuff
    
    puppet.drawControlPoints();
    
}

void Puppet::regenerateSubdivisionMesh() {
    
    butterfly.topology_start(mesh);
    
    for(int i = 0; i < subs; i++) {
        butterfly.topology_subdivide_boundary();
    }
    
    subdivided = butterfly.topology_end();
    
}

void Puppet::addExpressionZone(int meshIndex) {
    
    puppet.setControlPoint(meshIndex);
    
    ExpressionZone newExpressionZone;
    newExpressionZone.meshIndex = meshIndex;
    expressionZones.push_back(newExpressionZone);
    
}

void Puppet::removeExpressionZone(int meshIndex) {
    
    // todo
    
}

void Puppet::addNamespaceToExpressionZone(int meshIndex, OSCNamespace namesp) {
    
    bool foundexpressionZoneAtIndex = false;
    
    for(int i = 0; i < expressionZones.size(); i++) {
        if(expressionZones[i].meshIndex == meshIndex) {
            expressionZones[i].oscNamespaces.push_back(namesp);
            foundexpressionZoneAtIndex = true;
            break;
        }
    }
    
    // warn if an invalid index was given to us
    
    if(!foundexpressionZoneAtIndex) {
        ofLog() << "ERROR: expression zone with meshIndex " << meshIndex << " doesn't exist!";
    }
    
}

void Puppet::addFingerControllerToExpressionZone(int meshIndex, LeapFingerController fingerController) {
    
    bool foundexpressionZoneAtIndex = false;
    
    for(int i = 0; i < expressionZones.size(); i++) {
        if(expressionZones[i].meshIndex == meshIndex) {
            expressionZones[i].leapFingerControllers.push_back(fingerController);
            foundexpressionZoneAtIndex = true;
            break;
        }
    }
    
    // warn if an invalid index was given to us
    
    if(!foundexpressionZoneAtIndex) {
        ofLog() << "ERROR: expression zone with meshIndex " << meshIndex << " doesn't exist!";
    }

};