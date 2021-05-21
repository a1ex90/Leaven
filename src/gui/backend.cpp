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

#include "backend.h"
#include "volumeSampler.h"
#include "surfaceSampler.h"
#include "common.h"
#include "helpers/OBJLoader.h"
#include <QDebug>
#include <QFile>

/******************************************************
 * Constructors
 *****************************************************/

Backend::Backend(QObject *parent) :
        m_mesh(nullptr),
        m_particles(nullptr),
        m_grid(nullptr),
        m_file(),
        m_isSampled(false),
        m_meshLoaded(false),
        m_idle(false),
        QObject(parent),
        m_settings(new Settings(this)),
        m_settingsString(){
}

/******************************************************
 * Public Functions
 *****************************************************/

void Backend::loadFile(QString filePath) {
    // disable save button
    setSampled(false);
    // disable sample button
    setMeshLoaded(false);
    emit this->meshLoadedChanged();
    // Normalize Filepath
    if(filePath.length() < 7) {
        return;
    }
    // remove "File://" prefix
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    filePath = filePath.remove(0, 8);
#else
    filePath = filePath.remove(0, 6);
#endif
    initShaders();

    m_file = filePath;

    loadMesh();
    setView();

    // enable sample button
    setMeshLoaded(true);

    callUpate();
}

void Backend::save(QString filePath) {
    // Normalize Filepath
    if(filePath.length() < 7) {
        return;
    }
    // remove "File://" prefix
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    filePath = filePath.remove(0, 8);
#else
    filePath = filePath.remove(0, 6);
#endif
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "couldn't open file";
        return;
    }
    QTextStream out(&file);
    out << "ply\n";
    out << "format ascii 1.0\n";
    out << "comment generated with LEAVEN 1.0\n";
    out << m_settingsString << "\n";
    out << "element vertex " << m_sampling.size() << "\n";
#if USE_DOUBLE
    out << "property float64 x\n";
    out << "property float64 y\n";
    out << "property float64 z\n";
#else
    out << "property float32 x\n";
    out << "property float32 y\n";
    out << "property float32 z\n";
#endif
    out << "end_header\n";
    for(auto const &vector : m_sampling) {
        out << vector.x() << " " << vector.y() << " " << vector.z() << "\n";
    }
}

void Backend::reloadMesh() {
    if(m_file != "") {
        if(m_particles != nullptr)
            m_particles->flush();
        loadMesh();
        setView();
    }
}

void Backend::volumeSample() {
    // disable save button
    setSampled(false);
//    setIdle(true);
    if(m_mesh != nullptr)
        m_mesh->flush();
    if(m_file != "") {
        scalar cellSize = static_cast<scalar>(2.0) * m_settings->radius();
        if(m_settings->vMode()) {
            m_sampling = VolumeSampler::sampleMeshRandom(m_vertices, m_faces, m_settings->radius(), m_settings->vTrials(), m_settings->vDensity(), m_settings->vInvert(), m_settings->sdfResolution());
        } else {
            m_sampling = VolumeSampler::sampleMeshDense(m_vertices, m_faces, m_settings->radius(), cellSize, -1,
                                                        m_settings->vInvert(), m_settings->sdfResolution());
        }
        if(m_particles != nullptr) {
#if USE_DOUBLE
            m_particles->setPointSize(static_cast<float>(m_settings->radius()));
            m_samplesForRendering.resize(m_sampling.size());
            for(int i = 0; i < m_sampling.size(); i++)
                m_samplesForRendering[i] = m_sampling[i].cast<float>();
            m_particles->changePoints(m_samplesForRendering[0].data(), m_samplesForRendering.size());
#else
            m_particles->setPointSize(static_cast<float>(m_settings->radius()));
            m_particles->changePoints(m_sampling[0].data(), m_sampling.size());
#endif
        }
    }
    qDebug() << m_sampling.size();
    // save current settings to string
    writeSettingsToString(true);
//    setIdle(false);
    setSampled(true);
}

void Backend::surfaceSample() {
    // disable save button
    setSampled(false);
//    setIdle(true);
    if(m_mesh != nullptr)
        m_mesh->flush();
    if(m_file != "") {
        m_sampling = SurfaceSampler::sampleMesh(m_vertices, m_faces, m_settings->sMinDistance(), m_settings->sTrials(), m_settings->sDensity(), m_settings->norm());
        if(m_particles != nullptr) {
#if USE_DOUBLE
            m_particles->setPointSize(static_cast<float>(m_settings->radius()));m_samplesForRendering.resize(m_sampling.size());
            for(int i = 0; i < m_sampling.size(); i++)
                m_samplesForRendering[i] = m_sampling[i].cast<float>();
            m_particles->changePoints(m_samplesForRendering[0].data(), m_samplesForRendering.size());
#else
            m_particles->setPointSize(static_cast<float>(m_settings->radius()));
            m_particles->changePoints(m_sampling[0].data(), m_sampling.size());
#endif
        }
    }
    // save current settings to string
    writeSettingsToString(false);
//    setIdle(false);
    setSampled(true);
}

/******************************************************
 * Private Functions
 *****************************************************/

