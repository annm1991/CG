#version 400 core

layout(location = 0) in vec4 vPosition;
uniform vec3 vColor;

out vec4 Color;

void main()
{
	Color = vec4(vColor, 0.0);
    gl_Position = vPosition;
}
