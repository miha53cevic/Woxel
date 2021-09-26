#version 330
in layout(location = 0) vec2 position;
in layout(location = 1) vec2 textureCoords;

out vec2 pass_texture;

uniform mat4 model;
uniform mat4 projection;

void main(void)
{
	gl_Position = projection * model * vec4(position.xy, 0.0, 1.0);
	pass_texture = textureCoords;
}