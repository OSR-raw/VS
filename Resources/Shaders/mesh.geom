#version 410 core

uniform mat4 pvm;

layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in vec3 position_vert[];

out vec3 normal;
out vec3 position;

void main()
{
	vec3 a = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 b = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	//vec3 ñ = gl_in[1].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	/*if((( gl_in[0].gl_Position.z == 24.0f) || (gl_in[1].gl_Position.z  == 24.0)) || gl_in[2].gl_Position.z == 24.0 )
		return;
	*/
	vec3 nrm = normalize(cross( a, b ));
	//bool tmp = true;
	
	for (int i=0;i<3; i++) 
	{
		gl_Position = pvm*(gl_in[i].gl_Position);
		normal = nrm;
		position = position_vert[i];
		EmitVertex();
	}
	EndPrimitive();
	/*for (int i=0;i<3; i++) 
	{
		gl_Position = pvm*vec4(gl_in[i].gl_Position.xy, 128.0, 1.0);
		normal = nrm;
		position = position_vert[i];
		EmitVertex();
	}
	EndPrimitive();*/
}  