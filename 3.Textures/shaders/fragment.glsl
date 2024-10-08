#version 330 core

in vec2 TexCoord;
in vec3 ourColor;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float opacity;

void main() {
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, (TexCoord-0.4)*5.0), opacity);
}