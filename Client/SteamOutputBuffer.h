#pragma once

class StreamOutputBuffer
{
public:
    void Init(uint64 bufferSize);
    void Bind();
    void UnBind();
    void Render();

    ComPtr<ID3D12Resource> GetBuffer() const { return _StreamOutputBuffer; }
	ComPtr<ID3D12Resource> GetFilledSizeBuffer() const { return _FilledSizeBuffer; }

    D3D12_STREAM_OUTPUT_BUFFER_VIEW GetSOView() const { return _SOBufferView; }

private:
    ComPtr<ID3D12Resource> _StreamOutputBuffer;   // 스트림 출력 데이터 버퍼
    ComPtr<ID3D12Resource> _FilledSizeBuffer;     // 출력된 데이터 크기 추적용 버퍼
    ComPtr<ID3D12Resource> _FilledSizeReadbackBuffer;     // 출력된 데이터 크기 추적용 버퍼
    D3D12_STREAM_OUTPUT_BUFFER_VIEW _SOBufferView;

};