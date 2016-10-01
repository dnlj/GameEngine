#version 440 core

in vec3 fragPosition;
in vec3 smoothNormal;
in vec2 fragTexCoord;
in mat3 tbnMatrix;

out vec4 color;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D roughnessMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

//uniform float roughness;
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
// TODO: Document what type of fresnel this is (Optimized schlick from ue4 i think)
vec3 fresnel(vec3 f0, float dotHV) {
	float power = ((-5.55473 * dotHV) - 6.98316) * dotHV;
	return f0 + (1.0 - f0) * exp2(power);
	
}

// TODO: Document what type of distribution this is (GGX? idk)
float distribution(float r, float dotNH) {
	float alpha = r * r; // "Hotness" remap, used by Disney and UE4. Not to be used with IBL? Look into this further
	float alphaSquared = alpha * alpha;
	float dotNHSquared = dotNH * dotNH;

	float top = alphaSquared;
	float bottom = PI * pow(dotNHSquared * (alphaSquared - 1.0) + 1.0, 2);

	return top / bottom;
}

// TODO: Document what type of geometric this is (Smith i think?)
float geometric(float r, float dotNV, float dotNL) {
	float k = pow(r + 1.0, 2) / 8.0;

	float a = dotNL / ((dotNL * (1.0 - k)) + k);
	float b = dotNV / ((dotNV * (1.0 - k)) + k);

	return a * b;
}

vec3 brdf(float r, vec3 n, vec3 v, vec3 l, vec3 f0) {
	vec3 h = normalize(v + l);

	// We use epsilon here to avoid division by zero
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
// IBL
///////////////////////////////////////////////////////////////////////



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

	// Variables
	vec3 n			= normalize(normal);
	vec3 l			= normalize(lightPos - fragPosition);
	vec3 v			= normalize(viewPos - fragPosition);
	float r			= max(0.025, roughness); 
	float dotNL		= max(0.0, dot(n, l));
	vec3 lightColor	= vec3(1.0, 1.0, 1.0);

	// Albedo
	vec3 f0			= mix(vec3(0.04), albedo.rgb, metalness);
	vec3 realAlbedo = albedo.rgb - albedo.rgb * metalness; // TODO: Replace this with a lerp?

	// Other
	vec3 specular	= brdf(r, n, v, l, f0);
	vec3 diffuse	= lambert(realAlbedo);

	color.rgb	= (diffuse + specular) * dotNL * lightColor * intensity;
	color.a		= 1.0;
}