
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Node.h"
#include "Button.h"
#include "NewNode.h"
#include "GUI_btn.h"

using namespace std;

class Editor : public olc::PixelGameEngine
{
public:
	olc::vi2d screen_size = { 1200, 700 };

	//informations about the map of nodes
	olc::vi2d map_start = { 10,10 };
	int rows = 5;
	int cols = 8;
	int edge_length = 90;
	olc::vi2d map_end = map_start + olc::vi2d({ cols * edge_length, rows * edge_length });
	vector<vector<Node>> nodes;

	//this signals that there are none choosen starting node or end node
	olc::vi2d start_node = { -1,-1 };
	olc::vi2d target_node = { -1, -1 };

	//variables for working with palet of tools
	olc::vi2d palet_start = /*map_start + olc::vi2d({ cols * edge_length + 50, 0 })*/{ 810, 10 };
	olc::vi2d palet_end = palet_start + olc::vi2d( screen_size.x - palet_start.x - 50 , 300 );
	olc::vi2d palet_size = palet_end - palet_start;
	int btn_size = (palet_end.y - palet_start.y - 20) / 6;
	vector<Button> palet_btns;
	NewNode new_node;	//node to symbolise node made from palet, which user can use for changing map

	//variables for working with GUI buttons
	GUI_btn btn_change_map = GUI_btn(olc::vi2d(palet_start.x, screen_size.y - 230), olc::vi2d({ palet_size.x, 40 }), "CHANGE MAP", true);
	GUI_btn btn_choose_s_t = GUI_btn(olc::vi2d(palet_start.x, screen_size.y - 170), olc::vi2d({ palet_size.x, 40 }), "SELECT S & T NODE");
	GUI_btn btn_dijkstra = GUI_btn(olc::vi2d(palet_start.x, screen_size.y - 110), olc::vi2d({ palet_size.x, 40 }), "DIJKSTRA ALGORITHM", true);
	GUI_btn btn_a_star = GUI_btn(olc::vi2d(palet_start.x, screen_size.y - 50), olc::vi2d({ palet_size.x, 40 }), "A* ALGORITHM");

	//variables used to control gui
	Enums::GUI_phase gui_phase = Enums::CHANGE_NODES;
	Enums::Alg_type alg_type = Enums::DJIKSTRA;
	bool show_dist = false; //variable that will determine if nodes in map should show their distance towards start_node
	GUI_btn* selected_btn_gui = &btn_change_map;
	GUI_btn* selected_btn_alg = &btn_dijkstra;

	//variables used for finding the shortest path from start_node to target_node
	set<Node*> Q;
	set<Node*> path;
	int curr_min_path = INT_MAX;	//currently found minimal distance from start_node to target_node
	float curr_time = 0.0f;	//time since last tick
	float tick = 0.5f;	//time between ticks (for computing the shortest path)
	Node* next_node;
	Node* path_node;
	Node* start_node_obj;

	//heuristic using manhattan metric
	double heuristic(int i, int j) {
		return abs(i - target_node.x) + abs(j - target_node.y);	
	}

	Editor()
	{
		sAppName = "Editor";
	}

