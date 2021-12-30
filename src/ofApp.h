#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{
	public:
		
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

    ofShader agent_render_shader;
    ofShader diffuse_shader;
    ofShader agent_update_shader;
    ofShader point_shader;
    ofFbo agent_fbo;
    ofFbo sense_fbo;
    ofFbo last_sense_fbo;
    ofFbo update_fbo;
    ofVbo agent_vbo;
    ofImage image;
    ofTexture agent_texture;
    
    ofVec2f screen_size;
    
    struct Agent {
        float x;
        float y;
        float heading;
        float _padding;  // For copying RGBA data to this
    };
    std::vector<Agent> agents;
    std::vector<glm::vec2> points;
    size_t count;
};
