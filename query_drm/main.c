#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#define UNUSED(x) (void)(x)


static char *
get_connector_type (int type)
{
    switch (type)
    {
    case DRM_MODE_CONNECTOR_Unknown:     return "UNKNOWN";
    case DRM_MODE_CONNECTOR_VGA:         return "VGA";
    case DRM_MODE_CONNECTOR_DVII:        return "DVII";
    case DRM_MODE_CONNECTOR_DVID:        return "DVID";
    case DRM_MODE_CONNECTOR_DVIA:        return "DVIA";
    case DRM_MODE_CONNECTOR_Composite:   return "COMPOSITE";
    case DRM_MODE_CONNECTOR_SVIDEO:      return "SVIDEO";
    case DRM_MODE_CONNECTOR_LVDS:        return "LVDS";
    case DRM_MODE_CONNECTOR_Component:   return "COMPONENT";
    case DRM_MODE_CONNECTOR_9PinDIN:     return "9PinDIN";
    case DRM_MODE_CONNECTOR_DisplayPort: return "DisplayPort";
    case DRM_MODE_CONNECTOR_HDMIA:       return "HDMIA";
    case DRM_MODE_CONNECTOR_HDMIB:       return "HDMIB";
    case DRM_MODE_CONNECTOR_TV:          return "TV";
    case DRM_MODE_CONNECTOR_eDP:         return "eDP";
    case DRM_MODE_CONNECTOR_VIRTUAL:     return "VIRTUAL";
    case DRM_MODE_CONNECTOR_DSI:         return "DSI";
    default: return "???";
    }
}

static char *
get_connector_status (int type)
{
    switch (type)
    {
    case  1: return "CONNECTED";    
    case  2: return "DISCONNECTED";
    default: return "???";
    }
}

static char *
get_connector_subpixel (int type)
{
    switch (type)
    {
    case  1: return "SUBPIXEL_UNKNOWN";    
    case  2: return "SUBPIXEL_HORIZONTAL_RGB";
    case  3: return "SUBPIXEL_HORIZONTAL_BGR";
    case  4: return "SUBPIXEL_VERTICAL_RGB";
    case  5: return "SUBPIXEL_VERTICAL_BGR";
    case  6: return "SUBPIXEL_NONE";
    default: return "SUBPIXEL_???";
    }
}

static char*
get_encoder_type (int type)
{
    switch (type)
    {
    case  0: return "NONE ";
    case  1: return "DAC  ";
    case  2: return "TMDS ";
    case  3: return "LVDS ";
    case  4: return "TVDAC";
    case  5: return "VIRTUAL";
    case  6: return "DSI  ";
    case  7: return "DPMST";
    default: return "???";
    }    
}


static void
dump_blob(int fd, uint32_t blob_id)
{
    uint32_t i;
    unsigned char *blob_data;
    drmModePropertyBlobPtr blob;

    fprintf (stderr, "blob_id=%2d, ", blob_id);
    blob = drmModeGetPropertyBlob(fd, blob_id);

    if (blob == NULL)
    {
        fprintf (stderr, "blob is NULL\n");
        return;
    }
    blob_data = blob->data;
    fprintf (stderr, "length=%d", blob->length);

    if (blob_data)
    {
        for (i = 0; i < blob->length; i++) {
            if (i % 32 == 0)
                fprintf(stderr, "\n\t\t");
            fprintf(stderr, "%.2hhx", blob_data[i]);
        }
    }

    fprintf(stderr, "\n");

    drmModeFreePropertyBlob(blob);
}


