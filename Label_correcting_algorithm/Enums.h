#pragma once
#include <climits>
#include "olcPixelGameEngine.h"


namespace Enums {
	//enum representing weights of different edges(that are represented by different colors)
	enum Color
	{
		WHITE = 1,
		BLACK = INT_MAX,
		CYAN = 0,

		VERY_DARK_GREY = 100,
		DARK_GREY = 75,
		GREY = 50,

		VERY_DARK_RED = 25,
		DARK_RED = 15,
		RED = 5,

		VERY_DARK_YELLOW = 4,
		DARK_YELLOW = 3,
		YELLOW = 2,
	};

	//enum for directions
	enum Edge_dir {
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
	};

	//returns pixel's color of the given color (weight)
	olc::Pixel color_pixel(Enums::Color color) {
		switch (color)
		{
		case Enums::Color::WHITE:
			return olc::WHITE;
			break;
		case Enums::Color::BLACK:
			return olc::BLACK;
			break;
		case Enums::CYAN:
			return olc::CYAN;
			break;
		case Enums::Color::VERY_DARK_GREY:
			return olc::VERY_DARK_GREY;
			break;
		case Enums::Color::DARK_GREY:
			return olc::DARK_GREY;
			break;
		case Enums::Color::GREY:
			return olc::GREY;
			break;
		case Enums::Color::VERY_DARK_RED:
			return olc::VERY_DARK_RED;
			break;
		case Enums::Color::DARK_RED:
			return olc::DARK_RED;
			break;
		case Enums::Color::RED:
			return olc::RED;
			break;
		case Enums::Color::VERY_DARK_YELLOW:
			return olc::VERY_DARK_YELLOW;
			break;
		case Enums::Color::DARK_YELLOW:
			return olc::DARK_YELLOW;
			break;
		case Enums::Color::YELLOW:
			return olc::YELLOW;
			break;
		default:
			break;
		}
	}

	//enum for switching between different phases for GUI
	enum GUI_phase {
		CHANGE_NODES,	//phase in which user can change nodes on the map
		CHOOSE_S_T,	//phase in which user can choose start and finish node
		FIND_PATH,	//phase in which user cannot change map and algorithm is finding path
		FOUND_PATH,	//phase in which we found path and we are waiting for user to tell us what next
	};

	//enum for switching between different types of algortihm
	enum Alg_type {
		DJIKSTRA,
		A_STAR,
	};

	//enum for highlighting nodes
	enum Border_nodes {
		NORMAL_N,
		START_N,
		TARGET_N,
		IN_Q_N,
		NEXT_NODE,
		PATH_N,
	};
}

