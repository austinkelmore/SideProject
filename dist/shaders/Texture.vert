
#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

uniform vec4 offset;
uniform mat4 perspectiveMatrix;

void main(void)
{
	gl_Position = perspectiveMatrix * (position + offset);
	theColor = color;
}