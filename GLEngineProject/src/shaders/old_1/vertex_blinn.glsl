#version 440 core

in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord;

uniform mat4 mvp;
uniform mat4 modelMatrix;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
	gl_Position = mvp * vec4(vertPosition, 1.0);

	fragPosition	= vec3(modelMatrix * vec4(vertPosition, 1.0));
	fragNormal		= vec3(modelMatrix * vec4(vertNormal, 0.0));
	
	fragTexCoord	= vertTexCoord;
}