#pragma once
#include "olcPixelGameEngine.h"
#include "Enums.h"
#include <string>

using namespace std;

class GUI_btn{
public:
	//variables to hold information about position of the button
	olc::vi2d start_pos;
	olc::vi2d size;
	olc::vi2d end_pos;

	bool is_selected;

	string text;

	GUI_btn() {

	}

	GUI_btn(olc::vi2d start_pos, olc::vi2d size, string text, bool is_selected = false) {
		this->start_pos = start_pos;
		this->size = size;
		end_pos = start_pos + size;
		this->text = text;
		this->is_selected = is_selected;
	}

	//returns true if click_pos is inside the button
	bool handle_click(olc::vi2d click_pos) {
		return (click_pos.x >= start_pos.x && click_pos.y >= start_pos.y && click_pos.x <= end_pos.x && click_pos.y <= end_pos.y); //true if click_pos is inside the button
	}

	//draws the button to pixel game engine
	void draw_self(olc::PixelGameEngine* pge) {
		olc::Pixel color = is_selected ? olc::GREEN : olc::BLUE;
		pge->DrawRect(start_pos, size, color);
		pge->DrawString(start_pos + olc::vi2d(10, 14), text, color, 2U);
	}
};

