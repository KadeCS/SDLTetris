#include "Includes.h"
#include "Tetromino.h"
#include "Globals.h"
#pragma comment(lib,"x64\\SDL2.lib")
#pragma comment(lib,"x64\\SDL2_ttf.lib")

// the pieces stored in ints
// 0 = blank space
// 1 = tile

const int tetrominos[7][8] = { {
	{1},{1},{1},{1}, // 4 Wide
	{0},{0},{0},{0}
	},
	{
	{1},{0},{0},{0}, // L-Piece
	{1},{1},{1},{0}
	},
	{
	{0},{0},{1},{0}, // Reverse L-Piece
	{1},{1},{1},{0}
	},
	{
	{1},{1},{0},{0}, // Square
	{1},{1},{0},{0}
	},
	{
	{0},{1},{1},{0}, // Reverse S
	{1},{1},{0},{0}
	},
	{
	{0},{1},{0},{0}, // T-Piece
	{1},{1},{1},{0}
	},
	{
	{1},{1},{0},{0}, // S
	{0},{1},{1},{0}
	}
};

std::vector<Tetromino*> avalableTetrominos;

bool isOffseting = false;

int index = 0;
int selectedPieceIndex = 0;

std::vector<int> offsetX;
std::vector<int> offsetY;

void setTerio() {
	avalableTetrominos.clear();
	for (int i = 0; i < 8; i++)
	{
		Tetromino* tetr = new Tetromino();

		tetr->constructIndex = i;

		int colIndex = 0;

		for (int ii = 0; ii < 4; ii++)
		{
			int piece = tetrominos[i][ii];
			int bottomPiece = tetrominos[i][ii + 4];
			if (piece != 0)
				tetr->addPiece(0, colIndex);
			if (bottomPiece != 0)
				tetr->addPiece(1, colIndex);

			if (piece != 0 || bottomPiece != 0)
				colIndex++;

		}
		tetr->rect.x = 24 + (76 * i);
		tetr->rect.y = 400;
		avalableTetrominos.push_back(tetr);

	}
}

void switchOffset() {
	offsetX.clear();
	offsetY.clear();

	selectedPieceIndex = 0;

	for (int i = 0; i < avalableTetrominos[index]->pieces.size(); i++)
	{
		offsetX.push_back(0);
		offsetY.push_back(0);
	}
}

void toClipboard(const std::string& s) {
	OpenClipboard(0);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, s.size());
	if (!hg) {
		CloseClipboard();
		return;
	}
	memcpy(GlobalLock(hg), s.c_str(), s.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
}

int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	PSTR lpCmdLine,
	INT nCmdShow)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");


	SDL_Window* window = SDL_CreateWindow("Da window", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	Globals::renderer = renderer;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	bool run = true;

	setTerio();

	switchOffset();

	while (run)
	{
		SDL_RenderClear(renderer);
		SDL_Event event;
		while (SDL_PollEvent(&event) > 0)
		{
			switch (event.type) {
			case SDL_QUIT:
				run = false;
				break;
			case SDL_KEYDOWN:
				std::string clip = "";
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:
					if (!isOffseting)
					{
						for (int i = 0; i < avalableTetrominos.size(); i++)
						{
							Tetromino* piece = avalableTetrominos[i];
							piece->rotate();
						}
					}
					else
					{
						offsetY[selectedPieceIndex]--;
					}
					break;
				case SDLK_DOWN:
					if (isOffseting)
						offsetY[selectedPieceIndex]++;
					break;
				case SDLK_LEFT:
					if (isOffseting)
						offsetX[selectedPieceIndex]--;
					break;
				case SDLK_RIGHT:
					if (isOffseting)
						offsetX[selectedPieceIndex]++;
					break;
				case SDLK_r:
					switchOffset();
					for (int i = 0; i < avalableTetrominos[index]->pieces.size(); i++)
					{
						avalableTetrominos[index]->pieces[i].rect.x = avalableTetrominos[index]->pieces[i].og.x;
						avalableTetrominos[index]->pieces[i].rect.y = avalableTetrominos[index]->pieces[i].og.y;
					}
					break;
				case SDLK_l:
					selectedPieceIndex++;
					if (selectedPieceIndex > avalableTetrominos[index]->pieces.size() - 1)
						selectedPieceIndex = 0;
					break;
				case SDLK_s:
					for (int i = 0; i < offsetX.size(); i++)
					{
						clip += "movePiece(pieces[" + std::to_string(i) + "], " + std::to_string(offsetX[i]) + "," + std::to_string(offsetY[i]) + ");\n";
					}
					toClipboard(clip);
					break;
				case SDLK_p:
					switchOffset();
					index++;

					if (index >= avalableTetrominos.size() - 1)
						index = 0;
					break;
				}
				break;
			}

		}


		// test render
		avalableTetrominos[index]->draw();

		if (isOffseting)
		{

			avalableTetrominos[index]->movePiece(avalableTetrominos[index]->pieces[selectedPieceIndex], offsetX[selectedPieceIndex], offsetY[selectedPieceIndex]);


			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

			SDL_FRect box = avalableTetrominos[index]->pieces[selectedPieceIndex].rect;

			box.x += avalableTetrominos[index]->rect.x;
			box.y += avalableTetrominos[index]->rect.y;

			SDL_RenderFillRectF(renderer, &box);
		}
		// render box

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_FPoint p[4];

		p[0].x = 300;
		p[0].y = 100;
		p[1].x = 300;
		p[1].y = 450;
		p[2].x = 500;
		p[2].y = 450;
		p[3].x = 500;
		p[3].y = 100;

		SDL_RenderDrawLinesF(renderer, p, 4);


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderPresent(renderer);

	}

	return 0;

}
