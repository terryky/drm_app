#ifndef _RENDER_DBGSTR_H_
#define _RENDER_DBGSTR_H_

int init_debug_string ();

int draw_debug_string (void *dst_buf, int dst_w, int dst_h,
                       unsigned int fg_color, unsigned int bg_color,
                       float scale, char *str, int x, int y);

#endif
