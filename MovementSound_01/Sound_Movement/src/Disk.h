#ifndef _Disk_
#define _Disk_

#include "ofMain.h"

class Disk {
	ofVec2f position;
	float radius;

public:
	Disk();
	Disk(ofVec2f pos, float rad);
	void draw();

};

#endif