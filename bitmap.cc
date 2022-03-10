#include "bitmap.hh"

#include <math.h>
#include <webp/decode.h>
#include <webp/encode.h>

#include <fstream>
#include <random>

using namespace std;
/*

        AUTHOR: Ajay Thakkar, Juan Jimenez
        CITE: https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
        PLEDGE: We pledge our honor we have abided by the Stevens Honor System
*/

/*
        In order to compile you will need to install libwebp, a new graphics
        standard from google. Most image programs won't yet work with webp,
        which is better compression than jpeg in general.

        In order to install under msys2:
        https://packages.msys2.org/package/mingw-w64-x86_64-libwebp

        In order to access a particular element of this bitmap (x,y) use the
        following equation point(x,y) does this

        y*w + x

        this works because the first point is 0*w+0 = 0, the second would be
   0*w+1

        one row down would be 1*w + 0
 */

bitmap::bitmap(uint32_t w, uint32_t h, uint32_t color)
    : w(w), h(h), rgb(new uint32_t[w * h]) {
  for (uint32_t i = 0; i < w * h; i++) rgb[i] = color;
}

bitmap::~bitmap() { delete[] rgb; }

void bitmap::clear(uint32_t color) {
  for (uint32_t i = 0; i < w * h; i++) rgb[i] = color;
}

void bitmap::horiz_line(uint32_t x1, uint32_t x2, uint32_t y, uint32_t color) {
  // TODO: implement this
  for (int i = x1; i <= x2; i++) {
    rgb[y * w + i] = color;
  }
}

void bitmap::vert_line(uint32_t y1, uint32_t y2, uint32_t x, uint32_t color) {
  // TODO: implement this
  for (int i = y1; i <= y2; i++) {
    rgb[i * w + x] = color;
  }
}

void bitmap::fill_rect(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
                       uint32_t color) {
  // TODO: implement this
  for (int i = x0; i <= x0 + w; i++) {
    for (int j = y0; j <= y0 + h; j++) {
      vert_line(y0, j, i, color);
    }
  }
}

void bitmap::rect(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
                  uint32_t color) {
  // TODO: implement this
  horiz_line(x0, (x0 + w), y0, color);
  vert_line(y0, (y0 + h), x0, color);
  horiz_line(x0, (x0 + w), (y0 + h), color);
  vert_line(y0, (y0 + h), (x0 + w), color);
}

void bitmap::grid(uint32_t x0, uint32_t y0, uint32_t w, uint32_t h,
                  uint32_t divx, uint32_t divy, uint32_t color) {
  // TODO: implement this
  rect(x0, y0, w, h, color);
  int spacingh = h / divx;
  int spacingv = w / divy;
  for (int i = 0; i < divx; i++) {
    horiz_line(x0, x0 + w, y0 + spacingh, color);
    spacingh += h / divx;
  }
  for (int i = 0; i < divy; i++) {
    vert_line(y0, y0 + h, x0 + spacingv, color);
    spacingv += w / divy;
  }
}

std::default_random_engine gen;

// https:
// stackoverflow.com/questions/22853349/how-to-generate-random-32bit-integers-in-c
void bitmap::random(uint32_t x0, uint32_t y0, uint32_t rw, uint32_t rh,
                    uint32_t color) {
  // TODO: implement this
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> distx(x0, x0 + rw);
  uniform_int_distribution<> disty(y0, y0 + rh);

  for (int x = x0; x <= x0 + rw; x++)
    for (int y = y0; y <= y0 + rh; y++)
      rgb[(y0 + disty(gen) * w) + x + distx(gen)] = color;
}

void bitmap::flood_fill_allcolor(uint32_t x, uint32_t y, uint32_t findcolor,
                                 uint32_t replace_color) {
  // TODO:

  if (rgb[y * w + x] != findcolor) {
    return;
  }
  if (x < 0 || x > w - 1 || y < 0 || y > h - 1) {
    return;
  }
  point(x, y, replace_color);
  flood_fill_allcolor(x + 1, y, findcolor, replace_color);
  flood_fill_allcolor(x - 1, y, findcolor, replace_color);
  flood_fill_allcolor(x, y + 1, findcolor, replace_color);
  flood_fill_allcolor(x, y - 1, findcolor, replace_color);
}