static int 
dump_prop(int fd, int idx, uint32_t prop_id, uint64_t value)
{
    int i;
    drmModePropertyPtr prop;
    char strbuf[128];
    
    fprintf(stderr, "\t  (%2d) %2d", idx, prop_id);

    prop = drmModeGetProperty (fd, prop_id);
    if (!prop) 
    {
        fprintf(stderr, "\n");
        return 0;
    }

    sprintf(strbuf, "\"%s\"", prop->name);
    fprintf(stderr, " %-14s", strbuf);

    if ((prop->flags & DRM_MODE_PROP_BLOB) == 0)
        fprintf (stderr, "value=%4lu", value);
    
    fprintf(stderr, " (");
    if (prop->flags & DRM_MODE_PROP_PENDING)   fprintf(stderr, " pending");
    if (prop->flags & DRM_MODE_PROP_RANGE)     fprintf(stderr, " range");
    if (prop->flags & DRM_MODE_PROP_IMMUTABLE) fprintf(stderr, " immutable");
    if (prop->flags & DRM_MODE_PROP_ENUM)      fprintf(stderr, " enum");
    if (prop->flags & DRM_MODE_PROP_BLOB)      fprintf(stderr, " blob");
    fprintf(stderr, " ) ");

    if (prop->flags & DRM_MODE_PROP_RANGE) 
    {
        fprintf(stderr, "range:");
        for (i = 0; i < prop->count_values; i++)
            fprintf(stderr, " %lu,", prop->values[i]);
        fprintf(stderr, "\n");
    }

    if (prop->flags & DRM_MODE_PROP_ENUM) 
    {
        fprintf(stderr, "enums:");
        for (i = 0; i < prop->count_enums; i++)
            fprintf(stderr, " %s=%llu", prop->enums[i].name, prop->enums[i].value);
        fprintf(stderr, "\n");
    }

    if (prop->flags & DRM_MODE_PROP_BLOB) 
    {
        fprintf(stderr, "\n\t\tblobs:");
        for (i = 0; i < prop->count_blobs; i++)
            dump_blob(fd, prop->blob_ids[i]);

        dump_blob(fd, value);
    }

    if (!(prop->flags & (DRM_MODE_PROP_RANGE | DRM_MODE_PROP_ENUM | DRM_MODE_PROP_BLOB)))
    {
        fprintf (stderr, "\n");
    }
    
    drmModeFreeProperty(prop);

    return 0;
}


static int
dump_drm_version (int fd)
{
    char *dev_name = drmGetDeviceNameFromFd (fd);
    fprintf (stderr, "  device name: %s\n", dev_name);
    drmFree (dev_name);

    char *bus_id = drmGetBusid (fd);
    fprintf (stderr, "  bus ID     : %s\n", bus_id);
    drmFreeBusid (bus_id);

    drmVersionPtr version = drmGetVersion (fd);
    if (version == NULL)
        return -1;

    fprintf (stderr, "  version    : %d.%d.%d\n", version->version_major,
                    version->version_minor, version->version_patchlevel);
    fprintf (stderr, "  name       : %s\n", version->name);
    fprintf (stderr, "  date       : %s\n", version->date);
    fprintf (stderr, "  desc       : %s\n", version->desc);

    drmFreeVersion (version);

    return 0;
}


