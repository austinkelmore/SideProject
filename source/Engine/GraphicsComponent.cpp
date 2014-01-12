
#include "GraphicsComponent.h"
#include "IGraphics.h"

GraphicsComponent::GraphicsComponent()
	: _position(0.f, 0.f, 0.f)
{
	float temp[] = {0.25f,  0.25f, -1.25f, 1.0f,
		0.25f, -0.25f, -1.25f, 1.0f,
		-0.25f,  0.25f, -1.25f, 1.0f,

		0.25f, -0.25f, -1.25f, 1.0f,
		-0.25f, -0.25f, -1.25f, 1.0f,
		-0.25f,  0.25f, -1.25f, 1.0f,

		0.25f,  0.25f, -2.75f, 1.0f,
		-0.25f,  0.25f, -2.75f, 1.0f,
		0.25f, -0.25f, -2.75f, 1.0f,

		0.25f, -0.25f, -2.75f, 1.0f,
		-0.25f,  0.25f, -2.75f, 1.0f,
		-0.25f, -0.25f, -2.75f, 1.0f,

		-0.25f,  0.25f, -1.25f, 1.0f,
		-0.25f, -0.25f, -1.25f, 1.0f,
		-0.25f, -0.25f, -2.75f, 1.0f,

		-0.25f,  0.25f, -1.25f, 1.0f,
		-0.25f, -0.25f, -2.75f, 1.0f,
		-0.25f,  0.25f, -2.75f, 1.0f,

		0.25f,  0.25f, -1.25f, 1.0f,
		0.25f, -0.25f, -2.75f, 1.0f,
		0.25f, -0.25f, -1.25f, 1.0f,

		0.25f,  0.25f, -1.25f, 1.0f,
		0.25f,  0.25f, -2.75f, 1.0f,
		0.25f, -0.25f, -2.75f, 1.0f,

		0.25f,  0.25f, -2.75f, 1.0f,
		0.25f,  0.25f, -1.25f, 1.0f,
		-0.25f,  0.25f, -1.25f, 1.0f,

		0.25f,  0.25f, -2.75f, 1.0f,
		-0.25f,  0.25f, -1.25f, 1.0f,
		-0.25f,  0.25f, -2.75f, 1.0f,

		0.25f, -0.25f, -2.75f, 1.0f,
		-0.25f, -0.25f, -1.25f, 1.0f,
		0.25f, -0.25f, -1.25f, 1.0f,

		0.25f, -0.25f, -2.75f, 1.0f,
		-0.25f, -0.25f, -2.75f, 1.0f,
		-0.25f, -0.25f, -1.25f, 1.0f };

	for (int i = 0; i < sizeof(temp)/sizeof(temp[0]); ++i)
	{
		_vertices.push_back(temp[i]);
	}

	float temp2[] = {0.0f, 0.0f, 1.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f,

					0.0f, 0.0f, 1.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f,
					0.0f, 0.0f, 1.0f, 1.0f,

					0.8f, 0.8f, 0.8f, 1.0f,
					0.8f, 0.8f, 0.8f, 1.0f,
					0.8f, 0.8f, 0.8f, 1.0f,

					0.8f, 0.8f, 0.8f, 1.0f,
					0.8f, 0.8f, 0.8f, 1.0f,
					0.8f, 0.8f, 0.8f, 1.0f,

					0.0f, 1.0f, 0.0f, 1.0f,
					0.0f, 1.0f, 0.0f, 1.0f,
					0.0f, 1.0f, 0.0f, 1.0f,

					0.0f, 1.0f, 0.0f, 1.0f,
					0.0f, 1.0f, 0.0f, 1.0f,
					0.0f, 1.0f, 0.0f, 1.0f,

					0.5f, 0.5f, 0.0f, 1.0f,
					0.5f, 0.5f, 0.0f, 1.0f,
					0.5f, 0.5f, 0.0f, 1.0f,

					0.5f, 0.5f, 0.0f, 1.0f,
					0.5f, 0.5f, 0.0f, 1.0f,
					0.5f, 0.5f, 0.0f, 1.0f,

					1.0f, 0.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 0.0f, 1.0f,

					1.0f, 0.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 0.0f, 1.0f,
					1.0f, 0.0f, 0.0f, 1.0f,

					0.0f, 1.0f, 1.0f, 1.0f,
					0.0f, 1.0f, 1.0f, 1.0f,
					0.0f, 1.0f, 1.0f, 1.0f,

					0.0f, 1.0f, 1.0f, 1.0f,
					0.0f, 1.0f, 1.0f, 1.0f,
					0.0f, 1.0f, 1.0f, 1.0f };

	for (int i = 0; i < sizeof(temp2)/sizeof(temp2[0]); ++i)
	{
		_colors.push_back(temp2[i]);
	}
}

GraphicsComponent* GraphicsComponent::Create()
{
	return g_graphics->CreateGraphicsComponent();
}

void GraphicsComponent::Update()
{

}

int GraphicsComponent::GetVertexCount() const
{
	return _vertices.size() / 4;
}

std::vector<float> GraphicsComponent::GetVertices() const
{
	return _vertices;
}

std::vector<float> GraphicsComponent::GetColors() const
{
	return _colors;
}
