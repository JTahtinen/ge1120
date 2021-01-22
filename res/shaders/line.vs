#version 330 core

layout (location = 0) in vec2 position;

out vec4 v_Color;

uniform vec4 u_Color;


void main()
{
    v_Color = u_Color;
	gl_Position = vec4(position.x, position.y, 0, 1);
}