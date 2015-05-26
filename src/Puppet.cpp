#include "Puppet.h"

void Puppet::load(string path) {
    
    // load image
    image.loadImage(path + "/image.png");
    
    // load mesh
    mesh.load(path + "/mesh.ply");
    puppet.setup(mesh);
    regenerateSubdivisionMesh();
    
    // load control points & osc settings
    ofxXmlSettings controlPoints;
    if(controlPoints.loadFile(path + "/controlPoints.xml")){
        
        int nControlPoints = controlPoints.getNumTags("controlPoint");
        for(int i = 0; i < nControlPoints; i++) {
            
            controlPoints.pushTag("controlPoint", i);
            
            int controlPointIndex = controlPoints.getValue("index", 0);
            
            vector<OSCNamespace> namespaces;
            
            int nNamespaces = controlPoints.getNumTags("oscNamespace");
            for(int j = 0; j < nNamespaces; j++) {
                
                controlPoints.pushTag("oscNamespace", j);
                
                string message = controlPoints.getValue("message", "");
                string controlType = controlPoints.getValue("controlType", "");
                
                OSCNamespace namesp;
                namesp.message = message;
                namesp.controlType = controlType;
                
                namespaces.push_back(namesp);
                
                controlPoints.popTag();
                
            }
            
            addControlPoint(controlPointIndex, namespaces);
            
            controlPoints.popTag();
            
        }
        
    }
    
}

void Puppet::save(string path) {
    
    ofLog() << "making new directory at: " << path;
    
    // the folder itself
    string mkdirCommandString = "mkdir " + path;
    system(mkdirCommandString.c_str());
    
    // mesh
    mesh.save(path + "/mesh.ply");
    
    // image
    image.saveImage(path + "/image.png");
    
    // control points
    ofxXmlSettings controlPointsXML;
    for(int i = 0; i < controlPoints.size(); i++){
        
        controlPointsXML.addTag("controlPoint");
        controlPointsXML.pushTag("controlPoint",i);
        
        controlPointsXML.addValue("index", controlPoints[i].index);
        
        for(int j = 0; j < controlPoints[i].oscNamespaces.size(); j++) {
            
            controlPointsXML.addTag("oscNamespace");
            controlPointsXML.pushTag("oscNamespace",j);
            
            OSCNamespace namesp = controlPoints[i].oscNamespaces[j];
            controlPointsXML.addValue("message", namesp.message);
            controlPointsXML.addValue("controlType", namesp.controlType);
            
            controlPointsXML.popTag();
            
        }
        
        controlPointsXML.popTag();
        
    }
    controlPointsXML.saveFile(path + "/controlPoints.xml");
    
    
    // todo: save matrices that svd calculates to allow near-instant loading of puppets
    
}

void Puppet::setMesh(ofMesh m) {
    
    mesh = m;
    
    puppet.setup(mesh);
    regenerateSubdivisionMesh();
    
}

void Puppet::update() {
    
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
        subdivided.drawWireframe();
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

void Puppet::addControlPoint(int index, vector<OSCNamespace> namespaces) {
    
    puppet.setControlPoint(index);
    
    ControlPoint newControlPoint;
    newControlPoint.index = index;
    newControlPoint.oscNamespaces = namespaces;
    controlPoints.push_back(newControlPoint);
    
}