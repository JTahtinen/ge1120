#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat3 u_View;

void main()
{
	vec3 pos3 = u_View * vec3(position.xy, 1);
	v_TexCoord = texCoord;
	gl_Position = vec4(pos3.xy, 0, 1);
}