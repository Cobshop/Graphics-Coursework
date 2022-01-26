#version 330 core

uniform  sampler2D  diffuseTex;
uniform  sampler2D  bumpTex;
uniform  sampler2D  grassTex;
uniform  sampler2D  grassBumpTex;
uniform  sampler2D  rockTex;
uniform  sampler2D  rockBumpTex;
uniform int useTexture;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour[2];
void main(void) {
	int offset = 300;
	vec2 gravelLayer = vec2(1, 200);
	vec2 rockLayer = vec2(gravelLayer.y + offset, gravelLayer.y + offset*2);
	vec2 grassLayer = vec2(rockLayer.y + offset, rockLayer.y + offset*2);
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal ));
	vec3 bumpNormal = texture2D(bumpTex , IN.texCoord ).rgb * 2.0 - 1.0;
	if(IN.worldPos.y < gravelLayer.y)
			bumpNormal *= texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;
		else if(IN.worldPos.y < rockLayer.x && IN.worldPos.y > gravelLayer.y)
			bumpNormal *= mix(texture2D(rockBumpTex, IN.texCoord).rgb * 2.0 - 1.0, texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0, (rockLayer.x-IN.worldPos.y)/offset);
		else if(IN.worldPos.y > rockLayer.x && IN.worldPos.y < rockLayer.y)
			bumpNormal *= texture2D(rockBumpTex, IN.texCoord).rgb * 2.0 - 1.0;
		else if(IN.worldPos.y < grassLayer.x && IN.worldPos.y > rockLayer.y)
			bumpNormal *= mix(texture2D(grassBumpTex, IN.texCoord).rgb * 2.0 - 1.0, texture2D(rockBumpTex, IN.texCoord).rgb * 2.0 - 1.0, (grassLayer.x-IN.worldPos.y)/offset);
		else if(IN.worldPos.y > grassLayer.x)
			bumpNormal *= texture2D(grassBumpTex, IN.texCoord).rgb * 2.0 - 1.0;
	bumpNormal = normalize(TBN * normalize(bumpNormal));
	

	fragColour[0] = IN.colour;
	fragColour[1] = vec4(bumpNormal.xyz * 0.5 + 0.5 ,1.0);
	if(useTexture > 1){
		if(IN.worldPos.y < gravelLayer.y)
			fragColour[0] *= texture(diffuseTex, IN.texCoord);
		else if(IN.worldPos.y < rockLayer.x && IN.worldPos.y > gravelLayer.y)
			fragColour[0] *= mix(texture(rockTex, IN.texCoord), texture(diffuseTex, IN.texCoord), (rockLayer.x-IN.worldPos.y)/offset);
		else if(IN.worldPos.y > rockLayer.x && IN.worldPos.y < rockLayer.y)
			fragColour[0] *= texture(rockTex, IN.texCoord);
		else if(IN.worldPos.y < grassLayer.x && IN.worldPos.y > rockLayer.y)
			fragColour[0] *= mix(texture(grassTex, IN.texCoord), texture(rockTex, IN.texCoord), (grassLayer.x-IN.worldPos.y)/offset);
		else if(IN.worldPos.y > grassLayer.x)
			fragColour[0] *= texture(grassTex, IN.texCoord);
		fragColour[1] = vec4(bumpNormal.xyz * 0.5 + 0.5 ,1.0);
	}else if(useTexture > 0){
		fragColour[0] *= texture(diffuseTex, IN.texCoord);
		fragColour[1] = vec4(bumpNormal.xyz * 0.5 + 0.5 ,1.0);
	}
}