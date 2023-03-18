#include <math.h>
#include <stdio.h>
#include <unistd.h>

static const double TWO_PI = 6.28;
static const double theta_step = 0.07;
static const double phi_step = 0.02;
static const int SCALING_FACTOR = 15;
static const int FRAME_BUFFER_WIDTH = 80;
int str_idx, x, y, buffer_offset, N;
char frame_buffer[1920];

void print_frame_buffer(int k);

void update_frame_buffer(float *depth_buffer, float angle_A, float angle_B);

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
  // Careful adding braces here, tests broke
  while (code >= 0)
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
            "RsFwsFwsFw{zaqyaz|FmMpyaoyI\\]cuUw{J"[str_idx / 6] -
            62 >>
               str_idx++ % 6 &
            1
            ? right[code]
            : left[code]) -
           99;
  return code;
}

void render_donut(int start_pos) {
  for (buffer_offset = x = start_pos, y = 0, str_idx = 0; str_idx < 1006;) {
    start_pos = " /\\\n"
                "~|_."[code_to_index(6,
                                     "b"
                                     "cd\\a[g",
                                     "^`"
                                     "_e"
                                     "]fh") +
                       8],
            render_glyph("#$%"
                         "&'()*+,-.1"[code_to_index(11,
                                                    "_ac[]\\YZi"
                                                    "jkm",
                                                    "`bd^efghXWlV") +
                                      13] -
                         34,
                         start_pos);
  }
}

int main(int k, char **argv) {
  float depth_buffer[1920];
  float angle_A = 0;
  float angle_B = 0;

  puts(""
       "\x1b"
       "[2J");
  for (;;) {


    float sin_B1 = sin(angle_B);
    for (k = 0; k < 1840; k++) {
      y = -k / FRAME_BUFFER_WIDTH - 10, buffer_offset = 41 + (k % FRAME_BUFFER_WIDTH - 40) * 1.3 / y + sin_B1, N = angle_A - 100.0 / y,
      frame_buffer[k] = ".#"[buffer_offset + N & 1], depth_buffer[k] = 0;
    }
    render_donut(FRAME_BUFFER_WIDTH - (int) (9 * angle_B) % 250);
    update_frame_buffer(depth_buffer, angle_A, angle_B);
    printf("\x1b["
           "H");
    print_frame_buffer(k);
    angle_A += 0.053;
    angle_B += 0.037;
    usleep(10000);
  }
}

void update_frame_buffer(float *depth_buffer, float angle_A, float angle_B) {
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
      N = 8 *
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

void print_frame_buffer(int k) {
  for (k = 1; 1841 > k; k++) {
    putchar(k % FRAME_BUFFER_WIDTH ? frame_buffer[k] : 10);
  }
}
