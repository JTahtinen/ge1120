#version 330 core

in vec2 v_TexCoord;
out vec4 color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{	
	vec4 texColor = texture(u_Texture, v_TexCoord);
 	color = u_Color + texColor;
}