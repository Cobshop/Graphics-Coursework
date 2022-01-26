#version 330 core

uniform  sampler2D  diffuseTex;
uniform  sampler2D  bumpTex;
uniform  sampler2D  shadowTex;

uniform  vec3 cameraPos;
uniform  vec4 specColour;
uniform  vec4 emissColour;
uniform  vec3 lightDirection;
uniform  vec3 lightPos;
uniform  float lightRadius;
uniform  float lightAngle;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
} IN;

out vec4 fragColour;
void main(void) {
	vec3 incident = normalize(lightPos  - IN.worldPos );
	//vec3 spotdirection = normalize(lightDirections[i] - lightPose[i]);
	//vec3 incident = normalize(-lightDirections[i] + lightPose[i]);
	vec3 viewDir = normalize(cameraPos  - IN.worldPos );
	vec3 halfDir = normalize(incident + viewDir );

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal ));
	vec4 diffuse = texture(diffuseTex , IN.texCoord );
	vec3 bumpNormal = texture(bumpTex , IN.texCoord ).rgb;
	bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 - 1.0));
	float specFactor;
	
	//if(dot(spotdirection,incident) > lightAngles[i]){
	float lambert = max(dot(incident , bumpNormal), 0.0f);
	float distance = length(lightPos  - IN.worldPos );
	float attenuation = 1.0 - clamp(distance / lightRadius , 0.0,  1.0);
	//float attenuation = 1.0;
	specFactor = clamp(dot(halfDir , bumpNormal ) ,0.0 ,1.0);
	specFactor = pow(specFactor , 60.0 );
	/*fragColour.rgb = surface * lambert * attenuation;
	fragColour.rgb += (specColours[i].rgb * specFactor ) * attenuation * 0.33;
	fragColour.rgb +=  surface * 0.1f;
	fragColour.a = diffuse.a;*/

	float shadow = 2.0;
	
	vec3 shadowNDC = IN.shadowProj.xyz / IN.shadowProj.w;
	if(abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
		vec3 biasCoord = shadowNDC  * 0.5f + 0.5f;
		float shadowZ = texture(shadowTex , biasCoord.xy).x;
		if(shadowZ  < biasCoord.z) {
			shadow = 0.0f;
		}
	}

	vec3 surface = (diffuse.rgb * emissColour.rgb);
	vec4 temp = vec4(surface * lambert * attenuation, 1.0);
	temp.rgb += (specColour.rgb * specFactor ) * attenuation * 0.33;
	temp.rgb *= shadow;
	temp.rgb +=  surface * 0.1f;
	temp.a = diffuse.a;

	fragColour += temp;
	//}
}