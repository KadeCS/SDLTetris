#pragma once
#include "Includes.h"
#include "Globals.h"

struct TetrominoPiece {
	SDL_FRect rect;
	Globals::color color;
};

class Tetromino {
public:
	std::vector<TetrominoPiece> pieces[2];
	SDL_FRect rect;
	int constructIndex; // basically the type of the tetromino in the "tetrominos" int[] in ConsoleApplication1.cpp

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
		pieces[lane].push_back(piece);
	}

	void draw()
	{
		// basically draw all of the pieces, then convert them to a texture, and then draw that texture with the correct angles and stuff
		int height = pieces[1].size() != 0 ? 32 : 16; // if the bottom lane has any pieces, make the height 32.
		int width = pieces[1].size() > pieces[0].size() ? 16 + (16 * pieces[1].size()) : 16 + (16 * pieces[0].size()); // the the bottom lane is longer than the top, take its width instead of the top one.

		SDL_Texture* t = SDL_CreateTexture(Globals::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
		
		SDL_SetRenderTarget(Globals::renderer, t);
		for (int l = 0; l < 2; l++)
		{
			for (int i = 0; i < pieces[l].size(); i++)
			{
				std::vector<TetrominoPiece>& lane = pieces[l];
				SDL_SetRenderDrawColor(Globals::renderer, lane[i].color.r, lane[i].color.g, lane[i].color.b, lane[i].color.a);
				SDL_RenderFillRectF(Globals::renderer, &lane[i].rect);
				SDL_SetRenderDrawColor(Globals::renderer, 0, 0, 0, lane[i].color.a);
				SDL_RenderDrawRectF(Globals::renderer, &lane[i].rect);
			}
		}
		SDL_SetRenderTarget(Globals::renderer, NULL);

		// now get the actual rect of the texture, and draw the texture to there.
		rect.h = height;
		rect.w = width;

		SDL_RenderCopyF(Globals::renderer, t, NULL, &rect);
	}
};