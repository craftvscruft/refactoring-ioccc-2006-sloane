#include <math.h>
#include <stdio.h>
#include <unistd.h>

static const double TWO_PI = 6.28;
static const double theta_step = 0.07;
static const double phi_step = 0.02;
static const int SCALING_FACTOR = 15;
static const int FRAME_BUFFER_WIDTH = 80;
static const int FRAME_BUFFER_SIZE = 1840;
int str_idx, x, y, buffer_offset;
char frame_buffer[1920];

void print_frame_buffer();

void render_donut(float *depth_buffer, float angle_A, float angle_B);

void render_checker_board(float *depth_buffer, float angle_A, float angle_B);

void render_glyph(int num_glyphs, int glyph_char) {
  for (; num_glyphs--; x++) {
    if (glyph_char == 10) {
      y += FRAME_BUFFER_WIDTH;
      x = buffer_offset - 1;
    } else {
      if (x >= 0 && FRAME_BUFFER_WIDTH > x && glyph_char != '~') {
        frame_buffer[y + x] = glyph_char;
      }
    }
  }
}

int code_to_index(int code, char *left, char *right) {
  // Function iterates through a string and performs a calculation based on the passed code, left and right strings.
  // The result is an index into left or right string depending on the calculation.

  // Careful adding braces here, tests broke
  while (code >= 0) {
    int left_string_chunk_size = 6;
    int char_base_offset = 62;
    int char_code_offset = 99;
    code = ("E"
            "?yYrIxC{e^}KhE>[|LXbj}"
            "dOVsJ"
            "@"
            "idOV{Yab[bW}[bW}\\qFywyv{D"
            "ma\\A"
            ""
            "Ztq?Lyw>e{|Zq>Y\\gq\\qI[tYBe{w"
            "yvDZE\
vBA[`_"
            "Lo>}KcqdYrWqKxzKtW]|DXRwsfcUaT\\\
KXw{Y"
            "RsFwsFwsFw{zaqyaz|FmMpyaoyI\\]cuUw{J"[str_idx / left_string_chunk_size] -
            char_base_offset >>
                             str_idx++ % left_string_chunk_size &
            1
            ? right[code]
            : left[code]) -
           char_code_offset;
  }
  return code;
}

void render_banner(int start_pos) {
  for (buffer_offset = x = start_pos, y = 0, str_idx = 0; str_idx < 1006;) {
    int glyph_offset = 34;
    int left_size = 6;
    int left_offset = 8;
    int right_offset = 13;
    int right_size = 11;
    start_pos = " /\\\n"
                "~|_."[code_to_index(left_size,
                                     "b"
                                     "cd\\a[g",
                                     "^`"
                                     "_e"
                                     "]fh") +
                       left_offset],
            render_glyph("#$%"
                         "&'()*+,-.1"[code_to_index(right_size,
                                                    "_ac[]\\YZi"
                                                    "jkm",
                                                    "`bd^efghXWlV") +
                                      right_offset] -
                         glyph_offset,
                         start_pos);
  }
}

int main(int k, char **argv) {
  float depth_buffer[1920];
  float angle_A = 0;
  float angle_B = 0;

  puts("\x1b[2J"); // Clear screen
  for (;;) {
    render_checker_board(depth_buffer, angle_A, angle_B);
    render_banner(FRAME_BUFFER_WIDTH - (int) (9 * angle_B) % 250);
    render_donut(depth_buffer, angle_A, angle_B);
    printf("\x1b[H"); // Move cursor to home
    print_frame_buffer();
    angle_A += 0.053;
    angle_B += 0.037;
    usleep(10000);
  }
}

void render_checker_board(float *depth_buffer, float angle_A, float angle_B) {
  float sin_B1 = sin(angle_B);
  for (int k = 0; k < FRAME_BUFFER_SIZE; k++) {
    int y_offset = 10;
    y = -k / FRAME_BUFFER_WIDTH - y_offset;
    buffer_offset = 41 + (k % FRAME_BUFFER_WIDTH - 40) * 1.3 / y + sin_B1;
    int N = angle_A - 100.0 / y;
    frame_buffer[k] = ".#"[buffer_offset + N & 1];
    depth_buffer[k] = 0;
  }
}

void render_donut(float *depth_buffer, float angle_A, float angle_B) {
  // Updates the frame buffer based on angle_A and angle_B
  float sin_A = sin(angle_A);
  float sin_B = sin(angle_B);
  float cos_A = cos(angle_A);
  float cos_B = cos(angle_B);
  for (float theta = 0; TWO_PI > theta; theta += theta_step) {
    for (float phi = 0; TWO_PI > phi; phi += phi_step) {
      float sin_phi = sin(phi);
      float cos_theta = cos(theta);
      float sin_theta = sin(theta);
      float inc_cos_theta = cos_theta + 2;
      float distance = SCALING_FACTOR / (sin_phi * inc_cos_theta * sin_A + sin_theta * cos_A + 5);
      float l = cos(phi);
      float t = sin_phi * inc_cos_theta * cos_A - sin_theta * sin_A;
      x = 40 + 2 * distance * (l * inc_cos_theta * cos_B - t * sin_B);
      y = 12 + distance * (l * inc_cos_theta * sin_B + t * cos_B);
      buffer_offset = x + FRAME_BUFFER_WIDTH * y;
      int N = 8 *
          ((sin_theta * sin_A - sin_phi * cos_theta * cos_A) * cos_B - sin_phi * cos_theta * sin_A - sin_theta * cos_A -
           l * cos_theta * sin_B);

      if (distance > depth_buffer[buffer_offset]) {
        // If the new point is closer to the viewer than the previously drawn point, it will be drawn, and the depth buffer will be updated.
        depth_buffer[buffer_offset] = distance;
        frame_buffer[buffer_offset] = " ..,,-++=#$@"[N > 0 ? N : 0];
      }
    }
  }
}

void print_frame_buffer() {
  for (int k = 1; 1841 > k; k++) {
    putchar(k % FRAME_BUFFER_WIDTH ? frame_buffer[k] : 10);
  }
}
