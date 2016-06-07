#version 440 core

in vec2 outUV;

// These are here for reference, they are fro ma tuttorial not to be actually used
//layout (location = 0) out vec3 WorldPosOut; 
//layout (location = 1) out vec3 DiffuseOut; 
//layout (location = 2) out vec3 NormalOut; 
//layout (location = 3) out vec3 TexCoordOut; 

layout (location = 0) out vec4 fragColor;

uniform sampler2D tex0;

void main() {
	//fragColor = vec4(outColor, 1.0);
	fragColor = texture(tex0, outUV);
}