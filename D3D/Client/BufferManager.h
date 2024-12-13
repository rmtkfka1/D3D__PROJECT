#pragma once


class DescriptorTable;
class ComputeDescriptorTable;
class TextureBufferPool;

class ConstantBufferPool;


class BufferManager
{

public:
	BufferManager();
	~BufferManager();

	void SetIndex(int index) { _currentContextIndex = index; }
	void Init();
	void Clear(int nextValue);

	shared_ptr<DescriptorTable>& GetTable() { return _table[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetWorldBufferPool() { return _WorldBufferPool[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetCameraBufferPool() { return _CameraBufferPool[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetLIghtBufferPool() { return _lightBufferPool[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetMaterialParamsBufferPool() { return _materialParamsBufferPool[_currentContextIndex]; }
	shared_ptr<ConstantBufferPool>& GetAnimationBufferPool() { return _aniMationBufferPool[_currentContextIndex]; }
	shared_ptr<TextureBufferPool>& GetTextureBufferPool() { return _textureBufferPool; }

private:
	//Buffer pool
	array<shared_ptr<DescriptorTable>, MAX_FRAME_COUNT>  _table;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _lightBufferPool;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _WorldBufferPool;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _CameraBufferPool;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _materialParamsBufferPool;
	array<shared_ptr<ConstantBufferPool>, MAX_FRAME_COUNT> _aniMationBufferPool;
	shared_ptr<TextureBufferPool>  _textureBufferPool;

private:
	uint64 _currentContextIndex=0;


};

