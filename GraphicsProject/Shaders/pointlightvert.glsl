#version  330  core
uniform  mat4  modelMatrix;
uniform  mat4  viewMatrix;
uniform  mat4  projMatrix;
uniform  mat4  shadowMatrix;

in   vec3  position;
in   vec3  normal;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec4 specColour;
uniform vec4 emissColour;

out  Vertex {
	vec4 shadowProj;
} OUT;

void  main(void) {
	vec3 zero = vec3(0,0,0);
	vec3 scale = vec3(lightRadius);
	vec4 worldPos = vec4((position*scale) + lightPos, 1.0);
	
	gl_Position = (projMatrix * viewMatrix) * worldPos;

	/*vec3 viewDir = normalize(lightPos  - worldPos.xyz);
	vec4 pushVal = vec4(normal  , 0) * dot(viewDir , normal );
	OUT.shadowProj = shadowMatrix * (worldPos + pushVal );*/
}