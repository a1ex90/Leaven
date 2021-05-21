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
 *       @date:   23.11.20
 *****************************************************/

#ifndef SAMPLER_SURFACESAMPLING_H
#define SAMPLER_SURFACESAMPLING_H

#include <Eigen/Dense>
#include <unordered_map>
#include <vector>
#include <random>

namespace Common {
    struct PossiblePoint;
}

/**
 * \class SurfaceSampler
 * \brief Has a method to sample the surface of a given mesh with particles
 * of a minimal given distance. Follows the algorithm described in:
 * J. Bowers, et. al. Parallel poisson disk sampling with spectrum analysis on surfaces.
 * ACM Trans. Graph., 29(6), December 2010
 */
class SurfaceSampler
{
protected:
#ifdef USE_DOUBLE
    typedef double scalar;
#else
    typedef float scalar;
#endif
public:
    /**
     * Performs surface sampling of a given mesh as a poisson disk sampling
     * @param vertices mesh vertices
     * @param indices mesh face indices
     * @param minRadius minimal distance of sampled particles
     * @param numTrials # of trial iterations used to find samples in each valid cell
     * @param initialPointsDensity # initial sampling points density parameter
     * @param distanceNorm 0: euclidean norm, 1: approx geodesic distance
     * @return sampled particles
     */
    static std::vector<Eigen::Matrix<scalar, 3, 1>> sampleMesh(const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices,
                                                        const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices,
                                                        const scalar &minRadius, const unsigned int &numTrials = 10,
                                                        const scalar &initialPointsDensity = 40, const unsigned int &distanceNorm = 1);

protected:
    static void computeFaceNormals(std::vector<Eigen::Matrix<scalar, 3, 1>> &faceNormals, const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices, const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices);
    static void calculateTriangleAreas(std::vector<scalar> &areas, scalar &maxArea, scalar &totalArea,
                                const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices, const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices);
    static void generateInitialSetP(std::vector<Common::PossiblePoint> &possiblePoints, const std::vector<scalar> &areas,
                             const scalar &maxArea, const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices, const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices);
    static unsigned int generateRandomTriangleIndex(const std::vector<scalar> &areas,
                                             const scalar &maxArea, std::mt19937 &mt,
                                             std::uniform_real_distribution<scalar> &uniformDist);
    static void parallelUniformSurfaceSampling(std::vector<Eigen::Matrix<scalar, 3, 1>> &samples, const std::vector<Common::PossiblePoint> &possiblePoints, const unsigned int &numTrials,
                                               const scalar &minRadius, const unsigned int &distanceNorm,
                                               const std::vector<Eigen::Matrix<scalar, 3, 1>> &faceNormals);
};

#endif //SAMPLER_SURFACESAMPLING_H
