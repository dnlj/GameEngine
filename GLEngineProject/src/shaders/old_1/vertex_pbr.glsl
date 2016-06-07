#version 440 core

in vec3 v_position;
in vec3 v_normal;
in vec2 v_texcoord;

out vec3 position;
out vec3 normal;
out vec2 texcoord;

uniform mat4 mvp;


void main() {
	gl_Position = mvp * vec4(v_position, 1.0);
	texcoord = v_texcoord;
	position = v_position;
	normal = v_normal;
}