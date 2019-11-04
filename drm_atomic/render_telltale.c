#include <stdio.h>

extern unsigned int bmp_00[];
extern unsigned int bmp_01[];
extern unsigned int bmp_02[];
extern unsigned int bmp_03[];
extern unsigned int bmp_04[];
extern unsigned int bmp_05[];
extern unsigned int bmp_06[];
extern unsigned int bmp_07[];
extern unsigned int bmp_08[];
extern unsigned int bmp_09[];
extern unsigned int bmp_10[];
extern unsigned int bmp_11[];
extern unsigned int bmp_12[];
extern unsigned int bmp_13[];
extern unsigned int bmp_14[];
extern unsigned int bmp_15[];

static int s_ofst_x[16];
static int s_dsig_x[16];
static int s_ofst_y[16];
static int s_dsig_y[16];
static int s_bfirst = 1;


void
render_clear (unsigned int *dst_buf, int dst_w, int dst_h, unsigned int color)
{
    int x, y;
    for (y = 0; y < dst_h; y ++)
    {
        for (x = 0; x < dst_w; x ++)
        {
            *dst_buf ++ = color;
        }
    }
}

void
render_bbt (unsigned int *dst_buf, int dst_w, int dst_h,
            unsigned int *src_buf, int src_w, int src_h,
            int dst_x, int dst_y)
{
    unsigned int *pdst, *psrc;
    int sx, sy, dx, dy;
    
    for (sy = 0; sy < src_h; sy ++)
    {
        dy = dst_y + sy;
        if (dy >= dst_h)
            break;
        if (dy < 0)
            continue;

        pdst = dst_buf + (dst_w * dy + dst_x);
        psrc = src_buf + (src_w * sy +     0);

        for (sx = 0; sx < src_w; sx ++)
        {
            dx = dst_x + sx;
            if (dx >= dst_w)
                break;
            if (dx < 0)
            {
                pdst ++;
                psrc ++;
                continue;
            }

            *pdst ++ = *psrc ++;
        }
    }
}


int
render_telltale (unsigned int *dst, int scr_w, int scr_h, int count)
{
    int i;

    if (s_bfirst)
    {
        for (i = 0; i < 16; i ++ )
        {
            s_ofst_x[i] = 100 * (i + 1);
            s_dsig_x[i] = 5;
            s_ofst_y[i] = 50  * (i + 1);
            s_dsig_y[i] = 5;
        }
        s_bfirst = 0;
    }

    for (i = 0; i < 16; i ++ )
    {
        if (s_ofst_x[i] > scr_w - 64) s_dsig_x[i] *= -1;
        if (s_ofst_x[i] <          0) s_dsig_x[i] *= -1;
        if (s_ofst_y[i] > scr_h - 64) s_dsig_y[i] *= -1;
        if (s_ofst_y[i] <          0) s_dsig_y[i] *= -1;
        s_ofst_y[i] += s_dsig_y[i];
        s_ofst_x[i] += s_dsig_x[i];
    }
    
    
    /* clear framebuffer with solid color. */
    render_clear (dst, scr_w, scr_h, 0x00000000);

    /* render warning lamp icons */
    render_bbt (dst, scr_w, scr_h, bmp_00, 64, 64, s_ofst_x[ 0], s_ofst_y[ 0]);
    render_bbt (dst, scr_w, scr_h, bmp_01, 64, 64, s_ofst_x[ 1], s_ofst_y[ 1]);
    render_bbt (dst, scr_w, scr_h, bmp_02, 64, 64, s_ofst_x[ 2], s_ofst_y[ 2]);
    render_bbt (dst, scr_w, scr_h, bmp_03, 64, 64, s_ofst_x[ 3], s_ofst_y[ 3]);
    render_bbt (dst, scr_w, scr_h, bmp_04, 64, 64, s_ofst_x[ 4], s_ofst_y[ 4]);
    render_bbt (dst, scr_w, scr_h, bmp_05, 64, 64, s_ofst_x[ 5], s_ofst_y[ 5]);
    render_bbt (dst, scr_w, scr_h, bmp_06, 64, 64, s_ofst_x[ 6], s_ofst_y[ 6]);
    render_bbt (dst, scr_w, scr_h, bmp_07, 64, 64, s_ofst_x[ 7], s_ofst_y[ 7]);
    render_bbt (dst, scr_w, scr_h, bmp_08, 64, 64, s_ofst_x[ 8], s_ofst_y[ 8]);
    render_bbt (dst, scr_w, scr_h, bmp_09, 64, 64, s_ofst_x[ 9], s_ofst_y[ 9]);
    render_bbt (dst, scr_w, scr_h, bmp_10, 64, 64, s_ofst_x[10], s_ofst_y[10]);
    render_bbt (dst, scr_w, scr_h, bmp_11, 64, 64, s_ofst_x[11], s_ofst_y[11]);
    render_bbt (dst, scr_w, scr_h, bmp_12, 64, 64, s_ofst_x[12], s_ofst_y[12]);
    render_bbt (dst, scr_w, scr_h, bmp_13, 64, 64, s_ofst_x[13], s_ofst_y[13]);
    render_bbt (dst, scr_w, scr_h, bmp_14, 64, 64, s_ofst_x[14], s_ofst_y[14]);
    render_bbt (dst, scr_w, scr_h, bmp_15, 64, 64, s_ofst_x[15], s_ofst_y[15]);

    return 0;
}

