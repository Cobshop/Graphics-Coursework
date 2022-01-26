#version 330 core

uniform  sampler2D  diffuseTex;
uniform  sampler2D  bumpTex;
uniform  samplerCube  cubeTex;

uniform  vec3 cameraPos;

in Vertex{
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour[2];
void main(void) {
	 mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal ));
	 vec4 diffuse = texture(diffuseTex , IN.texCoord );
	 vec3 bumpNormal = texture(bumpTex , IN.texCoord ).rgb;
	 bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
	 vec3 viewDir = normalize(cameraPos  - IN.worldPos );
	 
	 vec3 reflectDir = reflect(-viewDir ,normalize(IN.normal));
	 vec4 reflectTex = texture(cubeTex ,reflectDir);
	 
	 fragColour[0] = vec4(1, 0, 0, 1.0f) * (reflectTex + (diffuse * 0.5f));
	 fragColour[1] = 0.1*vec4(bumpNormal.xyz * 0.5 + 0.5 ,1.0);

	 //fragColour = texture(diffuseTex ,IN.texCoord );
}