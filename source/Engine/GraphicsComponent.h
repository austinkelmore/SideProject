
#pragma once

#include "Vector3.h"
#include <vector>

class GraphicsComponent
{
public:
	GraphicsComponent();

	static GraphicsComponent* Create();

	void Update();

	int GetVertexCount() const;
	std::vector<float> GetVertices() const;
	std::vector<float> GetColors() const;

private:
	Vector3 _position;
	std::vector<float> _vertices;
	std::vector<float> _colors;
};
