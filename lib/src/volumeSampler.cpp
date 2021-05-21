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

#include "volumeSampler.h"

#include <Discregrid/All>
#include "typedef.h"
#include "common.h"
#include <random>
#include <iostream>

using namespace Common;

/******************************************************
 * Public Functions
 *****************************************************/

std::vector<Vector3> VolumeSampler::sampleMeshDense(
                const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices,
                const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices, const scalar &partRadius, const scalar &cellSize,
                const int &maxSamples, const bool &invert, const std::array<unsigned int, 3> &sdfResolution) {
    // Compute Bounding Box
    auto bbox = computeBoundingBox(vertices);

    // Generate SDF
    Discregrid::CubicLagrangeDiscreteGrid* sdf = generateSDF(vertices, indices, bbox, sdfResolution, invert);

    const scalar halfCellSize = cellSize / static_cast<scalar>(2.0);

    unsigned int sampleCounter = 0;
    std::vector<Vector3> samples;
    std::vector<uint> freeCells;

    for (scalar z = bbox.min()[2]; z <= bbox.max()[2]; z += cellSize)
    {
        for (scalar y = bbox.min()[1]; y <= bbox.max()[1]; y += cellSize)
        {
            for (scalar x = bbox.min()[0]; x <= bbox.max()[0]; x += cellSize)
            {
                auto offsetX = static_cast<scalar>(0.0), offsetY = static_cast<scalar>(0.0), offsetZ = static_cast<scalar>(0.0);
                Vector3 particlePosition = {x + halfCellSize + offsetX, y + halfCellSize + offsetY, z + halfCellSize + offsetZ};

                if (distanceToSDF(sdf, particlePosition, -partRadius) < 0.0) {
                    samples.push_back(particlePosition);
                    freeCells.push_back(sampleCounter);
                    sampleCounter++;
                }

            }
        }
    }

    return samples;
}

std::vector<Eigen::Matrix<scalar, 3, 1>> VolumeSampler::sampleMeshRandom(
        const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices,
        const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices, const scalar &partRadius,
        const unsigned int &numTrials, const scalar &initialPointsDensity, const bool &invert,
        const std::array<unsigned int, 3> &sdfResolution) {
    // Compute Bounding Box
    auto bbox = Common::computeBoundingBox(vertices);

    // Generate SDF
    Discregrid::CubicLagrangeDiscreteGrid* sdf = generateSDF(vertices, indices, bbox, sdfResolution, invert);

    std::vector<Vector3> samples;
    scalar minRadius = static_cast<scalar>(2.0)*partRadius;
    scalar cellsize = minRadius / sqrt(3.0);

    const auto numInitialPoints = static_cast<uint>(initialPointsDensity * (bbox.volume() / (cellsize*cellsize*cellsize)));

    std::vector<PossiblePoint> possiblePoints;
    possiblePoints.reserve(numInitialPoints);

    std::vector<std::vector<CellPos>> phaseGroups;
    phaseGroups.resize(27);

    // Generate the initial point set
    generateInitialSetP(possiblePoints, bbox, sdf, numInitialPoints, partRadius);

    // Calculate the cell indices for all points in P
    const scalar factor = 1.0 / cellsize;

    #pragma omp parallel for schedule(static) shared(possiblePoints, bbox, factor) default(none)
    for (auto & possiblePoint : possiblePoints)
    {
        const Vector3& v = possiblePoint.pos;
        const int cellPos1 = Common::floor((v.x() - bbox.min()[0]) * factor) + 1;
        const int cellPos2 = Common::floor((v.y() - bbox.min()[1]) * factor) + 1;
        const int cellPos3 = Common::floor((v.z() - bbox.min()[2]) * factor) + 1;
        possiblePoint.cP = CellPos(cellPos1, cellPos2, cellPos3);
    }

    // Sort Initial points for CellID
    sort(possiblePoints, 0, (int) possiblePoints.size() - 1);

    // PoissonSampling
    parallelUniformVolumeSampling(samples, possiblePoints, minRadius, numTrials, phaseGroups);

    return samples;
}

/******************************************************
 * Private Functions
 *****************************************************/

Discregrid::CubicLagrangeDiscreteGrid * VolumeSampler::generateSDF(const Matrix3X &vertices, const Indices &indices,
                                                                   Eigen::AlignedBox<scalar, 3> bbox,
                                                                   const std::array<unsigned int, 3> &resolution,
                                                                   const bool &invert) {
    std::vector<double> doubleVec;
    doubleVec.resize(3 * vertices.cols());
    for (unsigned int i = 0; i < vertices.cols(); i++)
        for (unsigned int j = 0; j < 3; j++)
            doubleVec[3 * i + j] = static_cast<double>(vertices.col(i)[j]);
    Discregrid::TriangleMesh sdfMesh(&doubleVec[0], indices.data(), vertices.cols(), indices.cols());

    Discregrid::MeshDistance md(sdfMesh);
    Eigen::AlignedBox3d domain;
    domain.extend(bbox.min().cast<double>());
    domain.extend(bbox.max().cast<double>());
    domain.max() += 1.0e-3 * domain.diagonal().norm() * Eigen::Vector3d::Ones();
    domain.min() -= 1.0e-3 * domain.diagonal().norm() * Eigen::Vector3d::Ones();

    auto *distanceField = new Discregrid::CubicLagrangeDiscreteGrid(domain, resolution);
    auto func = Discregrid::DiscreteGrid::ContinuousFunction{};
    auto factor = static_cast<scalar>(1.0);
    if (invert)
        factor = static_cast<scalar>(-1.0);
    func = [&md,&factor](Eigen::Vector3d const& xi) {return factor * md.signedDistanceCached(xi); };

    distanceField->addFunction(func, false);

    return distanceField;
}

