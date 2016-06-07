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

#define float4 vec4
#define float3 vec3
#define float2 vec2
#define saturate(value) clamp(value, 0.0f, 1.0f)
#define lerp mix


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

// ===============================================================================
// Calculates the Fresnel factor using Schlick’s approximation
// ===============================================================================
float3 Fresnel(in float3 specAlbedo, in float3 h, in float3 l) {
	float lDotH = saturate(dot(l, h));
	return specAlbedo + (1.0f - specAlbedo) * pow((1.0f - lDotH), 5.0f);
}

// ===============================================================================
// Helper for computing the GGX visibility term
// ===============================================================================
float GGX_V1(in float m2, in float nDotX) {
	return 1.0f / (nDotX + sqrt(m2 + (1 - m2) * nDotX * nDotX));
}

// ===============================================================================
// Computes the specular term using a GGX microfacet distribution, with a matching
// geometry factor and visibility term. m is roughness, n is the surface normal,
// h is the half vector, l is the direction to the light source, and specAlbedo is
// the RGB specular albedo
// ===============================================================================
float3 GGX_Specular(in float m, in float3 n, in float3 h, in float3 v, in float3 l, in float3 specAlbedo) {
	float nDotL = saturate(dot(n, l));
	if(nDotL <= 0.0f) { return vec3(0.0f); }

	float nDotH = saturate(dot(n, h));
	float nDotV = max(dot(n, v), 0.0001f);
	float nDotH2 = nDotH * nDotH;
	float m2 = m * m;

	// Calculate the distribution term
	float d = m2 / (PI * pow(nDotH * nDotH * (m2 - 1) + 1, 2.0f));

	// Calculate the matching visibility term
	float v1i = GGX_V1(m2, nDotL);
	float v1o = GGX_V1(m2, nDotV);
	float vis = v1i * v1o;

	// Calculate the fresnel term
	float3 f = Fresnel(specAlbedo, h, l);

	// Put it all together
	return d * f * vis;
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
	// Stuff
	vec3 n		= normalize(smoothNormal);
	vec3 l		= normalize(lightPos - fragPosition);
	vec3 v		= normalize(viewPos - fragPosition);
	vec3 h		= normalize(v + l);
	float r		= max(EPSILON, roughness); 
	float dotNL	= max(0.0, dot(n, l));

	// Albedo
	vec3 albedo		= toLinear(vec3(0.5, 0.15, 0.1));
	vec3 f0			= mix(vec3(0.04), albedo.rgb, metalness);
	vec3 realAlbedo = albedo.rgb - albedo.rgb * metalness;

	// Other
	vec3 lightColor	= vec3(0.0, 0.0, 1.0);

	vec3 specular	= GGX_Specular(r, n, h, v, l, f0);
	vec3 diffuse	= lambert(realAlbedo) * dotNL;

	color.rgb	= (diffuse + specular) * intensity;
	color.a		= 1.0 + r;
}