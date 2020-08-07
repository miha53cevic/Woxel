#version 330
in vec3 position;

uniform mat4 MVPMatrix;

void main(void)
{
	gl_Position = MVPMatrix * vec4(position, 1.0);
}