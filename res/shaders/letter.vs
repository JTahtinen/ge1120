#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 view0;
layout (location = 3) in vec3 view1;
layout (location = 4) in vec3 view2;

out vec2 v_TexCoord;

//uniform mat3 u_View;

void main()
{
    	mat3 view = transpose(mat3(view0, view1, view2));
	vec3 pos3 = view * vec3(position.xy, 1);
	v_TexCoord = texCoord;
	gl_Position = vec4(pos3.xy, 0, 1);
}