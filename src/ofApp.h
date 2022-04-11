#pragma once

#include "ofMain.h"
#include "ofxPixelBuffer.h"

class ofApp : public ofBaseApp{
	public:
		
	void setup();
	void update();
    
    void DrawPrettySense();
    void DrawAgents();
    void DrawSense();
    void UpdateAgentPositions();
    
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

    ofShader display_shader;
    ofShader diffuse_shader;
    ofShader agent_update_shader;
    ofShader point_shader;
    ofFbo agent_fbo;
    ofFbo sense_fbo;
    ofFbo last_sense_fbo;
    ofFbo update_fbo;
    ofxPixelBufferFloat update_pbo;
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
    size_t count;
    size_t update_fbo_width;  // Used to not exceed the fbo dimensions.
    
    struct RenderTimes
    {
        static const double UPDATE_RATE;
        
        std::chrono::duration<double> agent_vbo;
        std::chrono::duration<double> draw_current_sense;
        std::chrono::duration<double> update_agents;
        std::chrono::duration<double> draw_to_screen;
        std::chrono::duration<double> test_time;
        
        friend std::ostream& operator<<( std::ostream& os, const RenderTimes& times );
        
        void update_agent_vbo( std::chrono::duration<double> time )
        {
            agent_vbo = agent_vbo * ( 1. - UPDATE_RATE ) + time * UPDATE_RATE;
        }
        
        void update_draw_current_sense( std::chrono::duration<double> time )
        {
            draw_current_sense = draw_current_sense * ( 1. - UPDATE_RATE ) + time * UPDATE_RATE;
        }
        
        void update_update_agents( std::chrono::duration<double> time )
        {
            update_agents = update_agents * ( 1. - UPDATE_RATE ) + time * UPDATE_RATE;
        }
        
        void update_draw_to_screen( std::chrono::duration<double> time )
        {
            draw_to_screen = draw_to_screen * ( 1. - UPDATE_RATE ) + time * UPDATE_RATE;
        }
        
        void update_test_time( std::chrono::duration<double> time )
        {
            test_time = test_time * ( 1. - UPDATE_RATE ) + time * UPDATE_RATE;
        }
    };
    
    RenderTimes mRenderTimes;
    std::chrono::steady_clock::time_point mLastFrameTime;
    bool mRunning = true;
};
