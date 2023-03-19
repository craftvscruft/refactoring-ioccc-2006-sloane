#include <math.h>
#include <stdio.h>
#include <unistd.h>

static const double TWO_PI = 6.28;
static const double theta_step = 0.07;
static const double phi_step = 0.02;
static const int SCALING_FACTOR = 15;
static const int FRAME_BUFFER_WIDTH = 80;
static const int FRAME_BUFFER_SIZE = 1840;
static const int NEWLINE = 10;
int str_idx, x, y;
char frame_buffer[1920];

void print_frame_buffer();

void render_donut(float *depth_buffer, float angle_A, float angle_B);

void render_checker_board(float *depth_buffer, float angle_A, float angle_B);

int compute_glyph_char();

int compute_glyph_count();

void render_glyph(int num_glyphs, int glyph_char, int buf_offset) {
  for (; num_glyphs--;) {
    if (glyph_char == NEWLINE) {
      y += FRAME_BUFFER_WIDTH;
      x = buf_offset;
    } else {
      if (x >= 0 && FRAME_BUFFER_WIDTH > x && glyph_char != '~') {
        frame_buffer[y + x] = glyph_char;
      }
      x++;
    }
  }
}

int code_to_index(int *arr0, int *arr1, int length) {
  // Function iterates through a string and performs a calculation based on the passed code, arr0 and arr1 strings.
  // The result is an index into arr0 or arr1 string depending on the calculation.
  int code = length - 1;
  while (code >= 0) {
    char *ms = "111000100000110111110110001011110100010111101000101111111001000001111111101100010101111000000000101110011111011100010110001001001101111111011001100010000110101011001100010000110101011001100010000110101111110110110001001001101110001001100110111111101110001001100110111111011110110011000100110111100111110111000111101111011000111101110001011110110000001110011011110011100000011100110111100111000000111001101111011111001110110011000000110110011110100101110011011110110011110100101110011011110110001000111001101111100111110111000111011000001110111000000111001000110000101110010001100001011100100011000000111111101100101001110011011001110110001011100110110011101100010111001111101100011011100110111110011111011000010110001010100111101011000101101001111010110001011010011110101100010110100111101111110110001010101011000100100111101011000100100111101011000100100111101111001111110001110011110111110001001111011111000100111101111100010011110111110001100011110111110100011110111110101001111011111010100111101111001100";

    if (ms[str_idx] == '1') {
      code = arr1[code];
    } else {
      code = arr0[code];
    }
    str_idx++;
  }
  return code;
}

void render_banner(int start_pos) {
  int buffer_offset = x = start_pos;
  for (y = 0, str_idx = 0; str_idx < 1006;) {
    int glyph = compute_glyph_char();
    int glyph_count = compute_glyph_count();

    render_glyph(glyph_count, glyph, buffer_offset);
  }
}

int compute_glyph_count() {
  int r1[] = {-4, -2, 0, -8, -6, -7, -10, -9, 6, 7, 8, 10};
  int r2[] = {-3, -1, 1, -5, 2, 3, 4, 5, -11, -12, 9, -13};
  int right_offset = 14;
  int right_size = 12;
  int glyph_count = code_to_index(r1, r2, right_size) + right_offset;
  return glyph_count <= 12 ? glyph_count : 15;
}

int compute_glyph_char() {
  int left_size = 7;
  int left_offset = 8;
  int l1[] = {-1, 0, 1, -7, -2, -8, 4};
  int l2[] = {-5, -3, -4, 2, -6, 3, 5};
  int glyph_type_idx = code_to_index(l1, l2, left_size) + left_offset;
  return " /\\\n~|_."[glyph_type_idx];
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
    int buffer_offset = 41 + (k % FRAME_BUFFER_WIDTH - 40) * 1.3 / y + sin_B1;
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
      int buffer_offset = x + FRAME_BUFFER_WIDTH * y;
      int N = 8 *
              ((sin_theta * sin_A - sin_phi * cos_theta * cos_A) * cos_B - sin_phi * cos_theta * sin_A -
               sin_theta * cos_A -
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
    putchar(k % FRAME_BUFFER_WIDTH ? frame_buffer[k] : NEWLINE);
  }
}
