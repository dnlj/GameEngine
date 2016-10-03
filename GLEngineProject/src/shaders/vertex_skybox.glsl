#version 440 core

in vec3 vertPosition;

uniform mat4 mvp;

out vec3 fragTexCoord;


void main() {
	gl_Position = mvp * vec4(vertPosition, 0.0);

	fragTexCoord = vertPosition;
}