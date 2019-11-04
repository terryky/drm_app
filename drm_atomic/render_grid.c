#include <stdio.h>
#include <string.h>

void
render_grid (void *imgbuf, int buf_w, int buf_h, int ofst_x)
{
    unsigned int *buf = (unsigned int *)imgbuf;
    int i, j;

    memset (buf, 0, buf_w * buf_h * 4);
    
    for (i = 0; i < buf_h; i ++)
    {
        for (j = 0; j < buf_w; j ++)
        {
            int p = i * buf_w + j;
            int x = j + ofst_x;
            int y = i + ofst_x;
            
#if 1
            if ((x %  10 == 0) || (y %  10 == 0)) buf[p] = 0xFFC0C0BC;
            if ((x % 100 == 0) || (y % 100 == 0)) buf[p] = 0xFFFF0000;
            if ((x % 500 == 0) || (y % 500 == 0)) buf[p] = 0xFF0000FF;
#else
            if ((x % 500 == 0) || (y % 500 == 0)) buf[p] = 0xFF0000FF;
            //else if ((y % 100 == 0)) buf[p] = 0xFFFF0000;
            //else if ((y %  10 == 0)) buf[p] = 0xFFC0C0BC;
#endif
        }
    }
}


void 
render_fill_rect (void *imgbuf, int buf_w, int buf_h, 
                  int x, int y, int w, int h, unsigned int color)
{
    unsigned int *buf = (unsigned int *)imgbuf;
    int i, j;
    int sx, sy, ex, ey;
    
    sx = x;
    sy = y;
    ex = sx + w - 1;
    ey = sy + h - 1 ;
    
    if ((sx >= buf_w) || (sy >= buf_h) || (ex < 0) || (ey < 0))
        return;

    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;
    if (ex >= buf_w) ex = buf_w - 1;
    if (ey >= buf_h) ex = buf_h - 1;
    
    for (i = sy; i <= ey; i ++)
    {
        for (j = sx; j <= ex; j ++)
        {
            int p = i * buf_w + j;
            buf[p] = color;
        }
    }    
    
}
