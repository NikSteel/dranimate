#pragma once

#include <iostream>
#include <vector>

#include "ofMain.h"
#include "LeapFingerController.h"

class OSCNamespace {
    
public:
    
    std::string message;
    std::string controlType;
    
};

class ExpressionZone {
    
public:
    
    int meshIndex;
    
    vector<OSCNamespace> oscNamespaces;
    int leapFingerID;
    
    ofVec2f userControlledDisplacement;
    
    int parentEzone = -1;
    vector<int> childrenEzones;
    
    bool isAnchorPoint;
    
};

