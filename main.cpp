#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cmath>
#include <GLUT/glut.h>

#include "required.h"

using namespace std;

/* Global vars */

// Window Parameters
int win_width = 800;
int win_height = 640;
int window_id;

// Drawing modes
bool line_drawing_mode = true;
bool fill_mode = false;
bool polygon_drawing_mode = false;

int num_points = 0;
point_t first_point;
list<point_t> polygon_points;
list<point_t> fill_points;

color_t black(0, 0, 0);
pen_t pen(black, 2, false);
fill_t fill_object(black, black, false);

canvas_t* canvas = NULL;

/* Function declarations */
void make_new_canvas();
void save_drawing();
void load_drawing();
color_t read_rgb();

/* Callbacks */
void display(void);
void reshape(int, int);
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);

/* Functions */
void make_new_canvas() {
	if (!canvas) {
		color_t bg_c = read_rgb();
		//color_t bgcd(255,255,255);
		canvas = new canvas_t(win_width, win_height, bg_c, NULL);

		int r = bg_c.red;
		int g = bg_c.green;
		int b = bg_c.blue;

		glClearColor(r/255.0, g/255.0, b/255.0, 0);
		glClear(GL_COLOR_BUFFER_BIT);

		glutPostRedisplay();
		cout << "Initialized new canvas\n";
	}
	else {
		cerr << "A canvas already exists\n";
	}
}

// Save the drawing object pointed to by d
// Assumes that d is a valid drawing, so it
// does not error handle for the case when d
// is NULL
void save_drawing(drawing_t* d) {
	cout << "Enter filename to save to (no spaces): ";
	string filename; cin >> filename;
	// add code to check for invalid filename characters like '/'
	filename = "drawings/" + filename;
	ofstream draw_file;
	draw_file.open(filename.c_str());

	// Save the lines
	list<line_t>::iterator l_itr;
	for (l_itr = d->lines.begin(); l_itr != d->lines.end(); l_itr++) {
		draw_file << "L "
		          << l_itr->v0.x << " "
		          << l_itr->v0.y << " "
		          << l_itr->v1.x << " "
		          << l_itr->v1.y << " "
		          << l_itr->pen.size << " "
		          << (int)l_itr->pen.color.red << " "
		          << (int)l_itr->pen.color.green << " "
		          << (int)l_itr->pen.color.blue << "\n";
	}

	// Save the polygons
	list<polygon_t>::iterator pgn_itr;
	for (pgn_itr = d->polygons.begin() ; pgn_itr != d->polygons.end() ; pgn_itr++) {
		cout << "polygon\n";
		draw_file << "P ";
		// Print each point to the file
		list<point_t>::iterator itr;
		for (itr = pgn_itr->vertices.begin() ; itr != pgn_itr->vertices.end() ; itr++) {
			draw_file << itr->x << " "
			          << itr->y << " ";
		}
		draw_file << pgn_itr->border.size << " "
		          << (int)pgn_itr->border.color.red << " "
		          << (int)pgn_itr->border.color.green << " "
		          << (int)pgn_itr->border.color.blue << "\n";
	}

	draw_file.close();

	cout << "Saved to file '" << filename << "'\n";
}

// Load a drawing object into d
// When passed to the function, d is NULL
void load_drawing(canvas_t* canvas) {
	cout << "Enter drawing file to load: ./drawings/";
	string filename; cin >> filename;
	filename = "drawings/" + filename;

	canvas->drawing = new drawing_t(canvas);

	ifstream draw_file;
	string line;
	// draw_file.open(filename);
	// if (draw_file.is_open()) {
	// 	while (draw_file.good()) {
	// 		if (line[0] == 'L') {

	// 		}
	// 		else if (line[0] == 'P') {
				
	// 		}
	// 	}
	// }

	// draw_file.close();

	cout << "Loaded drawing from file '" << filename << "'\n";
}

color_t read_rgb() {
	ifstream canvas_file("config/canvas.cfg");
	string line;
	int r, g, b;
	if (canvas_file.is_open()) {
		while (canvas_file.good()) {
			getline(canvas_file, line);
			if (line[0] == 'R') r = atoi(line.substr(2).c_str());
			else if (line[0] == 'G') g = atoi(line.substr(2).c_str());
			else if (line[0] == 'B') b = atoi(line.substr(2).c_str());
		}
	}
	color_t bg_c(r, g, b);
	return bg_c;
}

/* Callbacks */
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	/* Test code here */
	glFlush();
}

