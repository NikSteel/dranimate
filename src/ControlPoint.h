#ifndef __dranimate__ControlPoint__
#define __dranimate__ControlPoint__

#include <iostream>
#include <vector>

#include "OSCNamespace.h"

class ControlPoint {
    
public:
    
    int index;
    vector<OSCNamespace> oscNamespaces;
    
};

#endif
