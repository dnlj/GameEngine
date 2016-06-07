#version 440 core

in vec3 fragPosition;
in vec3 smoothNormal;

out vec4 color;

uniform sampler2D albedoMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float roughness;
uniform float intensity;
uniform float metalness;

const float PI = 3.14159265358979323846264338327950288419716;
const float EPSILON = 0.0000001;


///////////////////////////////////////////////////////////////////////
// Utility
///////////////////////////////////////////////////////////////////////
vec3 toLinear(vec3 color, float gamma = 2.2) {
	color.r = pow(color.r, gamma);
	color.g = pow(color.g, gamma);
	color.b = pow(color.b, gamma);
	return color;
}


///////////////////////////////////////////////////////////////////////
// Specular
///////////////////////////////////////////////////////////////////////
vec3 fresnel(vec3 f0, float dotHV) {
	float power = ((-5.55473 * dotHV) - 6.98316) * dotHV;
	return f0 + (1.0 - f0) * pow(2.0, power);
}

float distribution(float r, float dotNH) {
	float alpha = r * r;
	alpha = r;
	float alphaSquared = alpha * alpha;
	float dotNHSquared = dotNH * dotNH;

	float top = alphaSquared;
	float bottom = PI * pow(dotNHSquared * (alphaSquared - 1.0) + 1.0, 2);

	return top / bottom;
}

float geometric(float r, float dotNV, float dotNL) {
	float k = pow(r + 1.0, 2) / 8.0;

	float a = dotNL / ((dotNL * (1.0 - k)) + k);
	float b = dotNV / ((dotNV * (1.0 - k)) + k);

	return a * b;
}

vec3 brdf(float r, vec3 n, vec3 v, vec3 l, vec3 f0) {
	vec3 h = normalize(v + l);
	n = normalize(n);
	v = normalize(v);
	l = normalize(l);

	float dotNH = max(EPSILON, dot(n, h));
	float dotHV = max(EPSILON, dot(h, v));
	float dotNL = max(EPSILON, dot(n, l));
	float dotNV = max(EPSILON, dot(n, v));

	float d	= distribution(r, dotNH);
	vec3 f	= fresnel(f0, dotHV);
	float g	= geometric(r, dotNV, dotNL);
	
	vec3 spec = (d * f * g) / (4.0 * dotNL * dotNV);
	return spec;
}


///////////////////////////////////////////////////////////////////////
// Diffuse
///////////////////////////////////////////////////////////////////////
vec3 lambert(vec3 diff) {
	return diff / PI;
}


///////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////
void main() {
	// Variables
	vec3 n			= normalize(smoothNormal);
	vec3 l			= normalize(lightPos - fragPosition);
	vec3 v			= normalize(viewPos - fragPosition);
	float r			= max(EPSILON, roughness); 
	float dotNL		= max(0.0, dot(n, l));
	vec3 lightColor	= vec3(1.0, 1.0, 1.0);

	// Albedo
	vec3 albedo		= toLinear(vec3(0.5, 0.15, 0.1));
	vec3 f0			= mix(vec3(0.04), albedo.rgb, metalness);
	vec3 realAlbedo = albedo.rgb - albedo.rgb * metalness;

	// Other
	vec3 specular	= brdf(r, n, v, l, f0);
	vec3 diffuse	= lambert(realAlbedo);

	color.rgb	= (diffuse + specular) * dotNL * lightColor * intensity;
	color.a		= 1.0;
}