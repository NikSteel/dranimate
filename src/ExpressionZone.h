#ifndef __dranimate__ControlPoint__
#define __dranimate__ControlPoint__

#include <iostream>
#include <vector>

#include "ofMain.h"
#include "OSCNamespace.h"
#include "LeapFingerController.h"

class ExpressionZone {
    
public:
    
    bool isRoot;
    
    int meshIndex;
    
    vector<OSCNamespace> oscNamespaces;
    int leapFingerID;
    
    ofVec2f userControlledDisplacement;
    
};

#endif
