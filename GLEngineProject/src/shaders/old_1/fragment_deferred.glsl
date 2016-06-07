#version 440 core

in vec3 position;
in vec3 normal;
in vec2 texcoord;


layout(location = 0) out vec4 theColor;
layout(location = 1) out vec3 thePosition;
layout(location = 2) out vec3 theNormal;

uniform sampler2D tex0;

void main() {
	thePosition = position;
	theNormal = normal;
	theColor = texture(tex0, texcoord);

	if (theColor.a < 0.5) {
		discard;
	}
}