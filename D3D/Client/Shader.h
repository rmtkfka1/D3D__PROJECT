#pragma once

#include "ResourceBase.h"
enum class ShaderType
{
	FORWARD,
	DEFREED,
};

enum class RASTERIZER_TYPE
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,
	WIREFRAME,
};

enum class DEPTH_STENCIL_TYPE : uint8
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	NO_DEPTH_TEST, // ±Ì¿Ã ≈◊Ω∫∆Æ(X) + ±Ì¿Ã ±‚∑œ(O)
	NO_DEPTH_TEST_NO_WRITE, // ±Ì¿Ã ≈◊Ω∫∆Æ(X) + ±Ì¿Ã ±‚∑œ(X)
	DPTH_TEST_NO_WRITE, // ±Ì¿Ã ≈◊Ω∫∆Æ(O) + ±Ì¿Ã ±‚∑œ(X)
};

enum class BLEND_TYPE : uint8
{
	DEFAULT,
	ALPHA_BLEND,
	ONE_TO_ONE_BLEND,
	END,
};

struct ShaderInfo
{

	ShaderType shaderType = ShaderType::FORWARD;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	RASTERIZER_TYPE rasterizerType = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE depthStencilType = DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE blendType = BLEND_TYPE::DEFAULT;
	//bool bActiveHSShader = false;
	//bool bActiveDSShader = false;
	bool bActiveGSShader = false;
	bool bActvieStreamOutput = false;
};


class Shader :public ResourceBase
{

public:
	Shader();
	virtual ~Shader();

protected:
	void CreateShader(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob, D3D12_SHADER_BYTECODE& shaderByteCode);

	ComPtr<ID3D12PipelineState>			_pipelineState;
	ComPtr<ID3DBlob>					_errBlob;
};

class GraphicsShader :public Shader
{
public:
	GraphicsShader();
	virtual ~GraphicsShader();

	void Init(const wstring& path, ShaderInfo info = ShaderInfo());
	void SetPipelineState();

private:

	void CreateVertexShader(const wstring& path, const string& name, const string& version);
	void CreatePixelShader(const wstring& path, const string& name, const string& version);
	void CreateGeometryShader(const wstring& path, const string& name, const string& version);

private:
	ShaderInfo _info;

	ComPtr<ID3DBlob>					_vsBlob;
	ComPtr<ID3DBlob>					_psBlob;
	ComPtr<ID3DBlob>					_gsBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC  _pipelineDesc = {};

	wstring _path = L"../Resources/Shader/";
};


class ComputeShader :public Shader
{
public:
	ComputeShader();
	virtual ~ComputeShader();

	void Init(const wstring& path);
	void SetPipelineState();

private:

	ComPtr<ID3DBlob>					_csBlob;
	D3D12_COMPUTE_PIPELINE_STATE_DESC   _pipelineDesc = {};
	wstring _path = L"../Resources/Shader/";
};





