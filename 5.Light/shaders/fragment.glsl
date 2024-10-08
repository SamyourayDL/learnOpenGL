#version 330 core

in vec3 Normal;
in vec3 FragPosition;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    float ambientFactor = 0.1;
    vec3 ambient = ambientFactor * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPosition);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(- FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm); // relfect expect -lightDir
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;

    FragColor = vec4((ambient+ diffuse + specular) * objectColor, 1.0);
}