#version 330

struct Distortion {
    vec2 position;
    float strength;
    float radius;
};

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

uniform Distortion distortions[100];
uniform int distortionCount;

void main()
{
    vec2 screenSize = vec2(textureSize(texture0, 0));
    vec2 pixelUVPos = vec2(fragTexCoord.x, 1 - fragTexCoord.y) * screenSize;

    for (int i = 0; i < distortionCount; i++) {
        Distortion d = distortions[i];

        float dist = distance(pixelUVPos, d.position);
        if (dist <= d.radius) {
            float percent = pow(1.0 - (dist / d.radius), 2);
            vec2 dir = normalize(pixelUVPos - d.position);

            pixelUVPos -= dir * percent * d.strength;
        }
    }

    finalColor = texture(texture0, vec2((pixelUVPos / screenSize).x, 1 - (pixelUVPos / screenSize).y));
}