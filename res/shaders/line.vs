#version 330 core

layout (location = 0) in vec2 position;

out vec4 v_Color;

uniform vec4 u_Color;

uniform float u_Aspect;

uniform mat3 u_Model;
uniform mat3 u_View;

void main()
{
    v_Color = u_Color;
	vec3 pos3 = u_View * u_Model * vec3(position.xy, 1);
	gl_Position = vec4(pos3.x, pos3.y * u_Aspect, 0, 1);
}