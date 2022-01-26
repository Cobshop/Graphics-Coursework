#version  330  core
uniform  mat4  modelMatrix;
uniform  mat4  viewMatrix;
uniform  mat4  projMatrix;

in   vec3  position;
in   vec3  normal;

uniform vec3 lightPos;

void  main(void) {
	vec4 worldPos = (vec4(position ,1));
	
	gl_Position = (projMatrix * viewMatrix) * modelMatrix * worldPos;

}