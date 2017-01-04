#version 430

out vec3 outColor;
in vec2 Texcoord;

uniform sampler2D tex;

void main()
{
	outColor = texture(tex, Texcoord).rgb;
}