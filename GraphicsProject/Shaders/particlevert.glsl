#version  330  core

in   vec4  position;
in   vec2  texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec2 offset;
uniform vec4 colour;

out  Vertex {
	vec2 texCoord;
	vec4 particleColour;
} OUT;

void  main(void) {
	OUT.particleColour = colour;

	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position.xy * 10 + offset, 0.0, 1.0);

	OUT.texCoord = texCoord;
}