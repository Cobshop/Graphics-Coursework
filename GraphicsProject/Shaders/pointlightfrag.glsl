#version 330 core

uniform  mat4  shadowMatrix;
uniform  sampler2D  depthTex;
uniform  sampler2D  normTex;
uniform  sampler2D  shadowTex;

uniform  vec2 pixelSize;
uniform  vec3 cameraPos;

uniform  vec4 specColour;
uniform  vec4 emissColour;
uniform  vec3 lightDirection;
uniform  vec3 lightPos;
uniform  float lightRadius;
uniform  float lightAngle;
uniform  mat4 inverseProjView;

out vec4 diffuseOutput;
out vec4 specularOutput;
void main(void) {
	 vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize );
	 float depth = texture(depthTex , texCoord.xy).r;
	 vec3 ndcPos = vec3(texCoord , depth) * 2.0 - 1.0;
	 vec4 invClipPos = inverseProjView * vec4(ndcPos , 1.0);
	 vec3 worldPos = invClipPos.xyz / invClipPos.w;
	 float dist = length(lightPos  - worldPos );
	 vec3 zero = vec3(0,0,0);
	 float atten = 1.0;
	 vec3 incident = normalize(lightPos  - worldPos );
	 vec3 normal = normalize(texture(normTex , texCoord.xy).xyz *2.0 -1.0);
	 vec3 viewDir = normalize(cameraPos  - worldPos );
	 
	 if(lightDirection != zero){
		atten = 5*(1.0 - clamp(dist / lightRadius , 0.0,  1.0));
		incident = normalize(lightPos  - worldPos );
	}else{
		atten = 2;
		incident = normalize(lightPos);
	}
	vec3  halfDir = normalize(incident + viewDir );

	 if(atten == 0.0) {
		discard;
	 }

	float shadow = 1.0;

	vec3 viewDire = normalize(lightPos  - worldPos.xyz);
	vec4 pushVal = vec4(normal  , 0) * dot(viewDire , normal );
	vec4 shadowNDC = shadowMatrix * vec4(worldPos + 100 * pushVal.xyz, 1);
	shadowNDC.xyz /= shadowNDC.w;
	//vec3 biasCoord2 = shadowNDC  * 0.5f + 0.5f;
	if(abs(shadowNDC.x) < 1.0f && abs(shadowNDC.y) < 1.0f && abs(shadowNDC.z) < 1.0f) {
		vec3 biasCoord = shadowNDC.xyz  * 0.5f + 0.5f;
		float shadowZ = texture(shadowTex , biasCoord.xy).x;
		if(shadowZ + 0.0000000000000001f< biasCoord.z) {
			shadow = 0.0f;
		}
	}
	 
	 float lambert = clamp(dot(incident , normal ) ,0.0 ,1.0);
	 //lambert *= shadow;
	 float rFactor = clamp(dot(halfDir , normal ) ,0.0 ,1.0);
	 float specFactor = clamp(dot(halfDir , normal ) ,0.0 ,1.0);
	 specFactor = pow(specFactor , 60.0 );
	 vec3 attenuated = specColour.xyz * atten;
	 diffuseOutput =  vec4(attenuated * lambert, 1.0);// vec4(biasCoord2.xyz, 1.0);//texture(shadowTex , biasCoord2.xy);//
	 diffuseOutput *= shadow;
	 specularOutput = vec4(attenuated * specFactor * 0.33,  1.0);// vec4(0,0,0,0);//
	 //specularOutput *= shadow;
}