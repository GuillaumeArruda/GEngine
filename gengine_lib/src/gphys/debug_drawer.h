#pragma once

#include <bullet/LinearMath/btIDebugDraw.h>

namespace grender
{
    struct debug_render_system;
}

namespace gphys
{
    struct debug_drawer 
        : btIDebugDraw
    {
        
        void reportErrorWarning(const char* warningString) override;
        void setDebugMode(int debugMode) override { m_debug_mode = debugMode; }
        int  getDebugMode() const override { return m_debug_mode; }
        void draw3dText(const btVector3& location, const char* textString) override {}
        void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}

        void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
        
        grender::debug_render_system* m_debug_render = nullptr;
    private:
        int m_debug_mode = DBG_DrawWireframe;
    };
}