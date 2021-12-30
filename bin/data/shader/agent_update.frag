#version 150

uniform sampler2DRect tex0;
uniform float blurAmnt;

in vec2 texCoordVarying;
out vec4 outputColor;

uniform sampler2DRect agentTexture;  // Texture to update the agent positions using

uniform sampler2DRect senseTexture;  // Texture to sense from
uniform vec2 screenSize;

uniform float senseAngle;
uniform float rotateAngle;
uniform float senseOffset;
uniform float stepSize;

// @todo This could be improved 
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec3 agent = texture( agentTexture, gl_FragCoord.st ).rgb;

    float head_a = agent.b * 2 * radians(180);
    float head_l = head_a - radians( senseAngle );
    float head_r = head_a + radians( senseAngle );

    vec2 dir_a = vec2( sin( head_a ), cos( head_a ) );
    vec2 dir_l = vec2( sin( head_l ), cos( head_l ) );
    vec2 dir_r = vec2( sin( head_r ), cos( head_r ) );

    // Sense ahead
    // @todo check texture sampling is repeating (not clamping)
    vec2 pos_a = vec2( agent.r, agent.g ) * screenSize + dir_a * senseOffset;
    float sense_a = texture( senseTexture, pos_a ).r;

    // Sense left
    vec2 pos_l = vec2( agent.r, agent.g ) * screenSize + dir_l * senseOffset;
    float sense_l = texture( senseTexture, pos_l ).r;

    // Sense right
    vec2 pos_r = vec2( agent.r, agent.g ) * screenSize  + dir_r * senseOffset;
    float sense_r = texture( senseTexture, pos_r ).r;

    vec2 dir = dir_a;
    float head = head_a;

    if( sense_l > sense_a && sense_a >= sense_r )
    {
        dir = dir_l;
        head = head_l;
    }
    else if( sense_r > sense_a && sense_a >= sense_l )
    {
        dir = dir_r;
        head = head_r;
    }
    else if( sense_r > sense_a && sense_l > sense_a )
    {
        if( rand(pos_a) > 0.5 )
        {
            dir = dir_r;
            head = head_r;
        }
        else
        {
            dir = dir_l;
            head = head_l;
        }
    }


    // @todo factor screenSize here
 //   agent.rg += dir * stepSize * 0.01;
    agent.rg += dir * vec2( stepSize, stepSize ) / screenSize;

    if( agent.r > 1. )
    {
        agent.r -= 1.;
    }
    else if( agent.r < 0. )
    {
        agent.r += 1.;
    }

    if( agent.g > 1. )
    {
        agent.g -= 1.;
    }
    else if( agent.g < 0. )
    {
        agent.g += 1.;
    }

    agent.b = head / ( 2 * radians( 180 ) );

    outputColor = vec4( agent, 1. );
}
