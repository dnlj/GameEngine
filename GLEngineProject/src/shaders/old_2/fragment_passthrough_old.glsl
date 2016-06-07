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
//	Fresnel Functions
///////////////////////////////////////////////////////////////////////
vec3 schlickApprox(vec3 f0, float dotHV) {
	//float dotHV = max(0.0, dot(h, v));
	return f0 + (1.0 - f0) * pow(1.0 - dotHV, 5.0);
}


///////////////////////////////////////////////////////////////////////
//	Normal Distribution Functions
///////////////////////////////////////////////////////////////////////
float beckmann(float a, float dotNH) {
	float a2		= a*a;
	float dotNM2	= dotNH * dotNH;
	float dotNM4	= dotNM2 * dotNM2;

	float top		= exp((dotNM2 - 1.0) / (a2 * dotNM2));
	float bottom	= PI * a2 * dotNM4;

	return top / bottom;
}

// GGX (Trowbridge-Reitz)
float ggx(float roughness, float dotNH) {
	float a			= roughness * roughness; // square
	float a2		= a * a; // square
	float dotNH2	= dotNH * dotNH; // square
	float alphaTerm	= a2 - 1.0;

	float top		= a2;
	float bottom	= PI * pow((dotNH2 * alphaTerm) + 1.0, 2.0);
	return top / bottom;
}


///////////////////////////////////////////////////////////////////////
//	Geometric Shadowing Functions
///////////////////////////////////////////////////////////////////////
//float geometricCT(vec3 n, vec3 h, vec3 v, vec3 l) {
float geometricCT(float dotNH, float dotNV, float dotNL, float dotHV) {
	//float dotNH = dot(n, h);
	//float dotNV = dot(n, v);
	//float dotNL = dot(n, l);
	//float dotVH = dot(v, h);

	float a = (2.0 * dotNH * dotNV) / dotHV;
	float b = (2.0 * dotNH * dotNL) / dotHV;

	return min(1.0, min(a, b));
}

///////////////////////////////////////////////////////////////////////
//	Unreal copy-paste for reference
///////////////////////////////////////////////////////////////////////


float Square(float x) {
	return x * x;
}

vec3 Square(vec3 x) {
	return x * x;
}

float rcp(float x) {
	return 1.0 / x;
}

// (GGX / Trowbridge-Reitz) NOTE: Looks same as your implementation (havent actually compared, just looked at, could look in photoshop)
float D_GGX( float Roughness, float NoH ) {
	float a = Roughness * Roughness;
	float a2 = a * a;
	float d = ( NoH * a2 - NoH ) * NoH + 1.0;	// 2 mad
	return a2 / ( PI*d*d );						// 4 mul, 1 rcp
}

vec3 F_Fresnel( vec3 SpecularColor, float VoH) {
	vec3 SpecularColorSqrt = sqrt( clamp( vec3(0.0, 0.0, 0.0), vec3(0.99, 0.99, 0.99), SpecularColor ) );
	vec3 n = (1.0 + SpecularColorSqrt) / (1.0 - SpecularColorSqrt);
	vec3 g = sqrt(n*n + VoH*VoH - 1.0);
	return 0.5 * Square((g - VoH) / (g + VoH)) * (1.0 + Square(((g + VoH)*VoH - 1.0) / ((g - VoH)*VoH + 1.0)));
}

// Smith term for GGX
float Vis_Smith( float Roughness, float NoV, float NoL ){
	float a = Square( Roughness );
	float a2 = a*a;

	float Vis_SmithV = NoV + sqrt( NoV * (NoV - (NoV * a2)) + a2);
	float Vis_SmithL = NoL + sqrt( NoL * (NoL - (NoL * a2)) + a2);
	return rcp(Vis_SmithV * Vis_SmithL);
}


float Pow5 (float x) {
	return pow(5.0, x);
}


///////////////////////////////////////////////////////////////////////
//	fsdafsdafdsa
///////////////////////////////////////////////////////////////////////
float distribution(float roughness, float dotNH) { // range: [0, inf]
	// TODO: Implement GGX
	//return beckmann(roughness, dotNH);
	return ggx(roughness, dotNH);
}

