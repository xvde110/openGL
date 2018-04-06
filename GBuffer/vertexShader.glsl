#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

void main()
{
	gl_Position= projectionMatrix * viewMatrix * modelMatrix * vec4(position,1.0);
	Normal=normal;
	Position=position;
	TexCoord=texCoord;
}