double VolumeSampler::distanceToSDF(Discregrid::CubicLagrangeDiscreteGrid *sdf, const Vector3 &x, const scalar &thickness) {
    Eigen::Vector3d xd = {static_cast<double>(x.x()), static_cast<double>(x.y()), static_cast<double>(x.z())};
    const double dist = sdf->interpolate(0, xd);
    if(dist == std::numeric_limits<double>::max())
        return dist;
    return dist - thickness;
}

void VolumeSampler::generateInitialSetP(std::vector<Common::PossiblePoint> &possiblePoints,
                                        const Eigen::AlignedBox<scalar, 3> &bbox,
                                        Discregrid::CubicLagrangeDiscreteGrid *sdf,
                                        const unsigned int &numInitialPoints, const scalar &partRadius) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<scalar> uniformDist(0.0, 1.0);

#pragma omp parallel for schedule(static)
    for (int i = 0; i < numInitialPoints; i++)
    {
        // Random coordinates
        scalar x = bbox.min().x() + uniformDist(mt) * (bbox.max().x() - bbox.min().x());
        scalar y = bbox.min().y() + uniformDist(mt) * (bbox.max().y() - bbox.min().y());
        scalar z = bbox.min().z() + uniformDist(mt) * (bbox.max().z() - bbox.min().z());

        Vector3 pos = Vector3(x,y,z);
        if(distanceToSDF(sdf, pos, -partRadius) < 0.0) {
            PossiblePoint p;
            p.pos = pos;
            #pragma omp critical
            {
                possiblePoints.push_back(p);
            }

        }
    }
}

void VolumeSampler::parallelUniformVolumeSampling(std::vector<Vector3> &samples,
                                                  const std::vector<PossiblePoint> &possiblePoints,
                                                  const scalar &minRadius, const unsigned int &numTrials,
                                                  std::vector<std::vector<CellPos>> &phaseGroups) {
    // Insert possible points into the HashMap
    std::unordered_map<CellPos, HashEntry, HashFunc> hMap(2 * possiblePoints.size());
    samples.clear();
    samples.reserve(possiblePoints.size());

    const uint maxSamplesPerCell = 1u;

    // Insert first possible point
    {
        const CellPos& cell = possiblePoints[0].cP;
        HashEntry &entry = hMap[cell];
        entry.startIndex = 0;
        entry.samples.reserve(maxSamplesPerCell);
        int index = cell[0] % 3 + 3 * (cell[1] % 3) + 9 * (cell[2] % 3);
        phaseGroups[index].push_back(cell);
    }

    for (int i = 1; i < (int)possiblePoints.size(); i++)
    {
        const CellPos& cell = possiblePoints[i].cP;
        if (cell != possiblePoints[i - 1].cP)
        {
            HashEntry &entry = hMap[cell];
            entry.startIndex = i;
            entry.samples.reserve(maxSamplesPerCell);
            int index = cell[0] % 3 + 3 * (cell[1] % 3) + 9 * (cell[2] % 3);
            phaseGroups[index].push_back(cell);
        }
    }
    // Loop over number of tries to find a sample in a cell
    for (int t = 0; t < (int)numTrials; t++)
    {
        // Loop over the 27 cell groups
        for (const auto &cells: phaseGroups)
        {
            // Loop over the cells in each cell group
#pragma omp parallel for schedule(static)
            for (int i = 0; i < (int)cells.size(); i++)
            {
                const auto entryIt = hMap.find(cells[i]);
                // Check if cell exists
                if (entryIt != hMap.end())
                {
                    HashEntry& entry = entryIt->second;
                    // Check if a sample is already found for this cell
                    if(entry.samples.size() > 0) {
                        continue;
                    }
                    // Check if lower than max index
                    if (entry.startIndex + t < possiblePoints.size())
                    {
                        if (possiblePoints[entry.startIndex].cP == possiblePoints[entry.startIndex + t].cP)
                        {
                            // Choose position corresponding to t-th trail from cell
                            const PossiblePoint& test = possiblePoints[entry.startIndex + t];
                            // Assign sample
                            std::vector<Eigen::Matrix<scalar, 3, 1>> tmp;
                            if (!checkNeighbors(hMap, test, possiblePoints, minRadius, 0, tmp))
                            {
                                const int index = entry.startIndex + t;
#pragma omp critical
                                {
                                    entry.samples.push_back(index);
                                    samples.push_back(possiblePoints[index].pos);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
