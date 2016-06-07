#version 440 core

in vec2 v_position;
in vec2 v_texcoord;

out vec2 texcoord;

void main() {
	gl_Position = vec4(v_position, 0.0f, 1.0f);
	texcoord = v_texcoord;
}