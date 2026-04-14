#version 130

precision mediump float;

struct Distortion {
    vec2 position;
    float strength;
    float radius;
};

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

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

    gl_FragColor = texture2D(texture0, vec2((pixelUVPos / screenSize).x, 1 - (pixelUVPos / screenSize).y));
}