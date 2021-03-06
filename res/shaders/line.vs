#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 model0;
layout (location = 2) in vec3 model1;
layout (location = 3) in vec3 model2;
layout (location = 4) in vec3 view0;
layout (location = 5) in vec3 view1;
layout (location = 6) in vec3 view2;

out vec4 v_Color;

uniform vec4 u_Color;

void main()
{
	mat3 model = transpose(mat3(model0, model1, model2));
	mat3 view = transpose(mat3(view0, view1, view2));
	v_Color = u_Color;
	vec3 pos3 = view * model * vec3(position.xy, 1);
	gl_Position = vec4(pos3.x, pos3.y, 0, 1);
}