	//returns column and row of clicked node (it assumes that given position is in map)
	olc::vi2d get_node(olc::vi2d click_pos) {
		click_pos = click_pos - map_start;
		return olc::vi2d({ click_pos.x / edge_length, click_pos.y / edge_length });
	}

public:
	bool OnUserCreate() override
	{
		nodes.resize(cols);
		// initialize map with white nodes(edge weight are 1)
		for (int col = 0; col < cols; col++) {
			nodes.at(col).resize(rows);
			for (int row = 0; row < rows; row++) {
				nodes.at(col).at(row) = Node(row, col, Enums::WHITE);
			}
		}

		//create palet of buttons representing different type of nodes
		palet_btns.resize(12);
		vector<olc::Pixel> colors = { olc::CYAN, olc::DARK_RED, olc::WHITE, olc::VERY_DARK_RED, olc::YELLOW, olc::GREY,
									olc::DARK_YELLOW, olc::DARK_GREY, olc::VERY_DARK_YELLOW, olc::VERY_DARK_GREY, olc::RED, olc::BLACK };
		vector<Enums::Color> weights = { Enums::CYAN, Enums::DARK_RED, Enums::WHITE, Enums::VERY_DARK_RED, Enums::YELLOW, Enums::GREY,
									Enums::DARK_YELLOW, Enums::DARK_GREY, Enums::VERY_DARK_YELLOW, Enums::VERY_DARK_GREY, Enums::RED, Enums::BLACK };

		//create all buttons of palet
		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 2; j++){
				int shift = j == 0 ? 10 : (palet_end.x - palet_start.x) / 2;
				palet_btns.at(i * 2 + j) = Button(palet_start + olc::vi2d{ shift, 10 + i * 2 + btn_size * i }, btn_size - 6, colors.at(i * 2 + j), weights.at(i * 2 + j));
			}
		}

		//create new_node(if user is in change_map mode then if he click some node in map it changes into new_node)
		new_node = NewNode(palet_start + olc::vi2d({ (palet_end.x - palet_start.x) / 2 - 60, palet_end.y - palet_start.y + 20 }), 120);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//clears screen
		Clear(olc::BLACK);

		//draw all nodes
		for (int col = 0; col < cols; col++) {
			nodes.at(col).resize(rows);
			for (int row = 0; row < rows; row++) {
				nodes.at(col).at(row).draw_self(this, map_start, edge_length, show_dist,Enums::NORMAL_N, false, alg_type == Enums::A_STAR);
			}
		}

		//highlight nodes that are in Q and highlights next_node
		if (gui_phase == Enums::FIND_PATH) {
			for (auto node : Q) {
				node->draw_self(this, map_start, edge_length, show_dist, Enums::IN_Q_N, false, alg_type == Enums::A_STAR);
			}
			//draws very dark magenta border around next_node
			if (next_node != nullptr) {
				next_node->draw_self(this, map_start, edge_length, show_dist, Enums::NEXT_NODE, false, alg_type == Enums::A_STAR);
			}
		}

		//draws starting node or end node(if selected)
		if (start_node.x != -1 && start_node.y != -1) {
			nodes.at(start_node.x).at(start_node.y).draw_self(this, map_start, edge_length, show_dist, Enums::START_N);
		}
		if (target_node.x != -1 && target_node.y != -1) {
			nodes.at(target_node.x).at(target_node.y).draw_self(this, map_start, edge_length, show_dist, Enums::TARGET_N);
		}

		//draw all buttons from palet
		for (Button& btn : palet_btns){
			btn.draw_self(this);
		}

		DrawRect(palet_start, palet_end - palet_start);

		//draw new_node
		new_node.draw_self(this);

		//draw all gui buttons
		btn_change_map.draw_self(this);
		btn_choose_s_t.draw_self(this);
		btn_dijkstra.draw_self(this);
		btn_a_star.draw_self(this);

		//handle clicks
		if (GetMouse(0).bPressed && gui_phase != Enums::FIND_PATH) {
			olc::vi2d click_pos = { int(GetMouseX()), int(GetMouseY()) };

			//clicked position is in map
			if (click_pos.x >= map_start.x && click_pos.y >= map_start.y && click_pos.x <= map_end.x && click_pos.y <= map_end.y) {
				olc::vi2d click_node = get_node(click_pos);

				//player can choose start_node and end_node
				if (gui_phase == Enums::CHOOSE_S_T) {
					show_dist = true;

					//checks if we have already starting node or end node and if that is clicked then unchoose them,
					//if we have no starting node, then we set clicked node as starting node, 
					//if we have no end node and starting node, then we set clicked node as end node
					if (start_node == click_node) {
						start_node = olc::vi2d({ -1, -1 });
						nodes.at(click_node.x).at(click_node.y).is_start_node = false;
					}
					else if (start_node.x == -1 && start_node.y == -1 && target_node != click_node) {
						start_node = click_node;
						nodes.at(click_node.x).at(click_node.y).is_start_node = true;
					}
					else if (target_node == click_node) {
						target_node = olc::vi2d({ -1, -1 });
						nodes.at(click_node.x).at(click_node.y).is_target_node = false;
					}
					else if (target_node.x == -1 && target_node.y == -1) {
						target_node = click_node;
						nodes.at(click_node.x).at(click_node.y).is_target_node = true;
					}
				}
				else if (gui_phase == Enums::CHANGE_NODES) {	//player can change nodes on the map based on the new_node
					show_dist = false;

					nodes.at(click_node.x).at(click_node.y) = new_node.create_node(click_node.y, click_node.x);
				}
				
			}
			else {
				new_node.handle_click(click_pos);	//handles click on new_note

				//handles click on buttons from palet
				pair<olc::Pixel, Enums::Color> new_color;	
				for (auto& btn : palet_btns) {
					if (btn.is_clicked(click_pos, new_color)) {
						new_node.load_color(new_color.second);
					}
				}

				//handle clicks on gui_buttons
				if (btn_change_map.handle_click(click_pos) && gui_phase != Enums::CHANGE_NODES) {
					gui_phase = Enums::CHANGE_NODES;
					btn_change_map.is_selected = true;
					selected_btn_gui->is_selected = false;
					selected_btn_gui = &btn_change_map;
				}
				else if (btn_choose_s_t.handle_click(click_pos) && gui_phase != Enums::CHOOSE_S_T) {
					gui_phase = Enums::CHOOSE_S_T;
					btn_choose_s_t.is_selected = true;
					selected_btn_gui->is_selected = false;
					selected_btn_gui = &btn_choose_s_t;
				}
				else if (btn_dijkstra.handle_click(click_pos) && alg_type != Enums::DJIKSTRA) {
					alg_type = Enums::DJIKSTRA;
					btn_dijkstra.is_selected = true;
					selected_btn_alg->is_selected = false;
					selected_btn_alg = &btn_dijkstra;
				}
				else if (btn_a_star.handle_click(click_pos) && alg_type != Enums::A_STAR) {
					alg_type = Enums::A_STAR;
					btn_a_star.is_selected = true;
					selected_btn_alg->is_selected = false;
					selected_btn_alg = &btn_a_star;
				}
			}
		}

		//handles pressing enter, user input should be stoped and currently selected algorithm shall be used to find shortes path from start node to target node
		if (GetKey(olc::ENTER).bPressed && start_node != olc::vi2d({ -1,-1 }) && target_node != olc::vi2d({ -1,-1 })) {
			gui_phase = Enums::FIND_PATH;
			show_dist = true;

			//loads childrens(edges to neighbors) to each node and sets distance to start_node
			for (int i = 0; i < cols; i++){
				for (int j = 0; j < rows; j++){
					map<Enums::Edge_dir, Node*> neighbors;
					if (i != 0 && nodes.at(i - 1).at(j).right_w != INT_MAX) neighbors.insert({ Enums::LEFT, &nodes.at(i - 1).at(j) });	//left neighbor
					if (i != cols - 1 && nodes.at(i + 1).at(j).left_w != INT_MAX) neighbors.insert({ Enums::RIGHT, &nodes.at(i + 1).at(j) });	//right neigbor
					if (j != 0 && nodes.at(i).at(j - 1).bottom_w != INT_MAX) neighbors.insert({ Enums::TOP, &nodes.at(i).at(j - 1) });	//top neighbor
					if (j != rows - 1 && nodes.at(i).at(j + 1).top_w != INT_MAX) neighbors.insert({ Enums::BOTTOM, &nodes.at(i).at(j + 1) });	//bottom neighbor
					nodes.at(i).at(j).neighbors = neighbors;
					nodes.at(i).at(j).distance = (i == start_node.x && j == start_node.y) ? 0 : INT_MAX;	//if it´s not starting node, set it´s distance to starting node as infinity(INT_MAX)
					nodes.at(i).at(j).path_est = heuristic(i, j);	//calculating path estimate
					nodes.at(i).at(j).parent = nullptr;
				}
			}

			path = {};	//clears path
			Q = {};	//clears Q
			curr_min_path = INT_MAX;
			Q.insert(&nodes.at(start_node.x).at(start_node.y));	 //loads starting node to set Q
			next_node = nullptr;
		}

		//computes the algorithm for finding the minimal path from start_node to target_node
		if (gui_phase == Enums::FIND_PATH) {
			curr_time += fElapsedTime;
			if (curr_time >= tick) {	//we go through one node per one tick
				curr_time -= tick;

				//we choose next node randomly from the set Q
				if (alg_type == Enums::A_STAR) {
					set<Node*>::iterator it_min_dist_node;
					int min_f = INT_MAX;

					//finds iterator pointing to the node with minimal distance to start_node
					for (auto it = Q.begin(); it != Q.end(); ++it) {
						if ((*it)->distance + (*it)->path_est < min_f) {
							it_min_dist_node = it;
							min_f = (*it)->distance + (*it)->path_est;
						}
					}

					next_node = *it_min_dist_node;
					Q.erase(it_min_dist_node);	//removes next_node from the set Q
				}
				else if (alg_type == Enums::DJIKSTRA) {
					set<Node*>::iterator it_min_dist_node;
					int min_dist = INT_MAX;

					//finds iterator pointing to the node with minimal distance to start_node
					for (auto it = Q.begin(); it != Q.end(); ++it) {
						if ((*it)->distance < min_dist) {
							it_min_dist_node = it;
							min_dist = (*it)->distance;
						}
					}

					next_node = *it_min_dist_node;
					Q.erase(it_min_dist_node);	//removes next_node from the set Q
				}
				//searches all neighbors of next_node
				map<Enums::Edge_dir, Node*> neighbors = next_node->neighbors;
				for (auto it = neighbors.begin(); it != neighbors.end(); it++) {
					Enums::Edge_dir dir = it->first;
					Node* neighbor = it->second;
					int new_f_neighbor = alg_type == Enums::A_STAR ? next_node->distance + neighbor->get_color_w(dir) + neighbor->path_est : next_node->distance + neighbor->get_color_w(dir);
					int old_f_neighbor = alg_type == Enums::A_STAR ? neighbor->distance + neighbor->path_est : neighbor->distance;	
					if (old_f_neighbor < 0) old_f_neighbor = INT_MAX;	//if old_f_neighbor is lesser than 0 then overflowing happened
					//checks if distance from start_node to next_node + edge to given neighbor is lesser than distance from start_node to given neighbor (or u if it´ lesser)
					if (neighbor->get_color_w(dir) != INT_MAX && new_f_neighbor < min(old_f_neighbor, curr_min_path)) {
						neighbor->distance = next_node->distance + neighbor->get_color_w(dir);
						neighbor->parent = next_node;
						if (target_node != olc::vi2d({ neighbor->col, neighbor->row })) {
							Q.insert(neighbor);	//if neighbor is not already in the set Q, put it in Q
						}
						else {	//neigbor is target_node
							curr_min_path = new_f_neighbor;
						}
					}
				}
				if (Q.empty()) {
					if (nodes.at(target_node.x).at(target_node.y).parent != nullptr) {
						gui_phase = Enums::FOUND_PATH;
						path_node = nodes.at(target_node.x).at(target_node.y).parent;
						start_node_obj = &nodes.at(start_node.x).at(start_node.y);
					}
					else {
						gui_phase = selected_btn_gui == &btn_change_map ? Enums::CHANGE_NODES : Enums::CHOOSE_S_T;
					}
				}
			}
		}

		//vizualizes path from start_node to end_node
		if (gui_phase == Enums::FOUND_PATH) {
			curr_time += fElapsedTime;
			//for each tick we add one more node to path(if there are any)
			if (curr_time >= tick / 2.0f) {
				curr_time -= tick / 2.0f;
				if (!(path_node->col == start_node_obj->col && path_node->row == start_node_obj->row)) {
					path.insert(path_node);
					path_node = path_node->parent;
				}
			}

			//draws path nodes
			for (auto node : path) {
				node->draw_self(this, map_start, edge_length, show_dist, Enums::PATH_N);
			}
			//updated target_node
			nodes.at(target_node.x).at(target_node.y).draw_self(this, map_start, edge_length, show_dist, Enums::TARGET_N, true);
		}

		return true;
	}
};

int main()
{
	Editor graph_editor;	
	if (graph_editor.Construct(graph_editor.screen_size.x, graph_editor.screen_size.y, 1, 1))
		graph_editor.Start();
	return 0;
}