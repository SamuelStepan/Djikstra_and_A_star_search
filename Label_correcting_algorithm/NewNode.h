#pragma once
#include "olcPixelGameEngine.h"
#include "Enums.h"
#include "Node.h"
#include <vector>
#include <set>
#include <map>

using namespace std;


//there shall be only one obj, it shall be used to vizualize currently made node (from palet) which can be used to change map
class NewNode{
public:
	olc::vi2d start_pos;	//starting position
	int edge_length;	
	olc::vi2d end_pos;
	olc::vi2d center_pos;	

	//weights of edges to neighbors
	Enums::Color left_w;
	Enums::Color right_w;
	Enums::Color top_w;
	Enums::Color bottom_w;

	//variables coresponding to triangles
	set<Enums::Edge_dir> selected_tri;	//selected triangles
	vector<vector<olc::vi2d>> triangles;	//vector holding each triangle (represented by 3 points)
	vector<Enums::Edge_dir> dirs = { Enums::Edge_dir::TOP, Enums::Edge_dir::LEFT, Enums::Edge_dir::RIGHT, Enums::Edge_dir::BOTTOM };	//array holding all directions
	map<Enums::Edge_dir, vector<olc::vi2d>> dir_triangles;	//map between direction and corresponding triangle

	NewNode() {

	}

	NewNode(olc::vi2d pos, int edge_length) {
		this->start_pos = pos;
		this->edge_length = edge_length;
		end_pos = start_pos + olc::vi2d({ edge_length, edge_length });
		center_pos = start_pos + (end_pos - start_pos) / 2;

		left_w = Enums::BLACK;
		right_w = Enums::BLACK;
		top_w = Enums::BLACK;
		bottom_w = Enums::BLACK;

		selected_tri = { Enums::LEFT, Enums::RIGHT, Enums::TOP, Enums::BOTTOM };
		center_pos = start_pos + olc::vi2d({ edge_length / 2,edge_length / 2 });	//center of the node with respect to origin
		vector<olc::vi2d> corners = { start_pos, start_pos + olc::vi2d({edge_length, 0}), start_pos + olc::vi2d({0,edge_length}), end_pos };
		triangles = { {corners.at(0), corners.at(1), center_pos}, {corners.at(0), corners.at(2), center_pos},
					  {corners.at(3), corners.at(1), center_pos}, {corners.at(3), corners.at(2), center_pos} };

		//asigns for every direction in dirs coresponding triangle
		for (int i = 0; i < 4; i++){
			dir_triangles.insert({ dirs.at(i), triangles.at(i) });
		}
	}

	~NewNode() {

	}

	//returns variable holding color (weight) for given direction
	Enums::Color* get_dir_var(Enums::Edge_dir dir) {
		switch (dir)
		{
		case Enums::Edge_dir::LEFT:
			return &left_w;
			break;
		case Enums::Edge_dir::RIGHT:
			return &right_w;
			break;
		case Enums::Edge_dir::TOP:
			return &top_w;
			break;
		case Enums::Edge_dir::BOTTOM:
			return &bottom_w;
			break;
		default:
			break;
		}
	}

	//returns color (for drawing) of given edge(left, right, top or bottom)
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

	//void choose_dir(set<Enums::Edge_dir> dir) {
	//	choosen_dir = dir;
	//}

	//function that sets given color to selected triangles
	void load_color(Enums::Color color) {
		for (Enums::Edge_dir dir : selected_tri) {
			*get_dir_var(dir) = color;	
		}
	}

	//function that draws itself to pixel game engine
	void draw_self(olc::PixelGameEngine *pge) {

		//draw all 4 triangles
		map<Enums::Edge_dir, vector<olc::vi2d>>::iterator it;
		for (it = dir_triangles.begin(); it != dir_triangles.end(); it++){
			vector<olc::vi2d> tri = it->second;	//current triangle
			pge->FillTriangle(tri.at(0), tri.at(1), tri.at(2), get_color(it->first));
		}

		//draw border of the node
		pge->DrawRect(start_pos, olc::vi2d({ edge_length, edge_length }), olc::BLUE);

		//draw border of currently choosen directions (triangles)
		for (auto& dir : selected_tri) {
			vector<olc::vi2d> tri = dir_triangles.find(dir)->second;	//current triangle
			pge->DrawTriangle(tri.at(0), tri.at(1), tri.at(2), olc::GREEN);
		}
	}

	//function that returns area of given triangle (triangle is defined by 3 points)
	float tri_area(olc::vi2d point_A, olc::vi2d point_B, olc::vi2d point_C) {
		olc::vi2d AB = point_B - point_A;
		olc::vi2d AC = point_C - point_A;
		return float(abs(AB.cross(AC))) / 2.0f;
	}

	//function that returns true if given point lies in given triangle
	bool in_tri(olc::vi2d test_point, olc::vi2d point_A, olc::vi2d point_B, olc::vi2d point_C) {
		float sum_area = 0.0f;
		vector<vector<olc::vi2d>> triangles = { {test_point, point_A, point_B}, {test_point, point_A, point_C}, {test_point, point_C, point_B} };
		for (vector<olc::vi2d>& triangle : triangles) {
			sum_area += tri_area(triangle.at(0), triangle.at(1), triangle.at(2));
		}
		return sum_area == tri_area(point_A, point_B, point_C);	//returns true if summary of area of 3 smaller triangles (created by substituting one point with testing point) is the same as area of given triangle
	}

	//function that handles clicks on new_note
	void handle_click(olc::vi2d click_pos) {
		//clicked position is inside new_node
		if (click_pos.x >= start_pos.x && click_pos.y >= start_pos.y && click_pos.x <= end_pos.x && click_pos.y <= end_pos.y) {
			
			vector<Enums::Edge_dir> dirs = { Enums::TOP, Enums::LEFT, Enums::RIGHT, Enums::BOTTOM };	//directions of triangles

			//searches all triangles and if some was clicked then sele
			int i = 0;
			for (vector<olc::vi2d>& triangle : triangles) {
				if (in_tri(click_pos, triangle.at(0), triangle.at(1), triangle.at(2))) {
					set<Enums::Edge_dir>::iterator it = selected_tri.find(dirs.at(i));	//gets iterator position if direction is in the set, otherwise returns pointer to end position
					if (it != selected_tri.end()) {
						selected_tri.erase(it);	//if we had already selected this triangle, we unselect it
					}
					else {
						selected_tri.insert(dirs.at(i));	//we select given triangle
					}
				}
				i++;
			}
		}
	}

	//function that returns new node, it shall be called after user clicked on some node (in map) to replace it with new_node
	Node create_node(int row, int col) {
		return Node(row, col, left_w, right_w, top_w, bottom_w);
	}
};
