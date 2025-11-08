#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform vec2 textureSize;
uniform float outlineSize;
uniform float threshold = 0.2;
uniform vec4 outlineColor;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);   // Get texel color

    if (texel.a < threshold) {
        float la = texture(texture0, fragTexCoord - vec2(outlineSize / textureSize.x, 0)).a;
        float ra = texture(texture0, fragTexCoord + vec2(outlineSize / textureSize.x, 0)).a;
        float ua = texture(texture0, fragTexCoord - vec2(0, outlineSize / textureSize.y)).a;
        float da = texture(texture0, fragTexCoord + vec2(0, outlineSize / textureSize.y)).a;

        float la2 = texture(texture0, fragTexCoord + vec2(outlineSize / textureSize.x, outlineSize / textureSize.y)).a;
        float ra2 = texture(texture0, fragTexCoord + vec2(outlineSize / textureSize.x, -outlineSize / textureSize.y)).a;
        float ua2 = texture(texture0, fragTexCoord + vec2(-outlineSize / textureSize.x, outlineSize / textureSize.y)).a;
        float da2 = texture(texture0, fragTexCoord + vec2(-outlineSize / textureSize.x, -outlineSize / textureSize.y)).a;
        if (la > threshold || ra > threshold || ua > threshold || da > threshold ||
        la2 > threshold || ra2 > threshold || ua2 > threshold || da2 > threshold) {
            texel = outlineColor;
        }
    }

    finalColor =texel;
}