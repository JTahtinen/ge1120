#version 330 core

in vec2 v_TexCoord;
out vec4 color;

uniform vec4 u_Color;
uniform sampler2D u_Texture;

void main()
{	
	vec4 texColor = texture(u_Texture, v_TexCoord);
	if (texColor.a == 0)
	{
		discard;
	}
 	color = vec4(u_Color.rgb, 0) + texColor;
	//color = texColor;
}
