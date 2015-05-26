#include "Puppet.h"

void Puppet::load(string path) {
    
    // load image
    image.loadImage(path + "/image.png");
    
    // load mesh
    mesh.load(path + "/mesh.ply");
    puppet.setup(mesh);
    updateSubdivisionMesh();
    
    // load control points & osc settings
    ofxXmlSettings controlPoints;
    if(controlPoints.loadFile(path + "/controlPoints.xml")){
        
        controlPoints.pushTag("controlPoints");
        int nControlPoints = controlPoints.getNumTags("controlPoint");
        
        for(int i = 0; i < nControlPoints; i++) {
            
            controlPoints.pushTag("controlPoint", i);
            
            int controlPointIndex = controlPoints.getValue("index", 0);
            puppet.setControlPoint(controlPointIndex);
            puppet.controlPointsVector.push_back(controlPointIndex);
            
            controlPoints.popTag();
            
        }
        
        controlPoints.popTag();
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
    ofxXmlSettings controlPoints;
    controlPoints.addTag("controlPoints");
    controlPoints.pushTag("controlPoints");
    for(int i = 0; i < puppet.controlPointsVector.size(); i++){
        
        controlPoints.addTag("controlPoint");
        controlPoints.pushTag("controlPoint",i);
        
        controlPoints.addValue("index", puppet.controlPointsVector[i]);
        controlPoints.popTag();
        
    }
    controlPoints.popTag();
    controlPoints.saveFile(path + "/controlPoints.xml");
    
    
    // todo: save matrices that svd calculates to allow near-instant loading of puppets
    
}

void Puppet::setMesh(ofMesh m) {
    
    mesh = m;
    
    puppet.setup(mesh);
    updateSubdivisionMesh();
    
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

void Puppet::updateSubdivisionMesh() {
    
    butterfly.topology_start(mesh);
    
    for(int i = 0; i < subs; i++) {
        butterfly.topology_subdivide_boundary();
    }
    
    subdivided = butterfly.topology_end();
    
}

void Puppet::addControlPoint(int index,
                             string message,
                             OSCNamespace::ControlType controlType) {
    
    
    
}