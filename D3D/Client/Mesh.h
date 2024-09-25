#pragma once

class Mesh
{
public:
	void Init(vector<Vertex>& vec, vector<uint32>& index);
	void Render();
	void CreateVertexBuffer(vector<Vertex>& vec);
	void CreateIndexBuffer(vector<uint32>& vec);

private:
	ComPtr<ID3D12Resource>		_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32 _vertexCount = 0;

	ComPtr<ID3D12Resource>		_IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};
	uint32 _indexCount = 0;
};

