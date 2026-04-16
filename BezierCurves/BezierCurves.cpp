/**********************************************************************************
// Curves (Código Fonte)
//
// Criação:     12 Ago 2020
// Atualização: 21 Mai 2025
// Compilador:  Visual C++ 2022
//
// Descrição:   Base para gerar curvas usando Corner-Cutting
//
**********************************************************************************/

#include "BezierCurves.h"

// ------------------------------------------------------------------------------

void BezierCurves::Init()
{   
    // ------------------
    // Aloca Geometria
    // ------------------
 
    // cria vertex buffer
    curveBuffer = new VertexBuffer<Vertex>(nullptr, MaxSize);
    curveBufferBackup = new VertexBuffer<Vertex>(nullptr, MaxSize);
    squareBuffer = new VertexBuffer<Vertex>(nullptr, 8);
    lineBuffer = new VertexBuffer<Vertex>(nullptr, 2);

    // ------------------
    // Ajusta Pipeline
    // ------------------

    BuildRootSignature();
    BuildPipelineState();

    // ------------------
}

// ------------------------------------------------------------------------------

void BezierCurves::Update()
{
    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    // cria vértices com o botão do mouse
    if (input->KeyPress(VK_LBUTTON))
    {
        float cx = float(window->CenterX());
        float cy = float(window->CenterY());
        float mx = float(input->MouseX());
        float my = float(input->MouseY());
        
        // converte as coordenadas da tela para a faixa -1.0 a 1.0
        // cy e my foram invertidos para levar em consideração que 
        // o eixo y da tela cresce na direção oposta do cartesiano
        float x = (mx - cx) / cx;
        float y = (cy - my) / cy;

        supportPoints[index] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::White) };
        index = (index + 1) % 4;
        
        if (points < MaxSize)
            ++points;
    }

    if (points > 3) 
    {
		float t, x, y;
        Vertex curve[LineSegs + 1];
        for (int i = 0; i <= LineSegs; i++) {
            t = 1.0f / LineSegs * i;
			x = pow(1.0f - t, 3) * supportPoints[0].Pos.x
                + 3 * t * pow(1.0f - t, 2) * supportPoints[1].Pos.x
                + 3 * t * t * (1.0f - t) * supportPoints[2].Pos.x
                + t * t * t * supportPoints[3].Pos.x;
            y = pow(1.0f - t, 3) * supportPoints[0].Pos.y
                + 3 * t * pow(1.0f - t, 2) * supportPoints[1].Pos.y
                + 3 * t * t * (1.0f - t) * supportPoints[2].Pos.y
                + t * t * t * supportPoints[3].Pos.y;
            curve[i] = { XMFLOAT3(x, y, 0.0f), XMFLOAT4(Colors::White) };
        }

        curveBuffer->Copy(curve, LineSegs + 1);
		points = LineSegs + 1;
        // desenha curva
        Display();
    }

    // salva os vértices já criados
    if(input->KeyPress('S')){}

	// carrega vértices previamente salvos
    if(input->KeyPress('L')){}

	// limpa vértices
    if(input->KeyPress(VK_DELETE)){}
}

// ------------------------------------------------------------------------------

void BezierCurves::Display()
{
    // limpa backbuffer
    graphics->Clear();

    // submete comandos de configuração do pipeline
    graphics->CommandList()->SetPipelineState(pipelineState);
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetVertexBuffers(0, 1, curveBuffer->View());
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

    // submete comandos de desenho
    graphics->CommandList()->DrawInstanced(points, 1, 0, 0);

    // apresenta backbuffer
    graphics->Present();    
}

// ------------------------------------------------------------------------------

void BezierCurves::Finalize()
{
    // espera GPU finalizar comandos pendentes
    graphics->WaitForGpu();

    // libera memória alocada
    rootSignature->Release();
    pipelineState->Release();
    delete curveBuffer;
	delete squareBuffer;
	delete lineBuffer;
	delete curveBufferBackup;
}

// ------------------------------------------------------------------------------
//                                  WinMain                                      
// ------------------------------------------------------------------------------

int APIENTRY WinMain(_In_ HINSTANCE hInstance,    _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    try
    {
        // cria motor e configura a janela
        Engine* engine = new Engine();
        engine->window->Mode(ASPECTRATIO);
        engine->window->Size(1024, 600);
        engine->window->Color(25, 25, 25);
        engine->window->Title("Bézier Curves");
        engine->window->Icon("Icon");
        engine->window->LostFocus(Engine::Pause);
        engine->window->InFocus(Engine::Resume);

        // cria e executa a aplicação
        engine->Start(new BezierCurves());

        // finaliza execução
        delete engine;
    }
    catch (Error & e)
    {
        // exibe mensagem em caso de erro
        MessageBox(nullptr, e.ToString().data(), "BezierCurves", MB_OK);
    }

    return 0;
}

// ----------------------------------------------------------------------------
