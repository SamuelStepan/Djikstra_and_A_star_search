#pragma once
#include <vector>
#include "Enums.h"
#include "olcPixelGameEngine.h"
#include <map>
#include <string>

using namespace std;

//Node of map/graph, it has it's position for vizualization, also holds it's edges to it's neighbors (different colors shall have different weights)
class Node {
public:
	//coordinates
	int row;
	int col;

	int distance = INT_MAX;
	int path_est = INT_MAX;

	bool same_edges;

	bool is_start_node = false;
	bool is_target_node = false;

	//weights of edges to neighbors
	Enums::Color left_w;
	Enums::Color right_w;
	Enums::Color top_w;
	Enums::Color bottom_w;

	
	map<Enums::Edge_dir, Node*> neighbors;	//map holding pointers to neighbors of the node in corresponding directions
	Node* parent = nullptr;	//pointer to nodes parent

	Node() {

	}

	Node(int row, int col, Enums::Color weight) {
		this->row = row;
		this->col = col;

		this->left_w = weight;
		this->right_w = weight;
		this->top_w = weight;
		this->bottom_w = weight;

		this->same_edges = true;
	}

	Node(int row, int col, Enums::Color left_w, Enums::Color right_w, Enums::Color top_w, Enums::Color bottom_w) {
		this->row = row;
		this->col = col;

		this->left_w = left_w;
		this->right_w = right_w;
		this->top_w = top_w;
		this->bottom_w = bottom_w;

		this->same_edges = false;
	}

	~Node() {

	}

	//returns color (for drawing) of given triangle (left, right, top or bottom)
	olc::Pixel get_color(Enums::Edge_dir dir) {
		switch (dir)
		{
		case Enums::Edge_dir::LEFT:
			return Enums::color_pixel(left_w);
			break;
		case Enums::Edge_dir::RIGHT:
			return Enums::color_pixel(right_w);
			break;
		case Enums::Edge_dir::TOP:
			return Enums::color_pixel(top_w);
			break;
		case Enums::Edge_dir::BOTTOM:
			return Enums::color_pixel(bottom_w);
			break;
		default:
			break;
		}
	}

	//returns weight(color) of the edge in oposite direction
	Enums::Color get_color_w(Enums::Edge_dir dir) {
		switch (dir)
		{
		case Enums::Edge_dir::LEFT:
			return right_w;
			break;
		case Enums::Edge_dir::RIGHT:
			return left_w;
			break;
		case Enums::Edge_dir::TOP:
			return bottom_w;
			break;
		case Enums::Edge_dir::BOTTOM:
			return top_w;
			break;
		default:
			break;
		}
	}

	void draw_self(olc::PixelGameEngine* pge, olc::vi2d map_start, int edge_length, bool write_dist, Enums::Border_nodes border_type = Enums::NORMAL_N, bool found_path = false, bool a_star = false) {
		olc::vi2d start = map_start + olc::vi2d({ col * edge_length,row * edge_length });	//top left corner of the node in respect to origin

		if (same_edges) {
			pge->FillRect(start, olc::vi2d({ edge_length,edge_length }), get_color(Enums::Edge_dir::TOP));	//it does not matter which direction we use
		}
		else {
			olc::vi2d center = start + olc::vi2d({ edge_length / 2,edge_length / 2 });	//center of the node with respect to origin

			vector<Enums::Edge_dir> dirs = { Enums::Edge_dir::LEFT, Enums::Edge_dir::RIGHT, Enums::Edge_dir::TOP, Enums::Edge_dir::BOTTOM };	//array of directions (for each direction we draw triangle starting from center to corners with respect to directions)

			//3d array, first index coresponds with direction for which we will want to draw triangle,
			//second index is for first point and second point of triangle (third is always the same)
			//third index is for x and y coordination of the point
			vector<vector<olc::vi2d>> triangle_points = { { {0, 0}, {0, edge_length} /*LEFT*/ },
														 { {edge_length, 0}, {edge_length, edge_length} /*RIGHT*/ },
														 { {0, 0}, {edge_length, 0} /*TOP*/ },
														 { {0, edge_length}, {edge_length, edge_length} /*BOTTOM*/ } };

			//draw all 4 triangles
			int i = 0;
			for(Enums::Edge_dir& dir : dirs){
				olc::vi2d point_1 = start + triangle_points.at(i).at(0);
				olc::vi2d point_2 = start + triangle_points.at(i).at(1);
				pge->FillTriangle(point_1, point_2, center, get_color(dir));
				i++;
			}
		}

		//lambda function that draws thick border of the node as 4 filled rectangles
		auto draw_thick_border = [&](olc::Pixel color_border) {
			vector<olc::vi2d> start_points = { start /*top border*/, start /*left border*/,
											   start + olc::vi2d({0, edge_length - 10}) /*right border*/, start + olc::vi2d({edge_length - 10, 0}) /*bottom border*/ };
			vector<olc::vi2d> sizes = { olc::vi2d({edge_length, 10}) /*top border*/, olc::vi2d({10, edge_length}) /*left border*/, 
										olc::vi2d({edge_length, 10}) /*right border*/, olc::vi2d({10, edge_length}) /*bottom border*/ };

			//draw thicker border as 4 rectangles
			for (int i = 0; i < 4; i++){
				pge->FillRect(start_points.at(i), sizes.at(i), color_border);
			}
		};

		//draws specific border
		switch (border_type)
		{
		case Enums::NORMAL_N:
			pge->DrawRect(start, olc::vi2d({ edge_length,edge_length }), olc::BLUE);		//draws thin border to distinguish different nodes
			break;
		case Enums::START_N:
			draw_thick_border(olc::DARK_GREEN);
			break;
		case Enums::TARGET_N:
			draw_thick_border(olc::DARK_MAGENTA);
			break;
		case Enums::IN_Q_N:
			draw_thick_border(olc::GREEN);
			break;
		case Enums::NEXT_NODE:
			draw_thick_border(olc::MAGENTA);
			break;
		case Enums::PATH_N:
			draw_thick_border(olc::DARK_BLUE);
			break;
		default:
			break;
		}

		//writes current distance to this node
		if (write_dist) {
			string text = distance == INT_MAX ? "inf" : " " + to_string(distance);
			string f_star;
			if (a_star && distance != INT_MAX && path_est != INT_MAX) {
				text = "g*=" + to_string(distance);
				f_star = "f*=" + to_string(path_est + distance);
			}
			if (is_start_node) text = " s";
			else if (is_target_node && !found_path) text = " t";
			if (a_star && distance != INT_MAX && path_est != INT_MAX) {
				pge->DrawString(start + olc::vi2d({ 5,20 }), text, olc::BLUE, 2U);
				pge->DrawString(start + olc::vi2d({ 5,60 }), f_star, olc::BLUE, 2U);
			}
			else {
				pge->DrawString(start + olc::vi2d({ 5,35 }), text, olc::BLUE, 3U);
			}
		}
	}
};