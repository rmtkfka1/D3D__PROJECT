#pragma once
#include "ResourceBase.h"

class Mesh:public ResourceBase
{
public:
	Mesh();
	virtual ~Mesh();

	void Init(vector<Vertex>& vec, vector<uint32>& index);
	void Init(vector<Vertex>& vec);
	void Render(D3D_PRIMITIVE_TOPOLOGY topolgy = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void RenderWithoutIndex(D3D_PRIMITIVE_TOPOLOGY topolgy = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void CreateVertexBuffer(vector<Vertex>& vec);
	void CreateIndexBuffer(vector<uint32>& vec);

	D3D12_VERTEX_BUFFER_VIEW& GetVertexView() { return _vertexBufferView; }
	D3D12_INDEX_BUFFER_VIEW& GetIndexView() { return _indexBufferView; }
	uint32 GetIndexCount() { return _indexCount; }

private:
	ComPtr<ID3D12Resource>		_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW	_vertexBufferView = {};
	uint32 _vertexCount = 0;

	ComPtr<ID3D12Resource>		_IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW		_indexBufferView = {};
	uint32 _indexCount = 0;
};

