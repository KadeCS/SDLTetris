#pragma once
#include "Includes.h"
#include "Globals.h"

struct TetrominoPiece {
	SDL_FRect rect;
	Globals::color color;
	SDL_FRect og;
};

class Tetromino {
public:
	std::vector<TetrominoPiece> pieces;
	SDL_FRect rect;
	int rotateIndex;
	int constructIndex; // basically the type of the tetromino in the "tetrominos" int[] in ConsoleApplication1.cpp
	SDL_FRect centerPoint;

	void addPiece(int lane, int col)
	{
		TetrominoPiece piece;
		piece.rect.x = 0 + (16 * col);
		// temp until I add colors
		Globals::color c;
		c.r = 255;
		c.g = 255;
		c.b = 255;
		c.a = 255;
		piece.color = c;
		piece.rect.y = 16 * lane;
		piece.rect.w = 16;
		piece.rect.h = 16;

		if (piece.rect.x > rect.w)
			rect.w = piece.rect.x;
		if (piece.rect.y > rect.h)
			rect.h = piece.rect.y;

		SDL_FRect copyRect = piece.rect;

		piece.og = copyRect;

		pieces.push_back(piece);

	}

	void movePiece(TetrominoPiece& piece, int directionX, int directionY)
	{
		SDL_FRect copyOfOriginal = piece.og;
		piece.rect.x = copyOfOriginal.x + (directionX * 16);
		piece.rect.y = copyOfOriginal.y + (directionY * 16);
	}

	void rotate()
	{
		rotateIndex = (rotateIndex + 1) % 4;
		switch (constructIndex)
		{
		case 0:
			switch (rotateIndex)
			{
			case 0:
				movePiece(pieces[0], 2, -3);
				movePiece(pieces[1], 1, -2);
				movePiece(pieces[2], 0, 0);
				movePiece(pieces[3], -1, -1);
				break;
			case 1:
				movePiece(pieces[0], 0, 0);
				movePiece(pieces[1], 0, 0);
				movePiece(pieces[2], 0, 0);
				movePiece(pieces[3], 0, 0);
				break;
			case 2:
				movePiece(pieces[0], 1, -3);
				movePiece(pieces[1], 0, -2);
				movePiece(pieces[2], -1, -1);
				movePiece(pieces[3], -2, 0);
				break;
			case 3:
				movePiece(pieces[0], 0, -1);
				movePiece(pieces[1], 0, -1);
				movePiece(pieces[2], 0, -1);
				movePiece(pieces[3], 0, -1);
				break;
			}
			break;
		case 1:
			switch (rotateIndex)
			{
			case 0:
				movePiece(pieces[0], 0, 0);
				movePiece(pieces[1], 0, 0);
				movePiece(pieces[2], 0, 0);
				movePiece(pieces[3], 0, 0);
				break;
			case 1:
				movePiece(pieces[0], 2, 0);
				movePiece(pieces[1], 1, -1);
				movePiece(pieces[2], 0, 0);
				movePiece(pieces[3], -1, 1);
				break;
			case 2:
				movePiece(pieces[0], 2, 2);
				movePiece(pieces[1], 2, 0);
				movePiece(pieces[2], 0, 0);
				movePiece(pieces[3], -2, 0);
				break;
			case 3:
				movePiece(pieces[0], 0, 2);
				movePiece(pieces[1], 1, 1);
				movePiece(pieces[2], 0, 0);
				movePiece(pieces[3], -1, -1);
				break;
			}
			break;
		}
	}

	void draw()
	{
		if (pieces.size() == 0)
			return;

		for (int i = 0; i < pieces.size(); i++)
		{
			std::vector<TetrominoPiece>& lane = pieces;
			SDL_SetRenderDrawColor(Globals::renderer, lane[i].color.r, lane[i].color.g, lane[i].color.b, lane[i].color.a);
			SDL_FRect newRect;
			newRect.x = rect.x + lane[i].rect.x;
			newRect.y = rect.y + lane[i].rect.y;
			newRect.w = lane[i].rect.w;
			newRect.h = lane[i].rect.h;
			SDL_RenderFillRectF(Globals::renderer, &newRect);
			SDL_SetRenderDrawColor(Globals::renderer, 0, 0, 0, lane[i].color.a);
			SDL_RenderDrawRectF(Globals::renderer, &newRect);
		}
	}
};