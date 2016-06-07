#version 440 core

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec4 color;

uniform sampler2D albedo;
//uniform sampler2D normal;
//uniform sampler2D gloss;
//uniform sampler2D specular;

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
 
void main(){
    color = texture2D(tex0, texcoord);

	if (color.a < 0.5) {
		discard;
	}


	vec3 normal2 = normalize(normal);

	vec3 lightDir	= normalize(lightPosition - position);

	vec3 eyeDir			= normalize(cameraPosition - position);
	vec3 vHalfVector	= normalize(lightDir + eyeDir);

	// As far as i can tell this is first part diffuse and second (after the +) specular
	//color = max(dot(normal, lightDir), 0.0) * color + pow(max(dot(normal, vHalfVector), 0.0), 100) * 1.5; /**/

	// This is a modified version of above, just messing with it
	color = max(dot(normal2, lightDir), 0.0) * color + pow(max(dot(normal2, vHalfVector), 0.0), 100) * 1.5;
}