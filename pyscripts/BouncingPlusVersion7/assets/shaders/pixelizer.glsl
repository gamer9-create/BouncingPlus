#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform int renderWidth=800;
uniform int renderHeight=450;
uniform float pixelSize=2;

void main()
{
    float dx = pixelSize * (1.0 / renderWidth);
    float dy = pixelSize * (1.0 / renderHeight);

    vec2 coord = vec2(dx*floor(fragTexCoord.x/dx), dy*floor(fragTexCoord.y/dy));

    vec3 tc = texture(texture0, coord).rgb;

    finalColor = vec4(tc, 1.0);
}