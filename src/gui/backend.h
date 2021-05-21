/******************************************************
 *
 *   #, #,         CCCCCC  VV    VV MM      MM RRRRRRR
 *  %  %(  #%%#   CC    CC VV    VV MMM    MMM RR    RR
 *  %    %#  #    CC        V    V  MM M  M MM RR    RR
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

#ifndef SAMPLER_BACKEND_H
#define SAMPLER_BACKEND_H

#include <QObject>
#include "settings.h"
#include "Geometry/grid.h"
#include "Geometry/spheres.h"
#include "Geometry/mesh.h"
#include "memory"
#include "typedef.h"

/**
 * \class Backend
 * \brief Backend for the Qt app
 */
class Backend: public QObject {
Q_OBJECT
    Q_PROPERTY(bool sampled READ isSampled WRITE setSampled NOTIFY sampleChanged);
    Q_PROPERTY(bool meshLoaded READ meshLoaded WRITE setMeshLoaded NOTIFY meshLoadedChanged);
    Q_PROPERTY(bool idle READ idle WRITE setIdle NOTIFY idleChanged);
    Q_PROPERTY(int samples READ samples);
    Q_PROPERTY(Settings* settings READ settings);
public:
    /**
     * Constructor
     * @param parent parental item
     */
    explicit Backend(QObject *parent = nullptr);

    /**
     * Deconstructor
     */
    ~Backend() noexcept = default;

    /**
     * Load a mesh file
     * @param filePath
     */
    Q_INVOKABLE void loadFile(QString filePath);

    /**
     * Saves a samling
     * @param filePath
     */
    Q_INVOKABLE void save(QString filePath);

    /**
     * Reloads the mesh, when scaling has been changed
     */
    Q_INVOKABLE void reloadMesh();

    /**
     * Performs the volume sampling
     */
    Q_INVOKABLE void volumeSample();

    /**
     * Performs the surface sampling
     */
    Q_INVOKABLE void surfaceSample();

    bool isSampled() const {
        return m_isSampled;
    }

    void setSampled(const bool &sampled) {
        if(sampled != m_isSampled) {
            m_isSampled = sampled;
            emit sampleChanged();
        }
    }

    bool meshLoaded() const {
        return m_meshLoaded;
    }

    void setMeshLoaded(const bool &loaded) {
        if(loaded != m_meshLoaded) {
            m_meshLoaded = loaded;
            emit meshLoadedChanged();
        }
    }

    bool idle() const {
        return m_idle;
    }

    void setIdle(const bool &idle) {
        if(idle != m_idle) {
            m_idle = idle;
            emit idleChanged();
        }
    }

    int samples() const {
        return m_sampling.size();
    }

    Settings* settings() const {
        return m_settings;
    }
signals:
    void addGeometry(std::shared_ptr<Geometry> geom);
    void callUpate();
    void setViewCenter(const QVector3D &center);
    void sampleChanged();
    void meshLoadedChanged();
    void idleChanged();
protected:
    void initShaders();
    void loadMesh();
    static scalar computeScaling(const Eigen::AlignedBox<scalar,3> &bbox);
    static Vector3 computeTranslation(const Eigen::AlignedBox<scalar,3> &bbox, const Vector3 &scaling);
    void setView();
    void writeSettingsToString(const bool &volume);

protected:
    // Drawable geometries
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<SphereCloud> m_particles;
    std::shared_ptr<Grid> m_grid;
    // Controllable parameters
    Settings* m_settings;
    QString m_settingsString;
    // Mesh file
    QString m_file;
    // Mesh entities
    Matrix3X m_vertices;
    Matrix3X m_normals;
    Indices m_faces;
    // Particle sampling
    std::vector<Vector3> m_sampling;
#if USE_DOUBLE
    std::vector<Eigen::Matrix<float, 3, 1>> m_samplesForRendering;
#endif
    // Application control
    bool m_isSampled;
    bool m_meshLoaded;
    bool m_idle;
};


#endif //SAMPLER_BACKEND_H