static int
dump_drm_res (int fd)
{
    int i, j;
    drmModeRes      *drmRes;

    fprintf (stderr, "  -------------------------------------\n");

    drmRes = drmModeGetResources (fd);
    if (drmRes == 0)
    {
        fprintf (stderr, "ERR: %s(%d)\n", __FILE__, __LINE__);
        return 0;
    }    
    
    fprintf (stderr, "  -connectors      : num=%d", drmRes->count_connectors);
    for (i = 0; i < drmRes->count_connectors; i ++)
        fprintf (stderr, " [%d]%d", i, drmRes->connectors[i]);
    fprintf (stderr, "\n");

    fprintf (stderr, "  -encoders        : num=%d", drmRes->count_encoders);
    for (i = 0; i < drmRes->count_encoders; i ++)
        fprintf (stderr, " [%d]%d", i, drmRes->encoders[i]);
    fprintf (stderr, "\n");

    fprintf (stderr, "  -crtcs           : num=%d", drmRes->count_crtcs);
    for (i = 0; i < drmRes->count_crtcs; i ++)
        fprintf (stderr, " [%d]%d", i, drmRes->crtcs[i]);
    fprintf (stderr, "\n");

    fprintf (stderr, "  -fbs             : num=%d", drmRes->count_fbs);
    for (i = 0; i < drmRes->count_fbs; i ++)
        fprintf (stderr, " [%d]%d", i, drmRes->fbs[i]);
    fprintf (stderr, "\n");
    
    fprintf (stderr, "  (min_w,h,max_w,h): (%d, %d)(%d, %d)\n", 
            drmRes->min_width, drmRes->min_height,
            drmRes->max_width, drmRes->max_height);


    /* dump CONNECTOR */
    fprintf (stderr, "  ------------- CONNECTOR -------------\n");
    for (i = 0; i < drmRes->count_connectors; i ++)
    {
        drmModeConnector *drmConn = drmModeGetConnector (fd, drmRes->connectors[i]);
        
        fprintf (stderr, "  [%d/%d] id=%d, encoder=%d, WH(%d,%d)[mm], %s, %s:%d, %s\n", 
                                i, drmRes->count_connectors,
                                drmConn->connector_id, 
                                drmConn->encoder_id,
                                drmConn->mmWidth, drmConn->mmHeight,
                                get_connector_subpixel(drmConn->subpixel),
                                get_connector_type(drmConn->connector_type), 
                                drmConn->connector_type_id,
                                get_connector_status(drmConn->connection));

        fprintf (stderr, "        *count_encoders=%d\n", drmConn->count_encoders);
        fprintf (stderr, "         ");
        for (j = 0; j < drmConn->count_encoders; j ++)
        {
            unsigned int *enc = drmConn->encoders + j;
            fprintf (stderr, " (%2d) %d ", j, *enc);
        }
        fprintf (stderr, "\n");

        fprintf (stderr, "        *count_modes=%d\n", drmConn->count_modes);
        fprintf (stderr, "         ");
        for (j = 0; j < drmConn->count_modes; j ++)
        {
            drmModeModeInfo *mode = drmConn->modes + j;
            if (mode->type & DRM_MODE_TYPE_PREFERRED)
                fprintf (stderr, "<(%2d) %4dx%4d@%d>", j, mode->hdisplay, mode->vdisplay, mode->vrefresh);
            else
                fprintf (stderr, " (%2d) %4dx%4d@%d ", j, mode->hdisplay, mode->vdisplay, mode->vrefresh);
            if (j % 4 == 3)
            {
                fprintf (stderr, "\n         ");
            }
        }
        fprintf (stderr, "\n");

        fprintf (stderr, "        *count_props=%d\n", drmConn->count_props);
        for (j = 0; j < drmConn->count_props; j ++)
        {
            dump_prop (fd, j, drmConn->props[j], drmConn->prop_values[j]);
        }
              
        drmModeFreeConnector (drmConn);
     }


    /* dump ENCODER */
    fprintf (stderr, "  ------------- ENCODER -------------\n");
    for (i = 0; i < drmRes->count_encoders; i ++)
    {
        drmModeEncoder *drmEnc = drmModeGetEncoder(fd, drmRes->encoders[i]);
        if (drmEnc == NULL)
            continue;

        fprintf (stderr, "  [%d/%d] id=%2d, type=%d:%s, crtc_id=%2d, possible_crtcs=%3d, possible_clones=%d\n", 
                                i, drmRes->count_encoders, 
                                drmEnc->encoder_id,
                                drmEnc->encoder_type,
                                get_encoder_type (drmEnc->encoder_type),
                                drmEnc->crtc_id,
                                drmEnc->possible_crtcs,
                                drmEnc->possible_clones);
    
        drmModeFreeEncoder (drmEnc);
    }

    /* dump CRTC */
    fprintf (stderr, "  ------------- CRTC -------------\n");
    for (i = 0; i < drmRes->count_crtcs; i ++)
    {
        drmModeCrtc *drmCrtc = drmModeGetCrtc(fd, drmRes->crtcs[i]);
        if (drmCrtc == NULL)
            continue;

        fprintf (stderr, "  [%d/%d] id=%2d, FB=%2d, xywh(%d,%d,%4d,%4d) mode(%d)%4dx%4d@%2d, gamma_size=%d\n", 
                                i, drmRes->count_crtcs, 
                                drmCrtc->crtc_id, drmCrtc->buffer_id,
                                drmCrtc->x, drmCrtc->y, drmCrtc->width, drmCrtc->height,
                                drmCrtc->mode_valid,
                                drmCrtc->mode.hdisplay,drmCrtc->mode.vdisplay, drmCrtc->mode.vrefresh,
                                drmCrtc->gamma_size);

        drmModeFreeCrtc (drmCrtc);

        /* Object Properties */
        drmModeObjectPropertiesPtr drmProp;
        drmProp = drmModeObjectGetProperties(fd, drmRes->crtcs[i], DRM_MODE_OBJECT_CRTC);
        if (drmProp == NULL)
            continue;

        for (j = 0; j < drmProp->count_props; j ++)
        {
            dump_prop (fd, j, drmProp->props[j], drmProp->prop_values[j]);
        }
        drmModeFreeObjectProperties (drmProp);
    }

    drmModeFreeResources (drmRes);
    
    return 0;
}

