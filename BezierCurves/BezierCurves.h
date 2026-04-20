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

enum CurveStates
{
    WaitingP0,
    WaitingP1,
    WaitingP2P3,
	Adjusting,
};

// ------------------------------------------------------------------------------

class BezierCurves : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    ID3D12PipelineState* pointState = nullptr;

    VertexBuffer<Vertex>* vBuffer = nullptr;
    VertexBuffer<Vertex>* actualCurveBuffer = nullptr;
    VertexBuffer<Vertex>* aux = nullptr;

    static const uint MaxSize = 8192;
    static const int LineSegs = 30;

    Vertex actualCurve[LineSegs + 1];
    Vertex totalCurve[MaxSize] = {};
	Vertex P[4] = {};
    CurveStates state = WaitingP0;
    uint count = 0;
    
    Vertex actualCurveBackup[LineSegs + 1];
    Vertex totalCurveBackup[MaxSize] = {};
    Vertex PBackup[4] = {};
    CurveStates stateBackup = WaitingP0;
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
