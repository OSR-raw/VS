#version 410 core
#define Lightning

in vec3 normal;
in vec3 position;
in float color_scale;

#ifdef Lightning
	vec3 light = vec3( 100, 100, -100 );
	//vec3 lightHalfDir = light/2.0;
#endif

layout(location = 0) out vec4 out_Color;
void main(void)
{
    out_Color = vec4(0.0,1.0,0.0,(-color_scale+ 1.0)/2.0);
	//out_Color = vec4(0.0,0.0,1.0, 0.5);
//return;
#ifdef Lightning
	{
	//out_Color.rgb = position/length(position);
		vec3 light_dir = normalize(light - position);		
		out_Color.rgb = out_Color.rgb*max(dot(normal, light_dir), 0.3);
	}
#endif		
}