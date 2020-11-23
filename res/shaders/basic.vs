#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform float u_Aspect;
uniform vec2 u_Offset;

void main()
{
	v_TexCoord = texCoord;
	gl_Position = vec4(position.x, position.y * u_Aspect, 0, 1) + vec4(u_Offset.x, u_Offset.y * u_Aspect, 0, 0);
}