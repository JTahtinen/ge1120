#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform float u_Aspect;
uniform vec2 u_Offset;

uniform mat3 u_Rotation;

void main()
{
	v_TexCoord = texCoord;
	vec3 pos3 = u_Rotation * vec3(position.xy, 0);
	gl_Position = vec4(pos3.x, pos3.y * u_Aspect, 0, 1) + vec4(u_Offset.x, u_Offset.y * u_Aspect, 0, 0);
}