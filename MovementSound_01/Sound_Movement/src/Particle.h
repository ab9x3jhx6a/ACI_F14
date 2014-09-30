#ifndef __ParticleMadness__Particle__
#define __ParticleMadness__Particle__

#include "ofMain.h"

class Particle {
	ofVec2f position, velocity;
	int red;
	int green;
	int blue;
	bool destroy;

public: 
	Particle();
	Particle(ofVec2f pos, ofVec2f vel, int r, int g, int b);
	void draw();
	void update();
	void applyForces();
	bool death();


};

#endif