#version 440 core

in vec3 vertPosition;
in vec3 vertNormal;
//in vec3 vertTangent;
//in vec2 vertTexCoord;

uniform mat4 mvp;
uniform mat4 modelMatrix;

out vec3 fragPosition;
//out vec2 fragTexCoord;
//out mat3 tbnMatrix;

out vec3 smoothNormal;

// TODO: Consider calculating bitangents ahdead of time

void main() {
	gl_Position		= mvp * vec4(vertPosition, 1.0);

	fragPosition	= vec3(modelMatrix * vec4(vertPosition, 1.0));
	//fragTexCoord	= vertTexCoord;

	//vec3 normal		= normalize(vec3(modelMatrix * vec4(vertNormal, 0.0)));
	//vec3 tangent	= normalize(vec3(modelMatrix * vec4(vertTangent, 0.0)));
	
	//tangent			= normalize(tangent - dot(tangent, normal) * normal);

	//vec3 bitangent	= cross(tangent, normal);

	//tbnMatrix = mat3(tangent, bitangent, normal);

	// TEMP
	//smoothNormal = vertNormal;
	smoothNormal = normalize(vec3(modelMatrix * vec4(vertNormal, 0.0)));
}