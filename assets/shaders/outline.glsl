#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 textureSize;
uniform float outlineSize;
uniform vec4 outlineColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);   // Get texel color

    if (texel.a == 0) {
        float left_px_alpha = texture(texture0, fragTexCoord - vec2(1 / textureSize.x, 0));
    }

    finalColor =texel;
}