/******************************************************
 *
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %## #    CC        V    V  MM M  M MM RR    RR
 *   ,%      %    CC        VV  VV  MM  MM  MM RRRRRR
 *   (%      %,   CC    CC   VVVV   MM      MM RR   RR
 *     #%    %*    CCCCCC     VV    MM      MM RR    RR
 *    .%    %/
 *       (%.      Computer Vision & Mixed Reality Group
 *
 *****************************************************/
/** @copyright:   Hochschule RheinMain,
 *                University of Applied Sciences
 *     @author:   Alex Sommer
 *    @version:   1.0
 *       @date:   19.11.20
 *****************************************************/

#include "openGLWindow.h"

#include "renderUnit.h"
#include "fboRenderer.h"

#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>

class FBORenderer : public FrameBufferObjectRenderer
{
public:
    FBORenderer()
    {
        m_render.setBgColor(QVector4D(84.0f/255.0f,80.0f/255.0f,73.0f/255.0f,1.0f));
    }

    void addSync(QQuickFramebufferObject *item) Q_DECL_OVERRIDE
    {
        FBO *i = static_cast<FBO *>(item);
        if(i->clearRequested()) {
            m_render.clearGeometries();
        }
        while(i->newGeometries()) {
            m_render.addGeometry(i->getGeometry());
        }
        for (int j = 0; j < m_render.geometryCount(); j++) {
            m_render.getGeometry(j)->performChanges();
        }
    }

    RenderUnit* passRenderUnit() {
        return &m_render;
    }
};

FBO::FBO(QQuickItem *parent)
        : FrameBufferObject(parent)
{
}

/******************************************************
 * Public Functions
 *****************************************************/

QQuickFramebufferObject::Renderer *FBO::createRenderer() const
{
    auto* FBOr = new FBORenderer;
    m_render = FBOr->passRenderUnit();
    m_arcball = &m_render->getArcballReference();
    m_arcball->setParameter({0,0.03,0}, {1,0,0,0}, 0.78);
    return FBOr;
}

bool FBO::clearRequested() {
    if(m_requestClear) {
        m_requestClear = false;
        return true;
    }
    return false;
}

bool FBO::newGeometries() {
    return !m_geometries.empty();
}

std::shared_ptr<Geometry> FBO::getGeometry() {
    std::shared_ptr<Geometry> geo = m_geometries.front();
    m_geometries.pop();
    return geo;
}

/******************************************************
 * Signals
 *****************************************************/

void FBO::receiveGeometry(std::shared_ptr<Geometry> geom) {
    m_geometries.push(geom);
}

void FBO::clearGeometries() {
    m_requestClear = true;
}

void FBO::resetCamera() {
    m_arcball->reset();
}

void FBO::setCenter(const QVector3D &center) {
    m_render->setCenter(center);
}


