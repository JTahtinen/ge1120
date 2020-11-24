#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform float u_Aspect;

uniform mat3 u_Model;
uniform mat3 u_View;

void main()
{
	v_TexCoord = texCoord;
	vec3 pos3 = u_View * u_Model * vec3(position.xy, 1);
	gl_Position = vec4(pos3.x, pos3.y * u_Aspect, 0, 1);
}