void reshape(int w, int h) {
	if (h == 0) h = 1;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1., 1.);
	glViewport(0, 0, w, h);

	win_width = w;
	win_height = h;

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
		// Create new canvas
		case 'n': { make_new_canvas(); }
		break;

		// Initialize new drawing
		case 'd': {
			if (!canvas) {
				cerr << "ERROR: No canvas to draw on. Make a new canvas by pressing 'n'\n";
			}
			else {
				if (canvas->drawing) {
					canvas->drawing = NULL;
				}
				canvas->drawing = new drawing_t(canvas);
				cout << "Initialized new drawing\n";
			}
		}
		break;

		// Save drawing
		case 's': {
			if (canvas and canvas->drawing) {
				save_drawing(canvas->drawing);
			}
			else {
				cerr << "ERROR: No canvas exists or no drawing has been initialized\n";
			}
		}
		break;

		// Load drawing
		case 'l': {
			if (canvas) {
				if (canvas->drawing) {
					cout << "Destroying current drawing... ";
					canvas->drawing = NULL;
				}
				load_drawing(canvas);
			}
			else {
				cerr << "ERROR: No canvas to load to. Make a new canvas by pressing 'n'\n";
			}
		}
		break;

		//Toggle fill mode
		case 'f': {
			cout << "Fill mode is now " << (fill_mode ? "off" : "on") << "\n";
			fill_mode = not fill_mode;
			if(fill_mode) {
				polygon_drawing_mode = false;
				line_drawing_mode = false;
			}
			else {
				line_drawing_mode = true;
				polygon_drawing_mode = false;
			}

			if (fill_mode) {
				canvas->drawing->draw_array();
			}
			else {
				canvas->drawing->draw();
			}
			glFlush();
		}
		break;

		// Toggle line drawing mode
		case '1': {
			if (canvas) {
				if (canvas->drawing) {
					// Draw the polygon, then clear polygon_points
					polygon_t poly(polygon_points, pen);
					canvas->drawing->polygons.push_back(poly);
					poly.draw(canvas->array, canvas->width, canvas->height);
					polygon_points.clear();
					glFlush();
				}
				
				line_drawing_mode = true;
				cout << "Line drawing mode on\n";
			}
		}
		break;

		// Toggle polygon drawing mode
		case '2': {
			if (canvas) {
				line_drawing_mode = false;
				polygon_drawing_mode = true;
				fill_mode = false;
				cout << "Polygon drawing mode on\n";	
			}
		}
		break;

		// Exit the program using Esc
		case 27: {
			cout << "Exiting myDraw...\n";
			glutDestroyWindow(window_id);
			exit(0);
		}
		break;

		// Toggle fill mode or pen attributes
		// depending on whether fill mode is
		// active or not
		case 'c': {
			if (fill_mode) {
				cout << "Enter new fill attributes\n";
				cout<<"Mode (Checker = 0 or normal = 1): "<<endl;
				int t;
				cin>>t;
				if(t==1){
					int r; cout << "R: "; cin >> r;
					int g; cout << "G: "; cin >> g;
					int b; cout << "B: "; cin >> b;
					color_t fill_c(r, g, b);
					fill_object.color_1 = fill_c;
					fill_object.checker_mode = false;
					cout << "Fill attributes modified\n";
				}
				else if(t==0){
					cout<<"First color"<<endl;
					int r; cout << "R: "; cin >> r;
					int g; cout << "G: "; cin >> g;
					int b; cout << "B: "; cin >> b;
					color_t fill_c(r, g, b);
					fill_object.color_1 = fill_c;
					cout<<"Second color"<<endl;
					cout << "R: "; cin >> r;
					cout << "G: "; cin >> g;
					cout << "B: "; cin >> b;
					color_t fill_b(r,g,b);
					fill_object.color_2 = fill_b;
					fill_object.checker_mode = true;
					cout << "Fill attributes modified\n";
				}
			}
			else {
				cout << "Enter new pen attributes\n";
				int r; cout << "R: "; cin >> r;
				int g; cout << "G: "; cin >> g;
				int b; cout << "B: "; cin >> b;
				color_t pen_c(r, g, b);
				float s; cout << "Size: "; cin >> s;
				pen_t new_pen(pen_c, s, false);
				pen = new_pen;
				cout << "Pen attributes modified\n";
			}
		}

	}
}

void mouse(int button, int state, int x, int y) {
	if (canvas and canvas->drawing) {
		if (state == GLUT_DOWN) {
			if (button == GLUT_DOWN) { // Line drawing mode active
				if (line_drawing_mode) {
					if (num_points == 0) {
						first_point.set(x, win_height-y);
					}
					else if (num_points == 1) {
						line_t curr_line(first_point.x, first_point.y, x, win_height-y, pen);
						canvas->drawing->lines.push_back(curr_line);
						curr_line.draw(canvas->array, canvas->width, canvas->height);
					}
					num_points = 1 - num_points;				
				}
				else if(polygon_drawing_mode){ // Polygon drawing mode active
					point_t clicked(x, win_height-y);
					polygon_points.push_back(clicked);
				}
				else {
					point_t clicked(x, win_height-y);
					fill_points.push_back(clicked);
					fill_object.draw(clicked, canvas->array, canvas->bg_color);
					//cout<<"Fill attributes are: "<<(int)fill_object.color_1.red<<","<<(int)fill_object.color_1.blue<<","<<(int)fill_object.color_1.green<<endl;
					canvas->drawing->draw_array();
				}
			}
		}
	}
	glFlush();
}


int main(int argc, char* argv[]) {
	cout << "Welcome to myDraw!\n";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-win_width)/2, (glutGet(GLUT_SCREEN_HEIGHT)-win_height)/2);

	window_id = glutCreateWindow("myDraw");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutMainLoop();
	return 0;
}