#version 330

float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);

    float res = mix(
        mix(rand(ip),rand(ip+vec2(1.0,0.0)),u.x),
        mix(rand(ip+vec2(0.0,1.0)),rand(ip+vec2(1.0,1.0)),u.x),u.y);
    return max(min(res*res, 1.0),-1.0);
}

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

uniform int renderWidth = 800;
uniform int renderHeight = 450;
uniform float time = 0;
uniform float tileX = 0;
uniform float tileY = 0;

void main()
{
    vec2 t = vec2(tileX/10.0,tileY/10.0);
    finalColor = vec4(t, 0.1, noise1(t) + sin(time*2));
}
