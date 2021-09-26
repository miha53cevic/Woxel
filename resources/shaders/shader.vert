#version 330
in layout(location = 0) vec3 position;
in layout(location = 1) vec2 textureCoords;

out vec2 pass_texture;

uniform mat4 MVPMatrix;

void main(void)
{
	gl_Position = MVPMatrix * vec4(position, 1.0);
	pass_texture = textureCoords;
}