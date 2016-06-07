#version 440 core

in vec3 fragPosition;
in vec2 fragTexCoord;
in mat3 tbnMatrix;

in vec3 smoothNormal;

out vec4 color;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform samplerCube cubeMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

const float PI = 3.14159265358979323846264338327950288419716;
const float EPSILON = 0.00001;

const vec3 lightColor = vec3(1.0, 1.0, 1.0);

// TEMP
uniform float roughness;
uniform float metalness;


vec4 zeroVec4(vec4 vec) {
	vec -= vec4(10000000.0, 10000000.0, 10000000.0, 10000000.0);
	vec -= vec4(10000000.0, 10000000.0, 10000000.0, 10000000.0);
	vec -= vec4(10000000.0, 10000000.0, 10000000.0, 10000000.0);
	vec = max(vec4(0.0), vec);
	vec.a = 1.0;
	return vec;
}

vec3 zeroVec3(vec3 vec) {
	vec -= vec3(10000000.0, 10000000.0, 10000000.0);
	vec -= vec3(10000000.0, 10000000.0, 10000000.0);
	vec -= vec3(10000000.0, 10000000.0, 10000000.0);
	vec = max(vec3(0.0), vec);
	return vec;
}

vec3 toLinear(vec3 color, float gamma = 2.2) {
	color.r = pow(color.r, gamma);
	color.g = pow(color.g, gamma);
	color.b = pow(color.b, gamma);
	return color;
}



///////////////////////////////////////////////////////////////////////
// BRDF
///////////////////////////////////////////////////////////////////////
vec3 fresnel(vec3 f0, float dotHV) {
	return f0 + (1.0 - f0) * pow(1.0 - dotHV, 5.0);
}

float distribution(float r, float dotNH) {
	float alpha = r * r;
	float alphaSquared = alpha * alpha;
	float dotNHSquared = dotNH * dotNH;

	float top = alphaSquared;
	float bottom = PI * pow(dotNHSquared * (alphaSquared - 1.0) + 1.0, 2.0); // Could this be zero? when dotNHSquared is one and alphasquared/roughness is zero this is a division by zero.

	return top / bottom;
}

float geometric(float r, float dotNV, float dotNL) {
	float k = pow(r + 1.0, 2.0) / 8.0;

	float a = dotNL / (dotNL * (1.0 - k) + k);
	float b = dotNV / (dotNV * (1.0 - k) + k);

	return a * b;
}

vec3 brdf(vec3 n, vec3 v, vec3 l, vec3 f0) {
	vec3 h = normalize(v + l);
	n = normalize(n);
	v = normalize(v);
	l = normalize(l);

	float dotNH = max(0.0, dot(n, h));
	float dotHV = max(0.0, dot(h, v));
	float dotNL = max(0.0, dot(n, l));
	//float dotNV = max(0.0, dot(n, v));
	float dotNV = abs(dot(n, v)) + 0.00001; // Why does frostbite do this?

	float d = distribution(roughness, dotNH);
	vec3 f = fresnel(f0, dotHV);
	float g = geometric(roughness, dotNV, dotNL);
	
	vec3 spec = (d * f * g) / (4.0 * dotNL * dotNV); // dotNV can be zero therefor we are dividing by zero, look at frostbite paper

	return spec;
}

///////////////////////////////////////////////////////////////////////
//	Main
///////////////////////////////////////////////////////////////////////
void main() {
	vec4 albedo = texture(albedoMap, fragTexCoord);

	if (albedo.a < 0.5) { discard; }

	
	// Normals
	vec3 normal = texture(normalMap, fragTexCoord).xyz;
	normal = normal * (255.0/128.0) - 1.0; // Change normal from [0, 1] to [-1, 1]. We use 255.0/128.0 instead of 2 to correct for rounding, this allows us to get perfectly vertical normals.
	normal = normalize(tbnMatrix * normal); // Transform normal into world space

	normal = zeroVec3(normal);
	normal += smoothNormal;

	normal = normalize(normal);


	// Diffuse
	vec3 lightDir = normalize(lightPos - fragPosition);
	vec3 viewDir = normalize(viewPos - fragPosition);

	float lambert = max(0.0, dot(lightDir, normal)); // TODO: Move diffuse into function lambertian function
	

	// Albedo
	albedo = zeroVec4(albedo);
	albedo.rgb += toLinear(vec3(0.5, 0.15, 0.1));

	vec3 f0 =  mix(vec3(0.04), albedo.rgb, metalness);

	vec3 finalAlbedo = albedo.rgb - albedo.rgb * metalness;


	// Final BRDF
	vec3 specular	= brdf(normal, viewDir, lightDir, f0);
	vec3 diffuse	= finalAlbedo * lightColor * lambert;

	if (specular.r < 0.0) {
		color.rgb = toLinear(vec3(1.0, 0.0, 0.0));
	} else if (specular.g < 0.0) {
		color.rgb = toLinear(vec3(0.0, 1.0, 0.0));
	} else if (specular.b < 0.0) {
		color.rgb = toLinear(vec3(0.0, 0.0, 1.0));
	} else {
		color.rgb = toLinear(vec3(1.0, 1.0, 1.0));
	}

	//color = zeroVec4(color);
	//color.rgb += diffuse;
	color.rgb	= diffuse + specular;
	//color.rgb	= specular;
	color.a		= 1.0;
}