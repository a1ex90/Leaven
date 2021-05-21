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
 *       @date:   18.11.20
 *****************************************************/

#ifndef SAMPLER_VOLUMESAMPLER_H
#define SAMPLER_VOLUMESAMPLER_H

#include <Eigen/Dense>
#include <array>
#include <unordered_map>
#include <vector>

namespace Discregrid {
    class CubicLagrangeDiscreteGrid;
}

namespace Common {
    struct PossiblePoint;
}

/**
 * \class VolumeSampler
 * \brief Has a method to fill a given mesh with sampling particles
 */
class VolumeSampler {
protected:
#ifdef USE_DOUBLE
    typedef double scalar;
#else
    typedef float scalar;
#endif

public:
    /**
     * Fills a given mesh dense with particles aligned on a grid
     * @param vertices mesh vertices
     * @param indices mesh face indices
     * @param partRadius sample particle radius
     * @param cellSize cell size in which each sampling particle lies. usually particle diameter
     * @param maxSamples maximum number of sampling particles. -1 for dense filling
     * @param invert samples the volume between the outside of the mesh and the bounding box of the mesh
     * @param sdfResolution resolution of the SDF
     * @return sampled particles
     */
    static std::vector<Eigen::Matrix<scalar, 3, 1>> sampleMeshDense(const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices,
                                                                    const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices,
                                                                    const scalar &partRadius, const scalar &cellSize, const int &maxSamples = -1,
                                                                    const bool &invert = false,
                                                                    const std::array<unsigned int, 3>& sdfResolution = {
                                                                            static_cast<unsigned int>(20),
                                                                            static_cast<unsigned int>(20),
                                                                            static_cast<unsigned int>(20)});


    /**
     * Fills a given mesh with random sampled points inside the volume without particles intersecting each other
     * @param vertices vertices mesh vertices
     * @param indices mesh face indices
     * @param partRadius sample particle radius
     * @param numTrials # of trial iterations used to find samples in each valid cell
     * @param initialPointsDensity # initial sampling points density parameter
     * @param invert samples the volume between the outside of the mesh and the bounding box of the mesh
     * @param sdfResolution resolution of the SDF
     * @return sampled particles
     */
    static std::vector<Eigen::Matrix<scalar, 3, 1>> sampleMeshRandom(const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices,
                                                              const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices,
                                                              const scalar &partRadius,
                                                              const unsigned int &numTrials = 10,
                                                              const scalar &initialPointsDensity = 40,
                                                              const bool &invert = false,
                                                              const std::array<unsigned int, 3>& sdfResolution = {
                                                                      static_cast<unsigned int>(20),
                                                                      static_cast<unsigned int>(20),
                                                                      static_cast<unsigned int>(20)});

private:
    static Discregrid::CubicLagrangeDiscreteGrid* generateSDF(const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices, const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices,
                                                              Eigen::AlignedBox<scalar,3> bbox, const std::array<unsigned int, 3> &resolution,
                                                              const bool &invert);
    static double distanceToSDF(Discregrid::CubicLagrangeDiscreteGrid *sdf, const Eigen::Matrix<scalar, 3, 1> &x, const scalar &thickness = 0.0f);
    static void generateInitialSetP(std::vector<Common::PossiblePoint> &possiblePoints, const Eigen::AlignedBox<scalar,3> &bbox, Discregrid::CubicLagrangeDiscreteGrid *sdf, const unsigned int &numInitialPoints, const scalar &partRadius);
    static void parallelUniformVolumeSampling(std::vector<Eigen::Matrix<scalar, 3, 1>> &samples, const std::vector<Common::PossiblePoint> &possiblePoints, const scalar &minRadius,
                                       const unsigned int &numTrials, std::vector<std::vector<Eigen::Vector3i >> &phaseGroups);
};



#endif //SAMPLER_VOLUMESAMPLER_H
