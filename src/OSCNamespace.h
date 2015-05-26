#ifndef __dranimate__OSCNamespace__
#define __dranimate__OSCNamespace__

#include <iostream>

class OSCNamespace {
    
public:
    
    enum ControlType {
        X,
        Y,
    };
    
    std::string name;
    ControlType controlType;
    
};


#endif