static int
dump_drm_planes (int fd)
{
    int i, j;
    drmModePlaneRes *drmPlanes;

    drmPlanes = drmModeGetPlaneResources(fd);
    if (drmPlanes == 0)
    {
        fprintf (stderr, "ERR: %s(%d)\n", __FILE__, __LINE__);
        return 0;
    }        
    
    fprintf (stderr, "  ------------- PLANE -------------\n");
    for (i = 0; i < (int)drmPlanes->count_planes; i ++)
    {
        drmModePlane *plane = drmModeGetPlane(fd, drmPlanes->planes[i]);

        fprintf (stderr, "  [%d/%d] id=%2d, crtc_id=%d, FB=%2d, xy(%d,%d), crtc_xy(%d,%d), gamma_size=%d,  possible_crtcs=%d\n", 
                            i, drmPlanes->count_planes, 
                            plane->plane_id, plane->crtc_id, plane->fb_id,
                            plane->x, plane->y,
                            plane->crtc_x, plane->crtc_y,
                            plane->gamma_size,
                            plane->possible_crtcs);
        fprintf (stderr, "        *count_formats=%d\n", plane->count_formats);
        
        /* Object Properties */
        drmModeObjectPropertiesPtr drmProp;
        drmProp = drmModeObjectGetProperties(fd, plane->plane_id, DRM_MODE_OBJECT_PLANE);
        if (drmProp == NULL)
            continue;

        for (j = 0; j < drmProp->count_props; j ++)
        {
            dump_prop (fd, j, drmProp->props[j], drmProp->prop_values[j]);
        }
        drmModeFreeObjectProperties (drmProp);
        drmModeFreePlane (plane);
    }
    
    drmModeFreePlaneResources (drmPlanes);

    return 0;
}

static void
_dump_drm_cap (int fd, uint64_t cap_id, char *cap_name)
{
    uint64_t cap;
    int ret;

    ret = drmGetCap (fd, cap_id, &cap);
    if (ret == 0)
        fprintf (stderr, "   %-31s:  %lu\n", cap_name, cap);
    else
        fprintf (stderr, "   %-31s: # ERROR #\n", cap_name);
}

