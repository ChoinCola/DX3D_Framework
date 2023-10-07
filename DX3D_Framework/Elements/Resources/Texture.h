#pragma once

class Texture
{
	Texture(Vertex& vertices, vector<UINT> indices);
	~Texture();

	void Render();
	void Change_vertices(Vertex& vertices);
private:

	ID3D11Device* device;   // CPU
	ID3D11DeviceContext* deviceContext; // GPU
};

