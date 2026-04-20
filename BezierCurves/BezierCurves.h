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
    ID3D12PipelineState* pointState = nullptr;

    VertexBuffer<Vertex>* vBuffer = nullptr;
    VertexBuffer<Vertex>* mountingCurve = nullptr;
    VertexBuffer<Vertex>* aux = nullptr;

    static const uint MaxSize = 8192;
    static const int LineSegs = 30;
    Vertex actualCurve[LineSegs + 1];
    Vertex vertices[MaxSize] = {};
	Vertex P[4] = {};
	uint numClicks = 0;
	bool isAdjusting = false;
    uint count = 0;
    
    Vertex actualCurveBackup[LineSegs + 1];
    Vertex verticesBackup[MaxSize] = {};
    Vertex PBackup[4] = {};
	uint numClicksBackup = 0;
    bool isAdjustingBackup = false;
    uint countBackup = 0;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();
    void BezierCurve(Vertex * curve, Vertex * P);

    void BuildRootSignature();
    void BuildPipelineState();
};

// ------------------------------------------------------------------------------
