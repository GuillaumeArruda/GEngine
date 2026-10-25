#include "stdafx.h"
#include "grender/resources/frame_buffer_resource.h"
#include "grender/utils.h"

bool grender::frame_buffer_resource::do_load_async()
{
    return true;
}

bool grender::frame_buffer_resource::do_load_sync()
{
    return true;
}

void grender::frame_buffer_resource::do_unload()
{
    gl_exec(glDeleteFramebuffers, 1, &m_fbo);
}
