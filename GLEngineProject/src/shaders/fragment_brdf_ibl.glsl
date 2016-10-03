#version 440 core

in vec3 fragPosition;
in vec3 smoothNormal;
in vec2 fragTexCoord;
in mat3 tbnMatrix;

out vec4 color;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;
uniform samplerCube cubeMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

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
// Fresnel - Schlick's approximation
vec3 fresnel_schlick(vec3 f0, float dotHV) {
	return f0 + (1.0 - f0) * pow(1.0 - dotHV, 5);
}

// Fresnel - Schlick's approximation, approximated ["Real Shading in Unreal Engine 4", SIGGRAPH 2013]
vec3 fresnel_schlick_approx(vec3 f0, float dotHV) {
	float power = ((-5.55473 * dotHV) - 6.98316) * dotHV;
	return f0 + (1.0 - f0) * exp2(power);
	
}

// Distribution - GGX(Trowbridge - Reitz) ["Real Shading in Unreal Engine 4", SIGGRAPH 2013]
float distribution_ggx_tr(float r, float dotNH) {
	float alpha = r * r; // "Hotness" remap, used by Disney and UE4. Not to be used with IBL? Look into this further
	float alphaSquared = alpha * alpha;
	float dotNHSquared = dotNH * dotNH;

	float top = alphaSquared;
	float bottom = PI * pow(dotNHSquared * (alphaSquared - 1.0) + 1.0, 2);

	return top / bottom;
}

// Geometric - Smith Schlick GGX ["Real Shading in Unreal Engine 4", SIGGRAPH 2013]
float geometric_smith_schlick_ggx(float r, float dotNV, float dotNL) {
	float k = pow(r + 1.0, 2) / 8.0; // k = ((r+1)/2)^2

	float a = dotNL / ((dotNL * (1.0 - k)) + k);
	float b = dotNV / ((dotNV * (1.0 - k)) + k);

	return a * b;
}

// Calculate specular term
vec3 specular(vec3 f0, float r, float dotNH, float dotHV, float dotNL, float dotNV) {
	float d	= distribution_ggx_tr(r, dotNH);
	vec3 f	= fresnel_schlick_approx(f0, dotHV);
	float g	= geometric_smith_schlick_ggx(r, dotNV, dotNL);
	
	vec3 spec = (d * f * g) / (4.0 * dotNL * dotNV);
	return spec;
}


///////////////////////////////////////////////////////////////////////
// Diffuse
///////////////////////////////////////////////////////////////////////
// Lambert diffuse
vec3 lambert(vec3 diff) {
	return diff / PI;
}

// Calculate diffuse term
vec3 diffuse(vec3 albedo) {
	return lambert(albedo);
}


///////////////////////////////////////////////////////////////////////
// IBL
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// Lighting
///////////////////////////////////////////////////////////////////////
// Calculate lighting
vec3 calcLight(vec3 albedo, vec3 f0, float r, vec3 n, vec3 v, vec3 l) {
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 h = normalize(v + l);

	// We use epsilon here to avoid division by zero
	float dotNH = max(EPSILON, dot(n, h));
	float dotHV = max(EPSILON, dot(h, v));
	float dotNL = max(EPSILON, dot(n, l));
	float dotNV = max(EPSILON, dot(n, v));

	vec3 diff = diffuse(albedo);
	vec3 spec = specular(f0, r, dotNH, dotHV, dotNL, dotNV);

	return (diff + spec) * dotNL * lightColor;
}


///////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////
void main() {
	// Textures
	vec3 albedo = texture(albedoMap, fragTexCoord).rgb;
	vec3 normal = texture(normalMap, fragTexCoord).xyz;
	float roughness = texture(roughnessMap, fragTexCoord).r;

	// Change normal from [0, 1] to [-1, 1]. We use 255.0/128.0 instead of 2 to correct for rounding, this allows us to get perfectly vertical normals.
	normal = normal * (255.0 / 128.0) - 1.0;

	// Transform normal into world space
	normal = normalize(tbnMatrix * normal);

	// Get enviroment color
	vec3 incident	= normalize(fragPosition - viewPos);
	vec3 reflected	= reflect(incident, normal);
	vec3 envColor	= texture(cubeMap, reflected).rgb;

	// Variables
	float lightDist = length(lightPos - fragPosition); // Distance from the fragment to the light
	vec3 lightDir	= normalize(lightPos - fragPosition); // Light direction
	vec3 viewDir	= normalize(viewPos - fragPosition); // View direction

	// Specular F0
	vec3 f0			= mix(vec3(0.04), albedo.rgb, metalness);

	// Albedo
	vec3 realAlbedo = mix(albedo.rgb, vec3(0.00), metalness);

	// Attenuation
	float attenuation = 1.0 / (lightDist * lightDist);
	attenuation = 1.0; // TODO: Remove, this is just for testing so i can see everything

	// Calculate final lighting
	vec3 light = calcLight(realAlbedo, f0, roughness, normal, viewDir, lightDir);

	// TODO: is this the correct way to do "intensity"? if so, what are the unit of intensity?
	color.rgb	= light * intensity * attenuation;
	color.a		= 1.0;


	//color.rgb *= 0.000000000000000000001;
	//color.rgb += envColor;
	color.rgb += envColor*0.00000000000000000000000001;
}