void Backend::initShaders() {
    const QVector3D mainColor = {155.0f/255.0f, 188.0f/255.0f, 238.0f/255.0f};
    if(m_grid == nullptr) {
        m_grid = std::make_shared<Grid>();
        m_grid->initialize("assets//shaders//gl3//checkerboard.frag", "assets//shaders//gl3//checkerboard.vert");
        m_grid->initGrid({0,0,0});
        emit this->addGeometry(m_grid);
    }

    if(m_mesh == nullptr) {
        m_mesh = std::make_shared<Mesh>();
        m_mesh->initialize("assets//shaders//gl3//phong.frag", "assets//shaders//gl3//phong.vert");
        m_mesh->setMaterial(mainColor, 0.2 * mainColor);
        emit this->addGeometry(m_mesh);
    } else {
        m_mesh->flush();
    }

    if(m_particles == nullptr) {
        m_particles = std::make_shared<SphereCloud>();
        m_particles->initialize("assets//shaders//gl3//spheres.frag", "assets//shaders//gl3//spheres.vert");
        m_particles->setColor(mainColor);
        emit this->addGeometry(m_particles);
    } else {
        m_particles->flush();
    }
}

void Backend::loadMesh() {
    if(m_file.endsWith(".obj")) {
        m_mesh->changeMesh(m_file);
        Vector3 scaling = m_settings->scaling();
        Vector3 translation = Vector3::Zero();
        Transform meshTransform;
        if(m_settings->meshNormalization()) {
            OBJLoader::loadObj(m_file.toStdString(), m_vertices, m_faces, m_normals);
            auto bbox = Common::computeBoundingBox(m_vertices);
            const scalar scaleFactor = computeScaling(bbox);
            for(uint i = 0; i < 3; i++) {
                scaling(i) *= scaleFactor;
            }
            translation = computeTranslation(bbox, scaling);
            meshTransform.setPos({static_cast<float>(translation.x()), static_cast<float>(translation.y()), static_cast<float>(translation.z())});
        }
        meshTransform.setScale({static_cast<float>(scaling.x()), static_cast<float>(scaling.y()), static_cast<float>(scaling.z())});
        OBJLoader::loadObj(m_file.toStdString(), m_vertices, m_faces, m_normals, scaling, translation);
        m_mesh->setTransform(meshTransform);
    } else {
        qDebug() << "file error:" << m_file;
    }
}

scalar Backend::computeScaling(const Eigen::AlignedBox<scalar, 3> &bbox) {
    const Vector3 diff = bbox.max() - bbox.min();
    const scalar factor = diff.maxCoeff();
    return static_cast<scalar>(1.0) / factor;
}

Vector3 Backend::computeTranslation(const Eigen::AlignedBox<scalar, 3> &bbox, const Vector3 &scaling) {
    const Vector3 trans = scaling.cwiseProduct(static_cast<scalar>(-1.0) * bbox.center());
    const scalar y = scaling.y() * (static_cast<scalar>(-1.0) * bbox.min().y());
    return { trans.x(), y, trans.z() };
}

void Backend::setView() {
    if(m_vertices.cols() > 0) {
        auto boundingbox = Common::computeBoundingBox(m_vertices);

        Vector3 center = boundingbox.center();
        Vector3 min = boundingbox.min();
        m_grid->initGrid({static_cast<float>(center.x()), static_cast<float>(min.y()), static_cast<float>(center.z())});
        emit setViewCenter({static_cast<float>(center.x()), static_cast<float>(center.y()), static_cast<float>(center.z())});
    }
}

void Backend::writeSettingsToString(const bool &volume) {
    m_settingsString = "";
    m_settingsString.append("comment Radius: ");
    m_settingsString.append(QString::number(m_settings->radius()));
    if(volume) {
        m_settingsString.append("\ncomment Mode: ");
        if(m_settings->vMode()) {
            m_settingsString.append("Random");
            m_settingsString.append("\ncomment Density: ");
            m_settingsString.append(QString::number(m_settings->vDensity()));
            m_settingsString.append("\ncomment Trials: ");
            m_settingsString.append(QString::number(m_settings->vTrials()));
        } else {
            m_settingsString.append("Dense");
        }
        m_settingsString.append("\ncomment SDF Resolution: ");
        m_settingsString.append(QString::number(m_settings->sdfX()));
        m_settingsString.append(" ");
        m_settingsString.append(QString::number(m_settings->sdfY()));
        m_settingsString.append(" ");
        m_settingsString.append(QString::number(m_settings->sdfZ()));
    } else {
        m_settingsString.append("\ncomment Minimum Distance: ");
        m_settingsString.append(QString::number(m_settings->sMinDistance()));
        m_settingsString.append("\ncomment Distance Norm: ");
        if(m_settings->norm())
            m_settingsString.append("Geodesic");
        else
            m_settingsString.append("Euclidean");
        m_settingsString.append("\ncomment Density: ");
        m_settingsString.append(QString::number(m_settings->sDensity()));
        m_settingsString.append("\ncomment Trials: ");
        m_settingsString.append(QString::number(m_settings->sTrials()));
    }
}