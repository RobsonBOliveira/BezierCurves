/**********************************************************************************
// Curves (Arquivo de Cabeçalho)
//
// Criação:     12 Ago 2020
// Atualização: 21 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Base para gerar curvas usando Corner-Cutting
//
**********************************************************************************/

#include "DXUT.h"

// ------------------------------------------------------------------------------

struct Vertex
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

// ------------------------------------------------------------------------------

class BezierCurves : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;

    VertexBuffer<Vertex>* curveBuffer = nullptr;
    VertexBuffer<Vertex>* curveBufferBackup = nullptr;
    VertexBuffer<Vertex>* squareBuffer = nullptr;
    VertexBuffer<Vertex>* lineBuffer = nullptr;

    static const uint MaxSize = 8192;
    static const int LineSegs = 100;
    Vertex vertices[MaxSize];
    Vertex supportPoints[4];
    uint points = 0;
    uint count = 0;
    uint index = 0;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();

    void BuildRootSignature();
    void BuildPipelineState();
};

// ------------------------------------------------------------------------------
