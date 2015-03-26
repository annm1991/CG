#version 400 core

layout(location = 1) in vec3 vPosition;
layout(location = 2) in vec3 vColor;
layout(location = 3) in vec3 vNormal;
layout(location = 4) in vec3 vAmbient;

uniform mat4 view, model, projection;
uniform vec3 light;

vec3 vn;
out vec4 Color;

void main()
{
	gl_Position = projection * view * model * vec4(vPosition, 1.0);
	vn = normalize(mat3(model) * vNormal); 
	Color = vec4(vAmbient, 1.0f) + max(0, dot(vec4(light, 1.0f), vec4(vn, 1.0f))) * vec4(vColor, 1.0f);
}
