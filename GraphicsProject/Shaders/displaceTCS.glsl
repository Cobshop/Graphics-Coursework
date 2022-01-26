#version  400  core
layout(vertices  = 4)  out;

in  Vertex    {
	vec2  texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN[];

out  Vertex   {
	vec2  texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT []; 

void  main() {
	gl_TessLevelInner [0] = 6400;
	gl_TessLevelInner [1] = 6400;

	gl_TessLevelOuter [0] = 6400;
	gl_TessLevelOuter [1] = 6400;
	gl_TessLevelOuter [2] = 6400;
	gl_TessLevelOuter [3] = 6400;
	
	OUT[gl_InvocationID ]. texCoord = IN[gl_InvocationID ]. texCoord;
	
	gl_out[gl_InvocationID ]. gl_Position = gl_in[gl_InvocationID ]. gl_Position;
}