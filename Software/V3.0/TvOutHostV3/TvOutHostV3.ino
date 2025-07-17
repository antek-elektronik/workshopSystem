/*
 * ===============================================================
 * Project: TvOutHostV3
 * Description:
 *   Arduino Uno-based graphics host using TVout library.
 *   Receives serial commands via pollserial and renders graphics
 *   to an NTSC screen, acting as a minimal microcontroller "graphics card".
 *
 * Commands:
 *   &clear or &clr
 *     Clears the entire screen.
 *
 *   &newline or &breakline
 *     Prints a newline (moves text cursor to next line).
 *
 *   &rect x y w h
 *     Draws an unfilled rectangle at (x,y) with width w and height h.
 *     Example: &rect 10 10 50 20
 *
 *   &fillrect x y w h
 *     Draws a filled rectangle at (x,y) with width w and height h.
 *     Example: &fillrect 10 10 50 20
 *
 *   &line x1 y1 x2 y2
 *     Draws a line from (x1,y1) to (x2,y2).
 *     Example: &line 0 0 50 50
 *
 *   &pixel x y
 *     Sets a single pixel at (x,y) to white.
 *     Example: &pixel 30 40
 *
 *   &circle x y r
 *     Draws an unfilled circle centered at (x,y) with radius r.
 *     Example: &circle 60 30 10
 *
 *   &fillcircle x y r
 *     Draws a filled circle centered at (x,y) with radius r.
 *     Example: &fillcircle 60 30 10
 *
 *   &triangle x1 y1 x2 y2 x3 y3
 *     Draws a triangle with corners at the specified coordinates.
 *     Example: &triangle 10 10 50 10 30 40
 *
 *   &invert
 *     Inverts all screen pixels (black <-> white).
 *
 *   <plain text>
 *     Any other input prints directly to the screen as text.
 *
 * Changelog:
 *   [v3.0]
 *     - Initial TvOut serial host with clear and text printing
 *   [v3.1]
 *     - Implemented line-based serial command parsing
 *   [v3.2]
 *     - Added draw_rect, fillrect, line, pixel, circle, fillcircle commands
 *   [v3.3]
 *     - Added triangle and invert commands
 *     - Replaced String parsing with strtok for RAM efficiency
 *     - Improved circle implementation using midpoint algorithm
 *
 * Author: Antoni Gzara & GPT-4o  (2025)
 * ===============================================================
 */


#include <TVout.h>
#include <pollserial.h>
#include <fontALL.h>

TVout TV;
pollserial pserial;

char data[64];
int data_index = 0;

