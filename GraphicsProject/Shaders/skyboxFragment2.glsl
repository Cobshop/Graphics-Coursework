#version 330 core

uniform  samplerCube  cubeTex2;

uniform  vec3 cameraPos;

in Vertex{
	vec3 viewDir;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;
void main(void) {
	 vec3 viewDir = normalize(cameraPos  - IN.worldPos );
	 
	 vec3 reflectDir = reflect(-viewDir ,normalize(IN.normal));
	 vec4 reflectTex = texture(cubeTex2 ,reflectDir);
	 
	 fragColour = reflectTex;
}