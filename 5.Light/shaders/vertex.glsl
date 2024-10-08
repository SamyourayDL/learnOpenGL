#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPosition;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPosition = (view*model * vec4(aPos, 1.0)).xyz;

    Normal = mat3(transpose(inverse(view*model)))* aNormal; // better to count on cpu, cause cost of inverse
}
