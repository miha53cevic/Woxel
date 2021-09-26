#version 330
in layout(location = 0) vec3 position;

uniform mat4 MVPMatrix;

void main(void)
{
	gl_Position = MVPMatrix * vec4(position, 1.0);
}