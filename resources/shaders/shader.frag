#version 330

in vec2 pass_texture;

out vec4 Frag_Colour;

uniform sampler2D textureSampler;

void main(void)
{
	Frag_Colour = texture(textureSampler, pass_texture);
}