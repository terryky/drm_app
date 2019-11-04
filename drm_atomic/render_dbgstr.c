#include <stdio.h>
#include <string.h>

#define DBGSTR_FONT_WIDTH   (8)
#define DBGSTR_FONT_HEIGHT  (8)
#define DBGSTR_FONT_SIZE    (DBGSTR_FONT_WIDTH * DBGSTR_FONT_HEIGHT / 8)
#define DBGSTR_NUM_STR      (96)
#define DBGSTR_WIDTH        (5)
#define DBGSTR_HEIGHT       (8)
#define DBGSTR_IMAGE_WIDTH  (8)
#define DBGSTR_IMAGE_HEIGHT (8 * 128)
#define DBGSTR_IMAGE_SIZE   (DBGSTR_IMAGE_WIDTH * DBGSTR_IMAGE_HEIGHT)

static const unsigned int 
font8x8[DBGSTR_NUM_STR * 2] = 
{
  0x00000000,0x00000000,0x00202020,0x20002000,0x00505050,0x00000000,0x5050f850,0xf8505000,
  0x2070a070,0x28702000,0x00405020,0x50100000,0x20505020,0x50502800,0x00302040,0x00000000,
  0x00102020,0x20100000,0x00402020,0x20400000,0x00483078,0x30480000,0x002020f8,0x20200000,
  0x00000000,0x30204000,0x00000000,0x78000000,0x00000000,0x20702000,0x00080810,0x20404000,
  0x00205050,0x50502000,0x00206020,0x20207000,0x00304808,0x30407800,0x00304810,0x08483000,
  0x00103050,0x78101000,0x00784070,0x08483000,0x00304050,0x68483000,0x00780810,0x10202000,
  0x00304830,0x48483000,0x00304858,0x28083000,0x00303000,0x30300000,0x00303000,0x30204000,
  0x00102040,0x40201000,0x00007000,0x70000000,0x00402010,0x10204000,0x00205010,0x20002000,
  0x304898a8,0xa8904030,0x00304848,0x78484800,0x00704870,0x48487000,0x00304840,0x40483000,
  0x00704848,0x48487000,0x00784070,0x40407800,0x00784070,0x40404000,0x00304840,0x58483000,
  0x00484878,0x48484800,0x00702020,0x20207000,0x00380808,0x08483000,0x00485060,0x50504800,
  0x00404040,0x40407000,0x00487878,0x48484800,0x00486878,0x58584800,0x00304848,0x48483000,
  0x00704848,0x70404000,0x00304848,0x68583008,0x00704848,0x70584800,0x00304820,0x10483000,
  0x00f82020,0x20202000,0x00484848,0x48483000,0x00484848,0x48303000,0x00484848,0x78784800,
  0x00484830,0x30484800,0x00888850,0x20202000,0x00780810,0x20407800,0x00704040,0x40407000,
  0x00404020,0x10080800,0x00701010,0x10107000,0x00205050,0x00000000,0x00000000,0x00000078,
  0x00604020,0x00000000,0x00000028,0x58582800,0x00404070,0x48487000,0x00000030,0x40403000,
  0x00080828,0x58582800,0x00000030,0x78403000,0x00102820,0x70202000,0x00000030,0x48380830,
  0x00404070,0x48484800,0x00200060,0x20207000,0x00100010,0x10105020,0x00404048,0x70484800,
  0x00602020,0x20207000,0x00000050,0xa8a88800,0x00000070,0x48484800,0x00000030,0x48483000,
  0x00000070,0x48704040,0x00000038,0x48380808,0x00000050,0x68404000,0x00000070,0x60107000,
  0x00202070,0x20281000,0x00000048,0x48483800,0x00000050,0x50502000,0x00000088,0xa8a87000,
  0x00000048,0x30304800,0x00000048,0x48384830,0x00000078,0x10207800,0x18201060,0x10201800,
  0x00202020,0x20202000,0x60102018,0x20106000,0x00285000,0x00000000,0x00000000,0x00000000,
};

static unsigned char font8x8UB[DBGSTR_IMAGE_SIZE];

int
init_debug_string ()
{
    int i, j, k = 0;

    for (i = 0; i < 96*2; i += 2)
    {
        unsigned int imgUpper = font8x8[i    ];
        unsigned int imgLower = font8x8[i + 1];
        unsigned int pixel;
      
        for (j = 0; j < 32; j ++)
        {
            pixel = imgUpper & 0x80000000;
            imgUpper <<= 1;
            font8x8UB[ k ++ ] = 0xFF * (pixel >> 31);
        }
      
        for (j = 0; j < 32; j ++)
        {
            pixel = imgLower & 0x80000000;
            imgLower <<= 1;
            font8x8UB[ k ++ ] = 0xFF * (pixel >> 31);
        }
    }

    return 0;
}



static void
render_bbt_i8 (void *dst_buf, int dst_w, int dst_h,
               void *src_buf, int src_w, int src_h,
               int sx, int sy, int sw, int sh,
               int dx, int dy,
               unsigned int fg_color, unsigned int bg_color)
{
    int x, y;
    unsigned int  *lpdst = dst_buf;
    unsigned char *lpsrc = src_buf;

    for (y = 0; y < sh; y ++)
    {
        if (((dy + y) > dst_h) || ((sy + y) > src_h))
            return;

        for (x = 0; x < sw; x ++)
        {
            if (((dx + x) > dst_w) || ((sx + x) > src_w))
                continue;

            unsigned int src = lpsrc[(sy + y) * src_w + (sx + x)];
            unsigned int r, g, b, a;
            float sval = (float)src / 255.0f;

            if (sval)
            {
                a = ((fg_color >> 24) & 0xFF);
                r = ((fg_color >> 16) & 0xFF) * sval;
                g = ((fg_color >>  8) & 0xFF) * sval;
                b = ((fg_color >>  0) & 0xFF) * sval;
            }
            else
            {
                a = ((bg_color >> 24) & 0xFF);
                r = ((bg_color >> 16) & 0xFF);
                g = ((bg_color >>  8) & 0xFF);
                b = ((bg_color >>  0) & 0xFF);
            }

            lpdst[(dy + y) * dst_w + (dx + x)] = (a << 24) | (r << 16) | (g << 8) | b; // AARRGGBB;
        }
    }
}


int
draw_debug_string (void *dst_buf, int dst_w, int dst_h,
                   unsigned int fg_color, unsigned int bg_color,
                   float scale, char *str, int x, int y)
{
  char c;
  int  i;
  int  sx, sy, dx, dy;


    dx = x;
    dy = y;
    for (i = 0; ; i++)
    {
        c = str[i];
      
        if (c == 0)
            break;

        if (c == '\n')
        {
            dx = x;
            dy += DBGSTR_IMAGE_HEIGHT * 2;
            continue;
        }
      
        sx = 0;
        sy = (c - 0x20) * DBGSTR_HEIGHT;

        render_bbt_i8 (dst_buf, dst_w, dst_h,
                       font8x8UB, DBGSTR_IMAGE_WIDTH, DBGSTR_IMAGE_HEIGHT,
                       sx, sy, DBGSTR_WIDTH, DBGSTR_HEIGHT, 
                       dx, dy, fg_color, bg_color);

        dx += DBGSTR_WIDTH * scale;
    }

    return 0;
}
