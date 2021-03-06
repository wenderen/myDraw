#ifndef _CANVAS_T_H_
#define _CANVAS_T_H_

#include "drawing_t.hpp"
#include "color_t.hpp"
#include "pen_t.hpp"
#include "fill_t.hpp"

class drawing_t;

class canvas_t {
public:
	drawing_t* drawing;
	int width;
	int height;
	color_t bg_color;

	color_t** array;

	canvas_t(int w, int h, color_t bg_c, drawing_t* d) {
		width = w;
		height = h;

		bg_color = bg_c;
		// Allocate memory for the array and clear it
		array = new color_t*[width];
		for (int i = 0 ; i < width ; i++) array[i] = new color_t[height];
		clear();

		drawing = d;
	}

	void clear() {
		for (int x = 0 ; x < width ; x++) {
			for (int y = 0 ; y < height ; y++) {
				array[x][y] = bg_color;
			}
		}
	}
};

#endif