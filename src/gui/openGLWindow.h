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

#ifndef SAMPLER_OPENGLWINDOW_H
#define SAMPLER_OPENGLWINDOW_H

#include <memory>
#include <queue>
#include "fbo.h"

#include <Geometry/geometry.h>
#include "Transformation/arcball.h"
#include "renderUnit.h"

/**
 * \class FBO
 *
 * \brief FBO extention. Has methods to receive geometries
 * from external classes/function and send them to the renderer.
 */
class FBO : public FrameBufferObject
{
    Q_OBJECT

public:
    /**
     * Constructor
     * @param parent parental element
     */
    explicit FBO(QQuickItem *parent = 0);

    /**
     * Creates the FBO Renderer
     * @return
     */
    Renderer *createRenderer() const Q_DECL_OVERRIDE;

    /**
     * Weather a clear all geometries was requested
     * @return
     */
    bool clearRequested();

    /**
     * Weather new geometries exist, that need to be
     * added to the renderer
     * @return
     */
    bool newGeometries();

    /**
     * Returns and removes the first element of the
     * new geometry queue for elements that need to
     * be sent to the renderer
     * @return Pointer to geometry element
     */
    std::shared_ptr<Geometry> getGeometry();

public slots:
    /**
     * Add a geometry to the queue of geometries,
     * that will be sent to the renderer
     * @param geom geometry pointer
     */
    void receiveGeometry(std::shared_ptr<Geometry> geom);

    /**
     * Request to clear all geometries from the renderer
     */
    void clearGeometries();

    /**
     * Reset camera to initial transformation
     */
    void resetCamera();

    /**
     * Set the center of the view aka rotation center of the arcball
     * @param center
     */
    void setCenter(const QVector3D &center);

public:
    // Queue of new geometries that need to be sent to the renderer
    std::queue<std::shared_ptr<Geometry>> m_geometries;
    // Weather clear all geometries from the renderer was requested
    bool m_requestClear;
    // Pointer to camera transform in capsulated renderer class
    mutable Arcball* m_arcball;
    // Pointer to capsulated renderer class
    mutable RenderUnit* m_render;
};


#endif //SAMPLER_OPENGLWINDOW_H
