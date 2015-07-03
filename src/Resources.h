#ifndef __dranimate__Resources__
#define __dranimate__Resources__

#include <stdio.h>
#include "ofMain.h"

class Resources {
  
public:
    static void loadResources();
    
    static ofImage hand;
    static ofImage grid;
    
    static ofTrueTypeFont verdana24;
    static ofTrueTypeFont verdana16;
    static ofTrueTypeFont verdana12;
    static ofTrueTypeFont verdana54;
    
};

#endif
