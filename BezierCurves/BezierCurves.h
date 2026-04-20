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
    WAITING_P0,
    WAITING_P1,
    WAITING_P2P3,
	ADJUSTING,
};

// ------------------------------------------------------------------------------

enum SupportStates
{
    INNITIAL_SUPPORTS,
	ADJUSTING_SUPPORTS,
    DRAWING_SUPPORTS
};

// ------------------------------------------------------------------------------

class BezierCurves : public App
{
private:
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    ID3D12PipelineState* squareState = nullptr;

    VertexBuffer<Vertex>* vBuffer = nullptr;
    VertexBuffer<Vertex>* actualCurveBuffer = nullptr;
    VertexBuffer<Vertex>* supportSquaresBuffer = nullptr;
    VertexBuffer<Vertex>* actualSupportSquaresBuffer = nullptr;

    static const uint MaxSize = 8192;
    static const int LineSegs = 30;

	Vertex totalSupportSquares[24];
	Vertex actualSupportSquares[12];
	uint supportSquaresCount;
    SupportStates supportState;

    Vertex actualCurve[LineSegs + 1];
    Vertex totalCurve[MaxSize];
	Vertex P[4];
    CurveStates state;
    uint count;

    Vertex totalSupportSquaresBackup[12];
    Vertex actualSupportSquaresBackup[12];
    uint supportSquaresCountBackup;
    
    Vertex actualCurveBackup[LineSegs + 1];
    Vertex totalCurveBackup[MaxSize];
    Vertex PBackup[4];
    CurveStates stateBackup;
    uint countBackup;

public:
    void Init();
    void Update();
    void Display();
    void Finalize();
    void BezierCurve(Vertex * curve, Vertex * P);
	void GetSupportSquaresVertex(Vertex* supportSquareVertex, Vertex vertex, float x, float y);

    void BuildRootSignature();
    void BuildPipelineState();
};

// ------------------------------------------------------------------------------
