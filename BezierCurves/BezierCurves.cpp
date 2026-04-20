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
 
    // cria vertex buffers
    vBuffer = new VertexBuffer<Vertex>(nullptr, MaxSize);
    supportSquaresBuffer = new VertexBuffer<Vertex>(nullptr, 24);
    actualSupportSquaresBuffer = new VertexBuffer<Vertex>(nullptr, 12);
    actualCurveBuffer = new VertexBuffer<Vertex>(nullptr, LineSegs + 1);

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
    float cx = float(window->CenterX());
    float cy = float(window->CenterY());
    float mx = float(input->MouseX());
    float my = float(input->MouseY());

    // converte as coordenadas da tela para a faixa -1.0 a 1.0
    // cy e my foram invertidos para levar em consideração que 
    // o eixo y da tela cresce na direção oposta do cartesiano
    float x = (mx - cx) / cx;
    float y = (cy - my) / cy;

    // sai com o pressionamento da tecla ESC
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    // cria vértices com o botão do mouse
    if (input->KeyPress(VK_LBUTTON))
    {
        switch (state)
        {
            case WAITING_P0:
                P[0] = {XMFLOAT3{x, y, 0}, XMFLOAT4{Colors::White}};
				state = WAITING_P1;
				break;

            case WAITING_P1:
				P[1] = { XMFLOAT3{x, y, 0}, XMFLOAT4{Colors::White} };
				state = WAITING_P2P3;
                break;

			case WAITING_P2P3:
                P[2] = { XMFLOAT3{x, y, 0}, XMFLOAT4{Colors::White} };
                P[3] = { XMFLOAT3{x, y, 0}, XMFLOAT4{Colors::White} };

                BezierCurve(actualCurve, P);
				actualCurveBuffer->Copy(actualCurve, LineSegs + 1);

                for(int i = 0; i < 12; i++)
                    totalSupportSquares[supportSquaresCount + i] = actualSupportSquares[i];

				supportSquaresBuffer->Copy(totalSupportSquares, 12);

				state = ADJUSTING;

                break;

            case ADJUSTING:
                P[0] = P[3];
                P[1] = { XMFLOAT3{x, y, 0}, XMFLOAT4{Colors::White} };

                for(int i = 0; i < LineSegs+1; i++)
                    totalCurve[count + i] = actualCurve[i];

                count += LineSegs + 1;
				vBuffer->Copy(totalCurve, count);

				state = WAITING_P2P3;
				break;
        }
    }

    if (state == WAITING_P1)
    {
		GetSupportSquaresVertex(actualSupportSquares, P[0], x, y);
		actualSupportSquaresBuffer->Copy(actualSupportSquares, 12);
    }

    if (state == ADJUSTING) 
    {
        float lastP2x = P[2].Pos.x;
        float lastP2y = P[2].Pos.y;

        GetSupportSquaresVertex(actualSupportSquares, P[3], x, y);
        actualSupportSquaresBuffer->Copy(actualSupportSquares, 12);

        if (lastP2x != x || lastP2y != y) 
        {
            P[2].Pos.x = (2 * P[3].Pos.x - x);
            P[2].Pos.y = (2 * P[3].Pos.y - y);

            BezierCurve(actualCurve, P);
            actualCurveBuffer->Copy(actualCurve, LineSegs + 1);
		}
    }

    
    // salva os vértices já criados
    if(input->KeyPress('S'))
    {
        for(uint i = 0; i < count; i++)
			totalCurveBackup[i] = totalCurve[i];

        for (uint i = 0; i < LineSegs + 1; i++)
            actualCurveBackup[i] = actualCurve[i];

        for (uint i = 0; i < 12; i++)
        {
            totalSupportSquaresBackup[i] = totalSupportSquares[i];
            actualSupportSquaresBackup[i] = actualSupportSquares[i];
        }

		PBackup[0] = P[0];
		PBackup[1] = P[1];
        PBackup[2] = P[2];
        PBackup[3] = P[3];

        countBackup = count;
		stateBackup = state;
		supportSquaresCountBackup = supportSquaresCount;
    }

	// carrega vértices previamente salvos
    if(input->KeyPress('L'))
    {
        for(uint i = 0; i < countBackup; i++)
			totalCurve[i] = totalCurveBackup[i];

        for(uint i = 0; i < LineSegs + 1; i++)
			actualCurve[i] = actualCurveBackup[i];

        for (uint i = 0; i < 12; i++)
        {
			totalSupportSquares[i] = totalSupportSquaresBackup[i];
			actualSupportSquares[i] = actualSupportSquaresBackup[i];
        }


		P[0] = PBackup[0];
		P[1] = PBackup[1];
		P[2] = PBackup[2];
		P[3] = PBackup[3];

        count = countBackup;
		state = stateBackup;
        supportSquaresCount = supportSquaresCountBackup;
    }

	// limpa vértices
    if(input->KeyPress(VK_DELETE))
    {
		state = WAITING_P0;
		count = 0;
    }

    Display();
}

