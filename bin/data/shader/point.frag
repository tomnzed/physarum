#version 150

uniform sampler2DRect tex0;

in vec2 texCoordVarying;
out vec4 outputColor;

void main (void) {
    float d = 1.-length( .5 - gl_PointCoord.xy );
    outputColor=vec4( d, d, d, 1.);
}