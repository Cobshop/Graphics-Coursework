#version 330 core

uniform  sampler2D  diffuseTex;
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
	vec3 worldPos;
} IN;

out vec4 fragColour;
void main(void) {
	for(int i = 0; i < 4; i++){
		vec3 incident = normalize(lightPose[i]  - IN.worldPos );
		vec3 spotdirection = normalize(lightDirections[i] - lightPose[i]);
		//vec3 incident = normalize(-lightPose[i]);
		vec3 viewDir = normalize(cameraPos  - IN.worldPos );
		vec3 halfDir = normalize(incident + viewDir );
		vec4 diffuse = texture(diffuseTex , IN.texCoord );
		float specFactor;
		
		if(dot(spotdirection,incident) > lightAngles[i]){
		float lambert = max(dot(incident , IN.normal), 0.0f);
		float distance = length(lightPose[i]  - IN.worldPos );
		float attenuation = 1.0 - clamp(distance / lightRadii[i] , 0.0,  1.0);
		//float attenuation = 1.0;
			specFactor = clamp(dot(halfDir , IN.normal ) ,0.0 ,1.0);
			specFactor = pow(specFactor , 60.0 );
			vec3 surface = (diffuse.rgb * emissColours[i].rgb);
		fragColour.rgb = surface * lambert * attenuation;
		fragColour.rgb += (specColours[i].rgb * specFactor ) * attenuation * 0.33;
		fragColour.rgb +=  surface * 0.1f;
		fragColour.a = diffuse.a;
		}
		
	}
}