//float geometric(vec3 n, vec3 h, vec3 v, vec3 l) { // range: [0, 1]
float geometric(float dotNH, float dotNV, float dotNL, float dotHV) { // range: [0, 1]
	// TODO: Lookinto smith
	//return geometricCT(n, h, v, l);
	return geometricCT(dotNH, dotNV, dotNL, dotHV);
}

vec3 fresnel(vec3 f0, float dotHV) { // range: [0, 1]
	return schlickApprox(f0, dotHV);
}

vec3 cookTorrance(vec3 n, vec3 v, vec3 l, vec3 f0) {
	vec3 h = normalize(v + l);
	n = normalize(n);
	v = normalize(v);
	l = normalize(l);

	float dotNH = max(0.0, dot(n, h));
	float dotHV = max(0.0, dot(h, v));
	float dotNL = max(0.0, dot(n, l));
	float dotNV = max(0.0, dot(n, v));
	float dotVH = max(0.0, dot(v, h));

	// TODO: Implement ibl, i wonder how much this affects the look at angles
	// TODO: Implement new geometric functions, ct is not so great
	float d = distribution(roughness, dotNH);
	vec3 f = fresnel(f0, dotHV);
	float g = geometric(dotNH, dotNV, dotNL, dotVH);


	// The (4.0 * dotNL * dotNV) division is what makes it look strange at backfacing angles
	//vec3 spec = (d * f * g) / (4.0 * dotNL * dotNV);
	//vec3 spec = d * f * g;
	//vec3 spec = D_GGX(roughness, dotNH) * Vis_Smith(roughness, dotNV, dotNL) * F_Fresnel(f0, dotVH); // WHY THIS NO WORK ITS UNREALS MODEL PLZ
	vec3 spec = F_Fresnel(f0, dotVH) * D_GGX(roughness, dotNH) * Vis_Smith(roughness, dotNV, dotNL); // WHY THIS NO WORK ITS UNREALS MODEL PLZ
	return spec;
}

///////////////////////////////////////////////////////////////////////
//	Main
///////////////////////////////////////////////////////////////////////
void main() {
	vec4 albedo = texture(albedoMap, fragTexCoord);
	
	if (albedo.a < 0.5) { discard; }

	vec3 normal = texture(normalMap, fragTexCoord).xyz;
	normal = normal * (255.0/128.0) - 1.0; // Change normal from [0, 1] to [-1, 1]. We use 255.0/128.0 instead of 2 to correct for rounding, this allows us to get perfectly vertical normals.
	normal = normalize(tbnMatrix * normal); // Transform normal into world space

	normal = zeroVec3(normal);
	normal += smoothNormal;

	normal = normalize(normal);

	vec3 lightDir = normalize(lightPos - fragPosition);
	vec3 viewDir = normalize(viewPos - fragPosition);

	float diffuse = max(0.0, dot(lightDir, normal)); // TODO: Move diffuse into function lambertian function
	
	albedo = zeroVec4(albedo);
	albedo.rgb += toLinear(vec3(0.5, 0.15, 0.1));

	vec3 f0 =  mix(vec3(0.04), albedo.rgb, metalness);

	//TODO: also do the max(0.0000000001,) thing to prevent it from going black (idk how small of a number to use, check google)

	//vec3 finalAlbedo = albedo.rgb * (1.0 - metalness); // TODO rename this variable, think this is a poor name
	vec3 finalAlbedo = albedo.rgb  - albedo.rgb * metalness;
	//vec3 finalAlbedo = albedo.rgb  * (1.0 - schlickApprox(f0, dot(normal, lightDir))); // Look into this implementation more, (http://www.gamedev.net/topic/664808-example-of-specular-aliasing/#entry5204431) (http://www.gamedev.net/topic/670517-verify-my-ggx-lambert-directional-light/#entry5243703)
	// Is this the same as? i think it is: GBuffer.DiffuseColor = GBuffer.BaseColor - GBuffer.BaseColor * GBuffer.Metallic

	//float ct = cookTorrance(v, normal, l);
	vec3 ct = cookTorrance(normal, viewDir, lightDir, f0);
	color.rgb	= finalAlbedo * diffuse + ct;
	color.a		= 1.0;

	zeroVec4(color);
	color.rgb = ct;


	//color = zeroVec4(color);
	// TODO: Name these variabels, also c
	//vec3 incident = -v;
	//vec3 reflection = reflect(incident, normal);
	// TODO: Figure out how to blend this.
	//color += texture(cubeMap, reflection);
}