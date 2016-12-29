#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 color;

uniform mat4 fullMatrix;

out vec3 theColor;

void main()
{
	vec4 v = vec4(position, 1.0);
	gl_Position = fullMatrix * v;

	theColor = color;
}