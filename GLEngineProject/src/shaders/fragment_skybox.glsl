#version 440 core

in vec3 fragTexCoord;

out vec4 color;

uniform samplerCube cubeMap;

void main() {
	color = texture(cubeMap, fragTexCoord);
}