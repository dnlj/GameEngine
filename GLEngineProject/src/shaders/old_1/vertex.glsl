#version 440 core

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 mvp;

out vec2 outUV;

void main() {
	gl_Position = mvp * vec4(position, 1.0);
	outUV = uv;
	//outColor = position;
}