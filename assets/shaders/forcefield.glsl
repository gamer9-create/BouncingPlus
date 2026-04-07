#version 330

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

void main()
{
    bool edge = false;

    if (texture(texture0, fragTexCoord - vec2(1 / renderWidth)).a == 0 || texture(texture0, fragTexCoord + vec2(1 / renderWidth)).a == 0)

    if (!edge) {
        finalColor = texture(texture0,fragTexCoord);
    } else {
        finalColor = vec4(0,1,0,1);
    }

}
