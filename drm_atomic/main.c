#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <drm_fourcc.h>
#include "util_drm.h"
#include "render_dbgstr.h"

extern int   render_telltale (unsigned int *dst, int scr_w, int scr_h, int count);
extern void  render_grid (void *imgbuf, int buf_w, int buf_h, int ofst_x);


int 
main (int argc, char *argv[])
{
    int layer_idx = 0;
    drm_obj_t drm_obj = {0};
    drm_fb_t  plane_fb;
    int format = DRM_FORMAT_ARGB8888;
    int i, ret;

    int c, option_index;
    const struct option long_options[] = 
    {
        {0, 0, 0, 0}
    };
    
    while ((c = getopt_long (argc, argv, "l:",
                    long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 'l':
            layer_idx = atoi (optarg);
            break;
        }
    }


    ret = drm_initialize (&drm_obj);
    if (ret < 0) 
    {
        fprintf (stderr, "ERR: %s(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    drm_alloc_fb (drm_obj.fd, 1920, 1080, format, &plane_fb);
    drm_add_fb   (drm_obj.fd, &plane_fb);

    drm_atomic_set_mode (&drm_obj, 0, 1);

    ret = drm_atomic_flush (&drm_obj, 1);
    if (ret < 0) 
    {
        fprintf (stderr, "ERR: %s(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    init_debug_string ();
    
    for (i = 0; ; i++)
    {
        render_telltale (plane_fb.map_buf, 1920, 1080, 0);
        draw_debug_string (plane_fb.map_buf, 1920, 1080, 0xFFFFFFFF, 0x00000000, 1.0f,
                            "aiueo", 100, 100);

        drm_atomic_set_plane (&drm_obj, &plane_fb, 0, 0, 0, layer_idx);
        drm_atomic_flush (&drm_obj, 1);
    }

    drm_atomic_set_plane (&drm_obj, NULL, 0, 0, 0, 2);
    drm_atomic_flush (&drm_obj, 0);
    sleep(1);

    drm_remove_fb (drm_obj.fd, &plane_fb);
    drm_free_fb   (drm_obj.fd, &plane_fb);

    drm_terminate (&drm_obj);

    return 0;
}
