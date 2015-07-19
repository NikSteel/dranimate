#ifndef __dranimate__globals__
#define __dranimate__globals__

#define OSC_DEFAULT_PORT 8000

#define LAUNCH_SETTINGS_PATH "settings/launch.xml"
#define MESHGEN_SETTINGS_PATH "settings/meshgen.xml"

#define LAYER_SYPHON_SERVER_NAME "Dranimate Layer "

#define LOAD_DEMO_PUPPET true
#define DEMO_PUPPET_PATH "puppets/demo-killing-ashkeboos"

#define BG_BRIGHTNESS 0

#define VERTEX_SELECTION_RADIUS 20

#define DEFAULT_PUPPET_NAME "New-Puppet"
#define DEFAULT_SCENE_NAME "New-Scene"

#define LEAP_CLICK_DELAY 20

#define NUM_BUTTERFLY_SUBDIVISIONS 2

#define VERTEX_BORDER_RADIUS 10
#define VERTEX_RADIUS 5
#define SELECTED_VERTEX_RADIUS 8
#define SELECTED_VERTEX_COLOR ofColor(255,0,100)
#define HOVERED_VERTEX_BORDER_COLOR ofColor(255,0,0,100)
#define HOVERED_VERTEX_COLOR ofColor(255,0,0,100)
#define BONE_COLOR ofColor(255, 255, 0)
#define BONE_LINE_WIDTH 3

#define WIREFRAME_COLOR ofColor(30,200,255)
#define RECORDING_COLOR ofColor(255,0,0)

#define EZONE_COLOR ofColor(255, 255, 0)
#define EZONE_BORDER_COLOR ofColor::orangeRed
#define EZONE_RADIUS 7
#define EZONE_BORDER_RADIUS 10

#define CONTOUR_COLOR ofColor(255,0,0)
#define UNUSED_CONTOUR_COLOR ofColor(155,100,100)
#define CONTOUR_LINE_WIDTH 5

#define LAYER_PREVIEW_HEIGHT 60
#define LAYER_PREVIEW_WIDTH 90
#define LAYER_PREVIEW_PUPPET_SCALE 0.05
#define LAYER_PREVIEW_TEXT_OFFSET 3
#define ACTIVE_LAYER_PREVIEW_COLOR ofColor(255,255,255,50)

#endif