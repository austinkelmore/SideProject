
#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

smooth out vec4 theColor;

void main(void)
{
	gl_Position = position;
	theColor = color;
}