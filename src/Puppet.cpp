#include "Puppet.h"

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