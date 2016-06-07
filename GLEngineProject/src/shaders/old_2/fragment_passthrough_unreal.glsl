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
float Pow5(float a) {
	return a * a * a * a * a;
}

float rcp(float a) {
	return 1.0 / a;
}

float Square(float a) {
	return a * a;
}

// [Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"]
float3 F_Schlick( float3 SpecularColor, float VoH )
{
	float Fc = Pow5( 1 - VoH );					// 1 sub, 3 mul
	//return Fc + (1 - Fc) * SpecularColor;		// 1 add, 3 mad
	
	// Anything less than 2% is physically impossible and is instead considered to be shadowing
	return saturate( 50.0 * SpecularColor.g ) * Fc + (1 - Fc) * SpecularColor;
	
}

// Appoximation of joint Smith term for GGX
// [Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"]
float Vis_SmithJointApprox( float Roughness, float NoV, float NoL )
{
	float a = Square( Roughness );
	float Vis_SmithV = NoL * ( NoV * ( 1 - a ) + a );
	float Vis_SmithL = NoV * ( NoL * ( 1 - a ) + a );
	return 0.5 * rcp( Vis_SmithV + Vis_SmithL );
}

// GGX / Trowbridge-Reitz
// [Walter et al. 2007, "Microfacet models for refraction through rough surfaces"]
float D_GGX( float Roughness, float NoH )
{
	float a = Roughness * Roughness;
	float a2 = a * a;
	float d = ( NoH * a2 - NoH ) * NoH + 1;	// 2 mad
	return a2 / ( PI*d*d );					// 4 mul, 1 rcp
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

	float d	= D_GGX(r, dotNH);
	vec3 f	= F_Schlick(f0, dotHV);
	float g	= Vis_SmithJointApprox(r, dotNV, dotNL);
	
	vec3 spec = (d * f * g);

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