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
        
        struct Timer
        {
            using DurationCb = std::function<void(std::chrono::duration<double>)>;
            Timer( DurationCb duration_cb )
                : mDurationCb( duration_cb )
                , mStart( std::chrono::steady_clock::now() )
            {}
            
            ~Timer()
            {
                auto duration = std::chrono::steady_clock::now() - mStart;
                mDurationCb( duration );
            }
            
            DurationCb mDurationCb;
            std::chrono::steady_clock::time_point mStart;
        };
        
        struct TrackedTime
        {
            void AddDuration(  std::chrono::duration<double> time )
            {
                duration = duration * ( 1. - UPDATE_RATE ) + time * UPDATE_RATE;
            }
            
            Timer TimeThisScope()
            {
                using namespace std::placeholders;
                return Timer( std::bind( &TrackedTime::AddDuration, this, _1 ) );
            }
            
            std::chrono::duration<double> duration;
        };
        
        TrackedTime agent_vbo;
        TrackedTime draw_current_sense;
        TrackedTime update_agents;
        TrackedTime draw_to_screen;
        
        friend std::ostream& operator<<( std::ostream& os, const RenderTimes& times );
    };
    
    RenderTimes mRenderTimes;
    std::chrono::steady_clock::time_point mLastFrameTime;
    bool mRunning = true;
};