// Midpoint circle algorithm (outline)
void drawCircleMidpoint(int x0, int y0, int radius) {
  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    TV.set_pixel(x0 + x, y0 + y, 1);
    TV.set_pixel(x0 + y, y0 + x, 1);
    TV.set_pixel(x0 - y, y0 + x, 1);
    TV.set_pixel(x0 - x, y0 + y, 1);
    TV.set_pixel(x0 - x, y0 - y, 1);
    TV.set_pixel(x0 - y, y0 - x, 1);
    TV.set_pixel(x0 + y, y0 - x, 1);
    TV.set_pixel(x0 + x, y0 - y, 1);

    y += 1;
    if (err <= 0) {
      err += 2 * y + 1;
    }
    if (err > 0) {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

// Filled circle using scanline filling of midpoint circle
void fillCircle(int x0, int y0, int r) {
  for (int y = -r; y <= r; y++) {
    int dx = (int)sqrt(r * r - y * y);
    for (int x = -dx; x <= dx; x++) {
      TV.set_pixel(x0 + x, y0 + y, 1);
    }
  }
}

// Draw triangle outline with 3 lines
void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
  TV.draw_line(x1, y1, x2, y2, 1);
  TV.draw_line(x2, y2, x3, y3, 1);
  TV.draw_line(x3, y3, x1, y1, 1);
}

// Invert screen pixels
void invertScreen() {
  for (int y = 0; y < TV.vres(); y++) {
    for (int x = 0; x < TV.hres(); x++) {
      char pix = TV.get_pixel(x, y);
      TV.set_pixel(x, y, !pix);
    }
  }
}

void setup() {
  TV.begin(NTSC);
  TV.select_font(font6x8);
  TV.println("TvOut Host");
  TV.println("-- Extended Commands --");
  TV.set_hbi_hook(pserial.begin(115200));
}

void loop() {
  while (pserial.available() > 0) {
    char c = (char)pserial.read();

    if (c == '\n') {
      data[data_index] = '\0';

      if (data[0] == '&') {
        if (strcmp(data, "&clear") == 0 || strcmp(data, "&clr") == 0) {
          TV.clear_screen();
        } 
        else if (strncmp(data, "&rect", 5) == 0) {
          char *cmd = strtok(data, " ");
          char *sx = strtok(NULL, " ");
          char *sy = strtok(NULL, " ");
          char *sw = strtok(NULL, " ");
          char *sh = strtok(NULL, " ");
          if (sx && sy && sw && sh) {
            TV.draw_rect(atoi(sx), atoi(sy), atoi(sw), atoi(sh), 1);
          }
        }
        else if (strncmp(data, "&fillrect", 9) == 0) {
          char *cmd = strtok(data, " ");
          char *sx = strtok(NULL, " ");
          char *sy = strtok(NULL, " ");
          char *sw = strtok(NULL, " ");
          char *sh = strtok(NULL, " ");
          if (sx && sy && sw && sh) {
            TV.draw_rect(atoi(sx), atoi(sy), atoi(sw), atoi(sh), 1, 1);
          }
        }
        else if (strncmp(data, "&line", 5) == 0) {
          char *cmd = strtok(data, " ");
          char *sx1 = strtok(NULL, " ");
          char *sy1 = strtok(NULL, " ");
          char *sx2 = strtok(NULL, " ");
          char *sy2 = strtok(NULL, " ");
          if (sx1 && sy1 && sx2 && sy2) {
            TV.draw_line(atoi(sx1), atoi(sy1), atoi(sx2), atoi(sy2), 1);
          }
        }
        else if (strncmp(data, "&pixel", 6) == 0) {
          char *cmd = strtok(data, " ");
          char *sx = strtok(NULL, " ");
          char *sy = strtok(NULL, " ");
          if (sx && sy) {
            TV.set_pixel(atoi(sx), atoi(sy), 1);
          }
        }
        else if (strncmp(data, "&circle", 7) == 0) {
          char *cmd = strtok(data, " ");
          char *sx = strtok(NULL, " ");
          char *sy = strtok(NULL, " ");
          char *sr = strtok(NULL, " ");
          if (sx && sy && sr) {
            int x = atoi(sx);
            int y = atoi(sy);
            int r = atoi(sr);
            if (r > 0) {
              drawCircleMidpoint(x, y, r);
            }
          }
        }
        else if (strncmp(data, "&fillcircle", 11) == 0) {
          char *cmd = strtok(data, " ");
          char *sx = strtok(NULL, " ");
          char *sy = strtok(NULL, " ");
          char *sr = strtok(NULL, " ");
          if (sx && sy && sr) {
            int x = atoi(sx);
            int y = atoi(sy);
            int r = atoi(sr);
            if (r > 0) {
              fillCircle(x, y, r);
            }
          }
        }
        /*
        // Cursor command removed because TVout doesn't support public cursor setter
        else if (strncmp(data, "&cursor", 7) == 0) {
          // Not supported
        }
        */
        else if (strncmp(data, "&triangle", 9) == 0) {
          char *cmd = strtok(data, " ");
          char *sx1 = strtok(NULL, " ");
          char *sy1 = strtok(NULL, " ");
          char *sx2 = strtok(NULL, " ");
          char *sy2 = strtok(NULL, " ");
          char *sx3 = strtok(NULL, " ");
          char *sy3 = strtok(NULL, " ");
          if (sx1 && sy1 && sx2 && sy2 && sx3 && sy3) {
            drawTriangle(atoi(sx1), atoi(sy1), atoi(sx2), atoi(sy2), atoi(sx3), atoi(sy3));
          }
        }
        else if (strcmp(data, "&invert") == 0) {
          invertScreen();
        }
        else {
          TV.println("Unknown cmd");
        }
      } else {
        TV.print(data);
      }

      data_index = 0;
    } else {
      if (data_index < sizeof(data) - 1) {
        data[data_index++] = c;
      }
    }
  }
}
