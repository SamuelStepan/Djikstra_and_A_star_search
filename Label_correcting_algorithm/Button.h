#pragma once
#include "olcPixelGameEngine.h"
#include "Enums.h"
#include <string>

using namespace std;

//Button for palet, we click him to choose color (weight) for new node to put in map/graph
class Button{
public:
	olc::vi2d start_pos;
	int edge_size;
	olc::vi2d end_pos;
	//olc::Pixel color;
	//int weight;
	pair<olc::Pixel, Enums::Color> color_pair;	//pair of color of pixel and color weight

	Button() {

	}

	Button(olc::vi2d start_pos, int edge_size, olc::Pixel color, Enums::Color weight) {
		this->start_pos = start_pos;
		this->edge_size = edge_size;
		end_pos = start_pos + olc::vi2d({ edge_size,edge_size });
		color_pair = { color, weight };
	}

	~Button() {

	}

	void draw_self(olc::PixelGameEngine* pge, bool clicked = false) {
		olc::Pixel border_c = clicked ? olc::GREEN : olc::WHITE;	//color of border
		pge->FillRect(start_pos, olc::vi2d({ edge_size, edge_size }), color_pair.first);
		pge->DrawRect(start_pos, olc::vi2d({ edge_size, edge_size }), border_c);
		string text = color_pair.second != INT_MAX ? " = " + to_string(color_pair.second) : " = inf";
		pge->DrawString(start_pos + olc::vi2d({ edge_size + 5, 15 }), text, olc::WHITE, 2U);
	}

	bool is_clicked(olc::vi2d click_pos, pair<olc::Pixel, Enums::Color>& color_pair) {
		//check if clicked pos is on this button
		if (click_pos.x >= start_pos.x && click_pos.y >= start_pos.y && click_pos.x <= end_pos.x && click_pos.y <= end_pos.y) {
			color_pair = this->color_pair;
			return true;
		}
		else {
			return false;
		}
	}
};