static int
dump_drm_caps (int fd)
{
    uint64_t cap;
    int ret;
    
    fprintf (stderr, "  ----------------- CAPS --------------\n");
    
    _dump_drm_cap (fd, DRM_CAP_DUMB_BUFFER,          "DRM_CAP_DUMB_BUFFER");
    _dump_drm_cap (fd, DRM_CAP_VBLANK_HIGH_CRTC,     "DRM_CAP_VBLANK_HIGH_CRTC");
    _dump_drm_cap (fd, DRM_CAP_DUMB_PREFERRED_DEPTH, "DRM_CAP_DUMB_PREFERRED_DEPTH");
    _dump_drm_cap (fd, DRM_CAP_DUMB_PREFER_SHADOW,   "DRM_CAP_DUMB_PREFER_SHADOW");

    ret = drmGetCap (fd, DRM_CAP_PRIME, &cap);
    if (ret == 0)
    {
        fprintf (stderr, "   DRM_CAP_PRIME                  :  %lu  ", cap);
        if (cap & DRM_PRIME_CAP_IMPORT) fprintf (stderr, "DRM_PRIME_CAP_IMPORT/");
        if (cap & DRM_PRIME_CAP_EXPORT) fprintf (stderr, "DRM_PRIME_CAP_EXPORT ");
        fprintf (stderr, "\n");
    }
    else
    {
        fprintf (stderr, "   DRM_CAP_PRIME                  : # ERROR #\n");
    }

    _dump_drm_cap (fd, DRM_CAP_TIMESTAMP_MONOTONIC,  "DRM_CAP_TIMESTAMP_MONOTONIC");
    _dump_drm_cap (fd, DRM_CAP_ASYNC_PAGE_FLIP,      "DRM_CAP_ASYNC_PAGE_FLIP");

#if defined(DRM_CAP_CURSOR_WIDTH)
    _dump_drm_cap (fd, DRM_CAP_CURSOR_WIDTH,         "DRM_CAP_CURSOR_WIDTH");
#else
    fprintf (stderr, "   DRM_CAP_CURSOR_WIDTH           : not support\n");
#endif

#if defined(DRM_CAP_CURSOR_HEIGHT)
    _dump_drm_cap (fd, DRM_CAP_CURSOR_HEIGHT,        "DRM_CAP_CURSOR_HEIGHT");
#else
    fprintf (stderr, "   DRM_CAP_CURSOR_HEIGHT          : not support\n");
#endif

#if defined(DRM_CAP_PAGE_FLIP_TARGET)
    _dump_drm_cap (fd, DRM_CAP_ADDFB2_MODIFIERS,     "DRM_CAP_ADDFB2_MODIFIERS");
#else
    fprintf (stderr, "   DRM_CAP_PAGE_FLIP_TARGET       : not support\n");
#endif

    
#if defined(DRM_CAP_PAGE_FLIP_TARGET)
    _dump_drm_cap (fd, DRM_CAP_PAGE_FLIP_TARGET,     "DRM_CAP_PAGE_FLIP_TARGET");
#else
    fprintf (stderr, "   DRM_CAP_PAGE_FLIP_TARGET       : not support\n");
#endif

#if defined(DRM_CAP_CRTC_IN_VBLANK_EVENT)
    _dump_drm_cap (fd, DRM_CAP_CRTC_IN_VBLANK_EVENT, "DRM_CAP_CRTC_IN_VBLANK_EVENT");
#else
    fprintf (stderr, "   DRM_CAP_CRTC_IN_VBLANK_EVENT   : not support\n");
#endif

#if defined(DRM_CAP_SYNCOBJ)
    _dump_drm_cap (fd, DRM_CAP_SYNCOBJ,              "DRM_CAP_SYNCOBJ");
#else
    fprintf (stderr, "   DRM_CAP_SYNCOBJ                : not support\n");
#endif

#if defined(DRM_CAP_SYNCOBJ_TIMELINE)
    _dump_drm_cap (fd, DRM_CAP_SYNCOBJ_TIMELINE,     "DRM_CAP_SYNCOBJ_TIMELINE");
#else
    fprintf (stderr, "   DRM_CAP_SYNCOBJ_TIMELINE       : not support\n");
#endif

    return 0;
}


static int
dump_drm_client_caps (int fd)
{
    int ret;

    fprintf (stderr, "  ------------- CLIENT CAPS -----------\n");

    ret = drmSetClientCap (fd, DRM_CLIENT_CAP_ATOMIC, 1);
    if (ret == 0)
        fprintf (stderr, "   DRM_CLIENT_CAP_ATOMIC          : SUCCESS\n");
    else
        fprintf (stderr, "   DRM_CLIENT_CAP_ATOMIC          : ERROR\n");


    ret = drmSetClientCap (fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES , 1);
    if (ret == 0)
        fprintf (stderr, "   DRM_CLIENT_CAP_UNIVERSAL_PLANES: SUCCESS\n");
    else
        fprintf (stderr, "   DRM_CLIENT_CAP_UNIVERSAL_PLANES: ERROR\n");

    return 0;
}


#if defined (USE_LIBUDEV)
#include <libudev.h>
static const char default_seat[] = "seat0";

/*
 * Find primary GPU
 * Some systems may have multiple DRM devices attached to a single seat. This
 * function loops over all devices and tries to find a PCI device with the
 * boot_vga sysfs attribute set to 1.
 * If no such device is found, the first DRM device reported by udev is used.
 */
