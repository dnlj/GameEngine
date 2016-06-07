#version 440 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D tex;

uniform vec3 lightPos;
uniform vec3 viewPos;

const vec3 specColor = vec3(1.0, 1.0, 1.0);

vec4 toLinear(vec4 color, float gamma = 2.2) {
	color.r = pow(color.r, gamma);
	color.g = pow(color.g, gamma);
	color.b = pow(color.b, gamma);
	return color;
}
 
void main() {
	color = texture(tex, fragTexCoord);

	if (color.a < 0.5) {
		discard;
	}

	vec3 normal = normalize(fragNormal);

	// Diffuse (Lambertian)
	vec3 lightDir = normalize(lightPos - fragPosition);
	float diffuse = max(dot(lightDir, normal), 0.0);

	// Specular (Blinn-Phong)
	vec3 viewDir		= normalize(viewPos - fragPosition);
	vec3 halfDir		= normalize(viewDir + lightDir);
	float dotHN			= max(dot(halfDir, normal), 0.0);
	float specular		= pow(dotHN, 100.0);



	color.a		= 1.0;
	color.rgb	= diffuse * color.rgb + specular * specColor;
}