void bitmap::circle(uint32_t x, uint32_t y, uint32_t d, uint32_t color) {
  // TODO: implement this

  int r = d / 2;

  int w = r, h = 0;
  point(w + x, h + y, color);  // initial points
  point(-w + x, h + y, color);
  point(h + x, -w + y, color);
  point(-h + x, w + y, color);

  int P = 1 - r;
  while (w > h) {
    h++;

    if (P <= 0) {
      P = P + 2 * h + 1;
    } else {
      w--;
      P = P + 2 * h - 2 * w + 1;
    }

    point(w + x, h + y, color);
    point(-w + x, h + y, color);
    point(w + x, -h + y, color);
    point(-w + x, -h + y, color);

    if (x != y) {
      point(h + x, w + y, color);
      point(-h + x, w + y, color);
      point(h + x, -w + y, color);
      point(-h + x, -w + y, color);
    }
  }
}
// https:  //
// stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
// extra credit +50
void bitmap::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                  uint32_t color) {
  // TODO: optional for +50
  // algorithm modeled after code shown in
  // https://www.youtube.com/watch?v=IDFB5CDpLDE
  float m = 1.0 * (y2 - y1) / (x2 - x1);
  if (m <= 1 && m >= -1) {  // case for lines with slope that is a fraction
    float offset = 0;
    // as x increases by one, y increases by factor of m. offset is used
    // to calculate the total increase of y as x increases

    float allowed_slope = .5;
    // allowed slope is used as a threshold to tell the bitmap when to increase
    // the y coordinate that the function is plotting based on the offset that
    // is accumulated as x increases
    float dy = abs(m);
    int y = y1;
    for (int x = x1; x <= x2; x++) {
      point(x, y, color);
      offset += dy;
      // each iteration of loop increases x by one, therefore increasing the
      // offset by a factor of m
      if (offset >= allowed_slope) {
        y += 1;
        allowed_slope += 1.0;
        // when the offset is greater than the threshold, it plots y+1 instead
        // of y, and increases the threshold to account for more offset as the
        // line goes on
      }
    }
  } else {  // for cases where slope of line is > 1
    float offset = 0;
    float dx = abs(1 / m);
    // when the slope is greater than one, the function must now manipulate the
    // x coordinate instead of the y coordinate. therefore, as y increases by
    // one, x increases by a factor of 1/m
    float allowed_slope = .5;
    int x = x1;
    for (int y = y1; y <= y2; y++) {
      point(x, y, color);
      offset += dx;
      if (offset >= allowed_slope) {
        x += 1;
        allowed_slope += 1;
      }
    }
  }
}
void bitmap::fill_circle(uint32_t x0, uint32_t y0, uint32_t d, uint32_t color) {
  // TODO: implement this
  int r = d / 2;
  for (int y = -r; y <= r; y++)
    for (int x = -r; x <= r; x++)
      if (x * x + y * y <= r * r + r) point(x0 + x, y0 + y, color);
}

void bitmap::half_circle(uint32_t x0, uint32_t y0, uint32_t d, uint32_t color) {
  // TODO: implement this
  int r = d / 2;
  for (int y = -r; y <= r; y++)
    for (int x = 0; x <= r; x++)
      if (x * x + y * y <= r * r + r) point(x0 + x, y0 + y, color);
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
// extra credit +50

/*int m_new = 2 * (y2 - y1);
int slope_error = m_new - (x2 - x1);
for (int x = x1, y = y1; x <= x2; x++) {
  slope_error += m_new;
  if (slope_error >= 0) {
    y++;
    slope_error -= 2 * (x2 - x1);
  }
  point(x, y, 0xFFFFFFFF);
}
int dx = x2 - x1;
int dy = y2 - y1;
float m = dy / dx;
float newm = 0;
float slope_low = 0;
float slope_high = 0;
float y = 0;
int yint = y1;
for (int x = x1; x <= x2; x++) {
  y = m * (x - x1) + y1;
  slope_low = (floor(y) - y1) / (x - x1);
  slope_high = (ceil(y) - y1) / (x - x1);
  if (abs(slope_low - m) < abs(slope_high - m)) {
    yint = static_cast<int>(floor(y));
  } else {
    yint = static_cast<int>(ceil(y));
  }
  point(x, yint, color);
}
}
*/
// https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
//  extra credit +50
/*void bitmap::wuline(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2,
                    uint32_t color) {
  // TODO: optional for +50
}
*/
void bitmap::save(const char filename[]) {
  uint8_t* out;
  size_t s = WebPEncodeRGBA((uint8_t*)rgb, w, h, 4 * w, 100, &out);
  ofstream f(filename, ios::binary);
  f.write((char*)out, s);
  WebPFree(out);
}

// not required for this assignment but +50 if you look it up and do it
//  but if you do, test it by loading in a picture, modify, and save it out
void bitmap::load(const char filename[]) {
  uint8_t* out;
  // TODO
}
