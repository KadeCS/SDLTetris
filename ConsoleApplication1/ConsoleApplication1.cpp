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

// [level][0] = speed for that level

const float cellFrames[16][1] = { // gotten from https://gamedev.stackexchange.com/questions/159835/understanding-tetris-speed-curve
	{0.01667},
	{0.021017},
	{0.026977},
	{0.035256},
	{0.04693},
	{0.06361},
	{0.0879},
	{0.1236},
	{0.1775},
	{0.2598},
	{0.388},
	{0.59},
	{0.92},
	{1.46},
	{2.36},
};

float deltaTime = 0;

std::vector<TetrominoPiece> groundedPieces[21];

Tetromino* faillingTetr;

bool isOffseting = false;

int index = 0;
int selectedPieceIndex = 0;

std::vector<int> offsetX;
std::vector<int> offsetY;

void createTetr(int constructId) {
	Tetromino* tetr = new Tetromino();

	tetr->constructIndex = constructId;

	int colIndex = 0;

	for (int ii = 0; ii < 4; ii++)
	{
		int piece = tetrominos[constructId][ii];
		int bottomPiece = tetrominos[constructId][ii + 4];
		if (piece != 0)
			tetr->addPiece(0, colIndex);
		if (bottomPiece != 0)
			tetr->addPiece(1, colIndex);

		if (piece != 0 || bottomPiece != 0)
			colIndex++;

	}
	tetr->rect.x = (800 / 2) - 24;
	tetr->rect.y = 192;
	tetr->storeY = 192;
	faillingTetr = tetr;
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

float currentTime = 0;
float simulatedTime = 0;

int level;

float align(float value, float size)
{
	return value - std::abs(std::fmod(value, size));
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

	createTetr(2);
	
	while (run)
	{
		const Uint32 startTime = SDL_GetTicks();
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
					if (faillingTetr)
						faillingTetr->rotate();
					break;
				case SDLK_LEFT:
					if (faillingTetr)
					{
						faillingTetr->rect.x -= 16;
						if (faillingTetr->rect.x < ((800 / 2) - 80))
							faillingTetr->rect.x = ((800 / 2) - 80);
					}
					break;
				case SDLK_RIGHT:
					if (faillingTetr)
					{
						faillingTetr->rect.x += 16;
						if (faillingTetr->rect.x + faillingTetr->rect.w >= ((800 / 2) - 80) + 160)
							faillingTetr->rect.x -= 16;
					}
				}
				break;
			}

		}

		currentTime += deltaTime;


		if (faillingTetr)
		{
			while (currentTime > simulatedTime)
			{
				faillingTetr->storeY++;
				faillingTetr->rect.y = align(faillingTetr->storeY, 16);

				simulatedTime += 1 / cellFrames[level][0];
			}

			// collision

			bool die = false;

			for (int i = 0; i < 21; i++)
			{
				for (TetrominoPiece& piece : groundedPieces[i])
				{
					if (faillingTetr->checkCol(piece))
					{
						for (TetrominoPiece piece : faillingTetr->pieces)
						{
							piece.rect.x = faillingTetr->rect.x + piece.rect.x;
							piece.rect.y = faillingTetr->rect.y + piece.rect.y;

							int lane = piece.rect.y - 192;
							for (int i = 0; i < 21; i++)
							{
								int laneMay = i * 16;
								if (lane == laneMay)
									groundedPieces[i].push_back(piece);
							}
						}
						die = true;
					}
				}
			}

			if (faillingTetr->rect.y + faillingTetr->rect.h >= 480)
			{
				faillingTetr->rect.y = 480;
				for (TetrominoPiece piece : faillingTetr->pieces)
				{
					piece.rect.x = faillingTetr->rect.x + piece.rect.x;
					piece.rect.y = faillingTetr->rect.y + piece.rect.y;

					int lane = piece.rect.y - 192;
					for (int i = 0; i < 21; i++)
					{
						int laneMay = i * 16;
						if (lane == laneMay)
							groundedPieces[i].push_back(piece);
					}
				}
				die = true;

			}

			if (die)
			{
				delete faillingTetr;
				faillingTetr = nullptr;
			}

			if (faillingTetr)
				faillingTetr->draw();

		}

		for (int i = 0; i < 21; i++)
		{
			for (TetrominoPiece& piece : groundedPieces[i])
			{
				SDL_SetRenderDrawColor(Globals::renderer, piece.color.r, piece.color.g, piece.color.b, piece.color.a);
				SDL_FRect newRect;
				newRect.x = piece.rect.x;
				newRect.y = piece.rect.y;
				newRect.w = piece.rect.w;
				newRect.h = piece.rect.h;
				SDL_RenderFillRectF(Globals::renderer, &newRect);
				SDL_SetRenderDrawColor(Globals::renderer, 0, 0, 0, 255);
				SDL_RenderDrawRectF(Globals::renderer, &newRect);
			}
		}
		// render box

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		SDL_FPoint p[4];

		p[0].x = ((800 / 2) - 80);
		p[0].y = 192;
		p[1].x = ((800 / 2) - 80);
		p[1].y = 512;
		p[2].x = ((800 / 2) - 80) + 160;
		p[2].y = 512;
		p[3].x = ((800 / 2) - 80) + 160;
		p[3].y = 192;

		SDL_RenderDrawLinesF(renderer, p, 4);


		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderPresent(renderer);

		deltaTime = SDL_GetTicks() - startTime;

	}

	return 0;

}
