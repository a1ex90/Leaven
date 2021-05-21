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
 *       @date:   20.11.20
 *****************************************************/

#ifndef SAMPLER_SETTINGS_H
#define SAMPLER_SETTINGS_H

#include <QObject>
#include "typedef.h"
#include <array>

#include <iostream>

/**
 * \class Settings
 * \brief Holds all controllable parameters for the mesh sampler, that can be
 * set through the app
 */
class Settings : public QObject {
    Q_OBJECT
#ifdef USE_DOUBLE
    Q_PROPERTY(double radius READ radius WRITE setRadius)
    Q_PROPERTY(double sMinDistance READ sMinDistance WRITE setSMinDistance)
    Q_PROPERTY(double scaleX READ scaleX WRITE setScaleX)
    Q_PROPERTY(double scaleY READ scaleY WRITE setScaleY)
    Q_PROPERTY(double scaleZ READ scaleZ WRITE setScaleZ)
    Q_PROPERTY(double sDensity READ sDensity WRITE setSDensity)
    Q_PROPERTY(double vDensity READ vDensity WRITE setVDensity)
#else
    Q_PROPERTY(float radius READ radius WRITE setRadius)
    Q_PROPERTY(float sMinDistance READ sMinDistance WRITE setSMinDistance)
    Q_PROPERTY(float scaleX READ scaleX WRITE setScaleX)
    Q_PROPERTY(float scaleY READ scaleY WRITE setScaleY)
    Q_PROPERTY(float scaleZ READ scaleZ WRITE setScaleZ)
    Q_PROPERTY(float sDensity READ sDensity WRITE setSDensity)
    Q_PROPERTY(float vDensity READ vDensity WRITE setVDensity)
#endif
    Q_PROPERTY(bool meshNormalization READ meshNormalization WRITE setMeshNormalization)
    Q_PROPERTY(bool vInvert READ vInvert WRITE setVInvert)
    Q_PROPERTY(int sdfX READ sdfX WRITE setSdfX)
    Q_PROPERTY(int sdfY READ sdfY WRITE setSdfY)
    Q_PROPERTY(int sdfZ READ sdfZ WRITE setSdfZ)
    Q_PROPERTY(unsigned int vMode READ vMode WRITE setVMode)
    Q_PROPERTY(int vMaxSamples READ vMaxSamples WRITE setVMaxSamples)
    Q_PROPERTY(unsigned int norm READ norm WRITE setNorm)
    Q_PROPERTY(unsigned int sTrials READ sTrials WRITE setSTrials)
    Q_PROPERTY(unsigned int vTrials READ vTrials WRITE setVTrials)

public:
    Settings(QObject *parent)
        : QObject(parent)
        , m_radius(0.02)
        , m_meshNormalization(true)
        , m_scaleX(1.0)
        , m_scaleY(1.0)
        , m_scaleZ(1.0)
        , m_vInvert(false)
        , m_sdfResolution({20,20,20})
        , m_vMode(0)
        , m_vMaxSamples(-1)
        , m_norm(1)
        , m_sTrials(10)
        , m_vTrials(10)
        , m_sInitialDensity(40.0)
        , m_vInitialDensity(10.0)
        , m_sMinDistance(0.02)
        {}

    scalar radius() const {
        return m_radius;
    }

    void setRadius(const scalar &radius) {
        if(radius != m_radius) {
            m_radius = radius;
        }
    }

    bool meshNormalization() const {
        return m_meshNormalization;
    }

    void setMeshNormalization(const bool &meshNormalization) {
        if(meshNormalization != m_meshNormalization) {
            m_meshNormalization = meshNormalization;
        }
    }

    Vector3 scaling() const {
        return {m_scaleX, m_scaleY, m_scaleZ};
    }

    scalar scaleX() const {
        return m_scaleX;
    }

    void setScaleX(const scalar &x) {
        if(x != m_scaleX) {
            m_scaleX = x;
        }
    }

    scalar scaleY() const {
        return m_scaleY;
    }

    void setScaleY(const scalar &y) {
        if(y != m_scaleY) {
            m_scaleY = y;
        }
    }

    scalar scaleZ() const {
        return m_scaleZ;
    }

    void setScaleZ(const scalar &z) {
        if(z != m_scaleZ) {
            m_scaleZ = z;
        }
    }

    bool vInvert() const {
        return m_vInvert;
    }

    void setVInvert(const bool &invert) {
        if(invert != m_vInvert) {
            m_vInvert = invert;
        }
    }

    int sdfX() const {
        return m_sdfResolution[0];
    }

    void setSdfX(const int &x) {
        if(m_sdfResolution[0] != x) {
            m_sdfResolution[0] = x;
        }
    }

    int sdfY() const {
        return m_sdfResolution[1];
    }

    void setSdfY(const int &y) {
        if(m_sdfResolution[1] != y) {
            m_sdfResolution[1] = y;
        }
    }

    int sdfZ() const {
        return m_sdfResolution[2];
    }

    void setSdfZ(const int &z) {
        if(m_sdfResolution[2] != z) {
            m_sdfResolution[2] = z;
        }
    }

    std::array<unsigned int, 3> sdfResolution() const {
        return m_sdfResolution;
    }

    unsigned int vMode() const {
        return m_vMode;
    }

    void setVMode(const unsigned int &mode) {
        if(m_vMode != mode) {
            m_vMode = mode;
        }
    }

    int vMaxSamples() const {
        return m_vMaxSamples;
    }

    void setVMaxSamples(const int &samples) {
        if(m_vMaxSamples != samples) {
            m_vMaxSamples = samples;
        }
    }

    unsigned int norm() const {
        return m_norm;
    }

    void setNorm(const unsigned int &norm) {
        if(m_norm != norm) {
            m_norm = norm;
        }
    }

    unsigned int sTrials() const {
        return m_sTrials;
    }

    void setSTrials(const unsigned int &trials) {
        if(m_sTrials != trials) {
            m_sTrials = trials;
        }
    }

    unsigned int vTrials() const {
        return m_vTrials;
    }

    void setVTrials(const unsigned int &trials) {
        if(m_vTrials != trials) {
            m_vTrials = trials;
        }
    }

    scalar sDensity() const {
        return m_sInitialDensity;
    }

    void setSDensity(const scalar &density) {
        if(m_sInitialDensity != density) {
            m_sInitialDensity = density;
        }
    }

    scalar vDensity() const {
        return m_vInitialDensity;
    }

    void setVDensity(const scalar &density) {
        if(m_vInitialDensity != density) {
            m_vInitialDensity = density;
        }
    }

    scalar sMinDistance() const {
        return m_sMinDistance;
    }

    void setSMinDistance(const scalar &minDistance) {
        if(m_sMinDistance != minDistance) {
            m_sMinDistance = minDistance;
        }
    }
protected:
    scalar m_radius;
    bool m_meshNormalization;
    scalar m_scaleX;
    scalar m_scaleY;
    scalar m_scaleZ;

    // Volume
    bool m_vInvert;
    std::array<unsigned int, 3> m_sdfResolution;
    unsigned int m_vMode;
    unsigned int m_vTrials;
    scalar m_vInitialDensity;
    int m_vMaxSamples;

    // Surface
    unsigned int m_norm;
    unsigned int m_sTrials;
    scalar m_sInitialDensity;
    scalar m_sMinDistance;
};


#endif //SAMPLER_SETTINGS_H
