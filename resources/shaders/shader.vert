#version 330
in vec3 position;
in vec2 textureCoords;

out vec2 pass_texture;

uniform mat4 MVPMatrix;

void main(void)
{
	gl_Position = MVPMatrix * vec4(position, 1.0);
	pass_texture = textureCoords;
}