static struct udev_device*
find_primary_gpu (struct udev *udev, const char *seat)
{
    struct udev_enumerate *e;
    struct udev_list_entry *entry;
    const char *path, *device_seat, *id;
    struct udev_device *device, *drm_device, *pci;

    e = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(e, "drm");
    udev_enumerate_add_match_sysname(e, "card[0-9]*");

    udev_enumerate_scan_devices(e);
    drm_device = NULL;
    udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(e)) 
    {
        path = udev_list_entry_get_name(entry);
        fprintf (stderr, "(udev) PATH: %s\n", path);

        device = udev_device_new_from_syspath(udev, path);
        if (!device)
            continue;

        device_seat = udev_device_get_property_value(device, "ID_SEAT");
        fprintf (stderr, "(udev) SEAT: %s\n", device_seat);
        if (!device_seat)
            device_seat = default_seat;
        if (strcmp(device_seat, seat)) 
        {
            udev_device_unref(device);
            continue;
        }

        pci = udev_device_get_parent_with_subsystem_devtype(device, "pci", NULL);
        if (pci) 
        {
            id = udev_device_get_sysattr_value(pci, "boot_vga");
            fprintf (stderr, "ID  : %s\n", device_seat);
            if (id && !strcmp(id, "1")) 
            {
                if (drm_device)
                    udev_device_unref(drm_device);
                drm_device = device;
                break;
            }
        }

        if (!drm_device)
            drm_device = device;
        else
            udev_device_unref(device);
    }

    udev_enumerate_unref(e);

    return drm_device;
}

int
open_drm ()
{
    struct udev *udev;
    struct udev_device *drm_device;
    const char *syspath, *sysnum, *devnode;
    int drm_id = -1;
    int fd;

    fprintf (stderr, "-------------------------------------\n");
    fprintf (stderr, " open drm: udev\n");
    fprintf (stderr, "-------------------------------------\n");

    udev = udev_new();
    if (udev == NULL)
    {
        fprintf (stderr, "%s(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    drm_device = find_primary_gpu (udev, default_seat);
    if (drm_device == NULL)
    {
        fprintf (stderr, "%s(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    syspath = udev_device_get_syspath (drm_device);
    sysnum  = udev_device_get_sysnum (drm_device);
    devnode = udev_device_get_devnode (drm_device);
    if (drm_device == NULL || sysnum == NULL || devnode == NULL)
    {
        fprintf (stderr, "%s(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    drm_id = atoi (sysnum);
    if (drm_id < 0) 
    {
        fprintf (stderr, "%s(%d)\n", __FILE__, __LINE__);
        return -1;
    }

    fprintf (stderr, "(udev) SYSPATH:%s\n", syspath);
    fprintf (stderr, "(udev) SYSNUM :%s\n", sysnum);
    fprintf (stderr, "(udev) drm_id :%d\n", drm_id);
    fprintf (stderr, "(udev) DEVNODE:%s\n", devnode);

    fd = open (devnode, O_RDWR | O_CLOEXEC);
    if (fd == -1)
        fprintf (stderr, "ERR: %s(%d): can't open DRM.\n", __FILE__, __LINE__);

    return fd;
}

#else /* #if defined (USE_LIBUDEV) */

int
open_drm ()
{
    fprintf (stderr, "-------------------------------------\n");
#if defined (DRM_DRIVER_NAME)
    fprintf (stderr, " open drm: drmOpen(%s)\n", DRM_DRIVER_NAME);
    int fd = drmOpen (DRM_DRIVER_NAME, NULL);
#else
    fprintf (stderr, " open drm: open(/dev/dri/card0)\n");
    int fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
#endif
    fprintf (stderr, "-------------------------------------\n");

    if (fd == -1)
        fprintf (stderr, "ERR: %s(%d): can't open DRM.\n", __FILE__, __LINE__);

    return fd;
}

#endif /* #if defined (USE_LIBUDEV) */ 



int
main (int argc, char *argv[])
{
    int drm_fd = open_drm ();
    if (drm_fd < 0)
        return -1;

    dump_drm_version (drm_fd);
    dump_drm_client_caps (drm_fd);
    dump_drm_caps (drm_fd);

    dump_drm_res (drm_fd);
    dump_drm_planes (drm_fd);

    return 0;
}
