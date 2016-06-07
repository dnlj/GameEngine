#version 440 core

in vec2 texcoord;

out vec4 color;

uniform sampler2D depthSampler;
uniform sampler2D colorSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;



/*
// DO NOT DELETE, Used for rendering the depth buffer if we ever want to.
float linearizeDepth(vec2 uv) {
	float z = texture(depthSampler, uv).r; // fetch the z-value from our depth texture
	float nearz = 0.1f; // the near plane
	float farz = 100.0f; // the far plane
	return (2.0f * nearz) / (farz + nearz - z * (farz - nearz)); // convert to linear values.  is 2.0 the gamma?
}*/

void main() {
	//color.rgb = vec3(linearizeDepth(texcoord)); // DO NOT DELETE, Used for rendering the depth buffer if we ever want to.
	color = texture(colorSampler, texcoord);

	// Lighting
	vec3 position	= texture(positionSampler, texcoord).xyz;
	vec3 normal		= normalize(texture(normalSampler, texcoord).xyz);

	vec3 light		= lightPosition;
	vec3 lightDir	= normalize(light - position);

	vec3 eyeDir			= normalize(cameraPosition - position);
	vec3 vHalfVector	= normalize(lightDir + eyeDir);

	// As far as i can tell this is first part diffuse and second (after the +) specular
	//color = max(dot(normal, lightDir), 0.0) * color + pow(max(dot(normal, vHalfVector), 0.0), 100) * 1.5; /**/

	// This is a modified version of above, just messing with it
	color = max(dot(normal, lightDir), 0.0) * color;



	// Highlight test
	//vec4 highlight = pow(1 - max(dot(normal, lightDir), 0.0), 2) * vec4(1.0, 0.5, 0.0, 1.0);
	//color += highlight;
	
	// Just here for testing, use the unlit texture
	//color = texture(colorSampler, texcoord);
	//color = vec4(normalize(texture(normalSampler, texcoord)).rgb, 1.0);

}