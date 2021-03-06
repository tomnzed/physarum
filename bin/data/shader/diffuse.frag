#version 150

uniform sampler2DRect tex0;
uniform float blurAmnt;

in vec2 texCoordVarying;
out vec4 outputColor;

uniform sampler2DRect agentTexture;
uniform sampler2DRect senseTexture;
uniform vec2 simSize;
uniform float maxChemoAttract;
uniform float depositChemoAttract;
uniform float chemoAttractDecayFactor;


vec3 blur9(sampler2DRect image, vec2 uv, vec2 direction, vec2 resolution) {
  vec3 color = vec3(0.0);
  vec2 off1 = vec2(1.3846153846) * direction;
  vec2 off2 = vec2(3.2307692308) * direction;
  color += texture(image, uv * resolution).rgb * 0.2270270270;
  color += texture(image, uv * resolution + (off1)).rgb * 0.3162162162;
  color += texture(image, uv * resolution - (off1)).rgb * 0.3162162162;
  color += texture(image, uv * resolution + (off2)).rgb * 0.0702702703;
  color += texture(image, uv * resolution - (off2)).rgb * 0.0702702703;
  return color;
}

vec4 blur(sampler2DRect image, vec2 resolution) {

    vec2 uv = gl_FragCoord.st / resolution;
    vec3 color = vec3(0.0);
    
    // Blur calculations
    int sample_count = 0;

    color += blur9(image, uv, vec2(1,0), resolution);
    sample_count++;
    color += blur9(image, uv, vec2(0,1), resolution);
    sample_count++;
    
    color /= sample_count;
    return vec4( color, 1.);
}

void main()
{
    // diffuse the chemo attractant
    vec4 sense_tex = blur(senseTexture, simSize );

    // decay
    sense_tex.rgb *= ( 1. - chemoAttractDecayFactor );

    // deposit from agent
    vec4 agent_tex = texture(agentTexture, gl_FragCoord.st );
    if( agent_tex.r > 0. )
    {
        sense_tex.rgb += depositChemoAttract / maxChemoAttract;
    }

    outputColor = vec4( sense_tex.rgb, 1. );
}
