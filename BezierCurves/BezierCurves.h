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
#include <vector>
using namespace std;

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
	vector <Vertex> backupVertices;
	vector <Vertex> vertices;
	Vertex P0, P1, P2, P3;
	uint numClicks = 0;
	bool isAdjusting = false;
    uint points = 0;
    uint count = 0;
    uint index = 0;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();
    void BezierCurve(Vertex * curve, Vertex P0, Vertex P1, Vertex P2, Vertex P3);

    void BuildRootSignature();
    void BuildPipelineState();
};

// ------------------------------------------------------------------------------
