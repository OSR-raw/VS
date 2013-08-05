#version 410 core

in vec3 in_Position;
in vec4 in_Color;
in vec2 in_UV;
in vec3 in_Normal;

out vec4 pass_Color_vert;

out vec3 normal_vert;
out vec3 position_vert;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0);
	position_vert = in_Position;
	normal_vert = in_Normal;
    pass_Color_vert = (in_Color);///vec4(255.0,255.0,255.0,255.0);
	//UV_vert = in_UV;
}