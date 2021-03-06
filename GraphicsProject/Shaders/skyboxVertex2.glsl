#version  330  core
uniform  mat4  modelMatrix;
uniform  mat4  viewMatrix;
uniform  mat4  projMatrix;

in   vec3  position;
in   vec3  normal;

out  Vertex {
	vec3 viewDir;
	vec3 normal;
	vec3 worldPos;
} OUT;

void  main(void) {
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix )));

	vec3 wNormal = normalize(normalMatrix * normalize(normal));

	OUT.normal = wNormal;

	vec4 worldPos = (modelMatrix * vec4(position ,1));
	
	OUT.worldPos = worldPos.xyz;
	
	gl_Position = (projMatrix * viewMatrix) * worldPos;
}