// ------------------------------------------------------------------------------

void BezierCurves::Display()
{
    // limpa backbuffer
    graphics->Clear();

    // submete comandos de configuração do pipeline
    graphics->CommandList()->SetPipelineState(pipelineState);
    graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
    graphics->CommandList()->IASetVertexBuffers(0, 1, vBuffer->View());
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);

    // submete comandos de desenho
    graphics->CommandList()->DrawInstanced(count, 1, 0, 0);

    // desenha linha em construção
    if (state == ADJUSTING)
    {
        graphics->CommandList()->IASetVertexBuffers(0, 1, actualCurveBuffer->View());
        graphics->CommandList()->DrawInstanced(LineSegs + 1, 1, 0, 0);
    }

    //desenho dos pontos de controle
    graphics->CommandList()->SetPipelineState(squareState);
    graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphics->CommandList()->IASetVertexBuffers(0, 1, actualSupportSquaresBuffer->View());
    graphics->CommandList()->DrawInstanced(12, 1, 0, 0);
    graphics->CommandList()->IASetVertexBuffers(0, 1, supportSquaresBuffer->View());
    graphics->CommandList()->DrawInstanced(12, 1, 0, 0);

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
	squareState->Release();
    delete vBuffer;
    delete supportSquaresBuffer;
    delete actualSupportSquaresBuffer;
	delete actualCurveBuffer;
}

// ------------------------------------------------------------------------------

void BezierCurves::BezierCurve(Vertex * curve, Vertex * P)
{
    double t, x, y;

    for (int i = 0; i <= LineSegs; i++) {
        t = 1.0 / LineSegs * i;
        x = pow(1.0 - t, 3) * P[0].Pos.x
            + 3 * t * pow(1.0 - t, 2) * P[1].Pos.x
            + 3 * t * t * (1.0 - t) * P[2].Pos.x
            + t * t * t * P[3].Pos.x;
        y = pow(1.0 - t, 3) * P[0].Pos.y
            + 3 * t * pow(1.0 - t, 2) * P[1].Pos.y
            + 3 * t * t * (1.0 - t) * P[2].Pos.y
            + t * t * t * P[3].Pos.y;

        curve[i] = { XMFLOAT3{float(x), float(y), 0}, XMFLOAT4{Colors::White}};
    }
}

// ------------------------------------------------------------------------------

void BezierCurves::GetSupportSquaresVertex(Vertex * supportSquareVertex, Vertex vertex, float x, float y)
{
	float size = 0.005f;
	float oppositeX = (2 * vertex.Pos.x - x);
	float oppositeY = (2 * vertex.Pos.y - y);

    supportSquareVertex[0] = { XMFLOAT3{x - size, y - size, 0}, XMFLOAT4{ Colors::Red } };
    supportSquareVertex[1] = { XMFLOAT3{x + size, y - size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[2] = { XMFLOAT3{x + size, y + size, 0}, XMFLOAT4{Colors::Red} };

    supportSquareVertex[3] = { XMFLOAT3{x - size, y - size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[4] = { XMFLOAT3{x + size, y + size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[5] = { XMFLOAT3{x - size, y + size, 0}, XMFLOAT4{Colors::Red} };

    supportSquareVertex[6] = { XMFLOAT3{oppositeX - size, oppositeY - size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[7] = { XMFLOAT3{oppositeX + size, oppositeY - size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[8] = { XMFLOAT3{oppositeX + size, oppositeY + size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[9] = { XMFLOAT3{oppositeX - size, oppositeY - size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[10] = { XMFLOAT3{oppositeX + size, oppositeY + size, 0}, XMFLOAT4{Colors::Red} };
    supportSquareVertex[11] = { XMFLOAT3{oppositeX - size, oppositeY + size, 0}, XMFLOAT4{Colors::Red} };
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
        engine->window->Size(1024, 1024);
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
