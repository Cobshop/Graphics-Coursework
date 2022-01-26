#version 330 core

uniform  sampler2D  diffuseTex;
uniform  sampler2D  bumpTex;
uniform  vec3 cameraPos;
uniform  vec4 specColours[4];
uniform  vec4 emissColours[4];
uniform  vec3 lightDirections[4];
uniform  vec3 lightPose[4];
uniform  float lightRadii[4];
uniform  float lightAngles[4];

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 fragColour;
void main(void) {
	for(int i = 0; i < 4; i++){
		//vec3 incident = normalize(lightPose[i]  - IN.worldPos );
		vec3 spotdirection = normalize(lightDirections[i] - lightPose[i]);
		vec3 incident = normalize(-lightDirections[i] + lightPose[i]);
		vec3 viewDir = normalize(cameraPos  - IN.worldPos );
		vec3 halfDir = normalize(incident + viewDir );

		mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal ));
		vec4 diffuse = texture(diffuseTex , IN.texCoord );
		vec3 bumpNormal = texture(bumpTex , IN.texCoord ).rgb;
		bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
		float specFactor;
		
		//if(dot(spotdirection,incident) > lightAngles[i]){
		float lambert = max(dot(incident , bumpNormal), 0.0f);
		float distance = length(lightPose[i]  - IN.worldPos );
		//float attenuation = 1.0 - clamp(distance / lightRadii[i] , 0.0,  1.0);
		float attenuation = 1.0;
			specFactor = clamp(dot(halfDir , bumpNormal ) ,0.0 ,1.0);
			specFactor = pow(specFactor , 60.0 );
			vec3 surface = (diffuse.rgb * emissColours[i].rgb);
		/*fragColour.rgb = surface * lambert * attenuation;
		fragColour.rgb += (specColours[i].rgb * specFactor ) * attenuation * 0.33;
		fragColour.rgb +=  surface * 0.1f;
		fragColour.a = diffuse.a;*/

		vec4 temp = vec4(surface * lambert * attenuation, 1.0);
		temp.rgb += (specColours[i].rgb * specFactor ) * attenuation * 0.33;
		temp.rgb +=  surface * 0.1f;
		temp.a = diffuse.a;

		fragColour += temp;
		//}
		
	}
}