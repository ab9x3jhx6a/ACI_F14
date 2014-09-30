#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "Particle.h"
#include "Disk.h"
#include <vector>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		vector <Particle> emitter;
		
		ofImage dj;

		int r;
		int g;
		int b;
		int sr;
		int sg;
		int sb;
		int deg;
		bool pause;
		int mousex;
		int mousey;
		int num_song;
		std::vector<string> songs;

		ofSoundPlayer disk, scratch, applause;


		
};
