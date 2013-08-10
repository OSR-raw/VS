#version 410 core

uniform mat4 pvm;

layout(points) in;
layout(triangle_strip, max_vertices=32) out;

in vec4 pass_Color_vert[];
//in vec2 UV_vert[];
in vec3 normal_vert[];
in vec3 position_vert[];

out vec4 pass_Color;
//out vec2 UV;
out vec3 normal;
out vec3 position;

const vec4 cubeVerts[8] = vec4[8](
    vec4(-0.5 , -0.5, -0.5,0),  //LB   0
     vec4(-0.5, 0.5, -0.5,0), //L T   1
    vec4(0.5, -0.5, -0.5,0), //R B    2
    vec4( 0.5, 0.5, -0.5,0),  //R T   3
                        //back face
    vec4(-0.5, -0.5, 0.5,0), // LB  4
     vec4(-0.5, 0.5, 0.5,0), // LT  5
    vec4(0.5, -0.5, 0.5,0),  // RB  6
     vec4(0.5, 0.5, 0.5,0)  // RT  7
    );

const int  cubeIndices[24]  = int [24]
    (
	  7,5,6,4,  //front
      0,1,2,3, //back
	  1,0,5,4, //left
      7,6,3,2, //right      
      4,0,6,2, //btm       
      3,1,7,5
    );

void EmitSide( vec4 pos, vec4 color, int start, vec4 transVerts[8], vec3 norm )
{
	for (int i=start;i<start+4; i++)
	{
        gl_Position = transVerts[cubeIndices[i]];
        pass_Color = color;
		//UV = vec2(0.0,0.0);
		normal = norm;
		position = position_vert[0];
        EmitVertex();
	}
}

vec3 GetNormal( uint code )
{
	vec3 normal_ = vec3(0.0,0.0,0.0);
	//is front
	//normal_[2] = ( code%2 == 0 ) ? 1.0 : 0;
	if ( code%2 == 0 )
	{
		normal_[2] += 1.0;
	}
	code = code>>1;
	//back
	if ( code%2 == 0 )
	{
		normal_[2] -= 1.0;
	}
	code = code>>1;
	//left
	if ( code%2 == 0 )
	{
		normal_[0] -= 1.0;
	}
	code = code>>1;
	//right
	if ( code%2 == 0 )
	{
		normal_[0] += 1.0;
	}
	code = code>>1;
	//btm
	if ( code%2 == 0 )
	{
		normal_[1] -= 1.0;
	}
	code = code>>1;
	//top
	if ( code%2 == 0 )
	{
		normal_[1] += 1.0;
	}
	return -normal_/2.0;
}

	
void EmitCube( vec4 pos, vec4 color, uint code )
{
	vec4 transVerts[8];
	for (int i=0;i<8; i++) 
	{
		transVerts[i]= (pvm)*(pos + cubeVerts[i]);
	}
	vec3 normal_ = GetNormal(code);
	for (int i=0;i<6;i++)
    {
		if ( code%2 == 1 )
		{
			EmitSide( pos, color, i*4, transVerts, normal_);
			EndPrimitive();
		}
		code = code>>1;
    }
    
}

void main()
{	
  EmitCube(gl_in[0].gl_Position, pass_Color_vert[0]/255.0, uint(pass_Color_vert[0][3]));
}  