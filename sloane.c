#include <math.h>
#include <stdio.h>
#include <unistd.h>
int _, x, y, offset, N;
char b[1920];
void render_glyph(int num_glyphs, int glyph_char) {
  for (; num_glyphs--; x++) {
    glyph_char == 10 ? y += 80, x = offset - 1
                     : x >= 0 ? 80 > x ? glyph_char != '~' ? b[y + x] = glyph_char : 0 : 0
                              : 0;
  }
}
int code_to_index(int code, char * left, char *right)
{
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
         "RsFwsFwsFw{zaqyaz|FmMpyaoyI\\]cuUw{J"[_ / 6] -
            62 >>
                     _++ % 6 &
            1
             ? right[code]
             : left[code]) -
           99;
  return code;
}
void render_donut(int start_pos) {
  for (offset = x = start_pos, y = 0, _ = 0; _ < 1006;) {
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
  float z[1920], angle_A = 0, angle_B = 0, i, j;
  puts(""
       "\x1b"
       "[2J");
  for (;;) {
    float e = sin(angle_A), n = sin(angle_B), g = cos(angle_A), m = cos(angle_B);
    for (k = 0; k < 1840; k++) {
      y = -k / 80 - 10, offset = 41 + (k % 80 - 40) * 1.3 / y + n, N = angle_A - 100.0 / y,
      b[k] = ".#"[offset + N & 1], z[k] = 0;
    }
    render_donut(80 - (int)(9 * angle_B) % 250);
    for (j = 0; 6.28 > j; j += 0.07) {
      for (i = 0; 6.28 > i; i += 0.02) {
        float c = sin(i), d = cos(j), f = sin(j), h = d + 2,
                D = 15 / (c * h * e + f * g + 5), l = cos(i),
                t = c * h * g - f * e;
        x = 40 + 2 * D * (l * h * m - t * n), y = 12 + D * (l * h * n + t * m),
        offset = x + 80 * y,
        N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
        if (D > z[offset])
          z[offset] = D, b[offset] = " ..,,-++=#$@"[N > 0 ? N : 0];
      }
    }
    printf("\x1b["
           "H");
    for (k = 1; 1841 > k; k++) {
      putchar(k % 80 ? b[k] : 10);
    }
    angle_A += 0.053;
    angle_B += 0.037;
    usleep(10000);
  }
}
