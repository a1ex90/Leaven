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

#include "surfaceSampler.h"

#include "typedef.h"
#include "common.h"
#include <algorithm>
#include <limits>

#include <iostream>

using namespace Common;

/******************************************************
 * Public Functions
 *****************************************************/

std::vector<Eigen::Matrix<scalar, 3, 1>> SurfaceSampler::sampleMesh(
        const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices,
        const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices, const scalar &minRadius,
        const unsigned int &numTrials, const scalar &initialPointsDensity, const unsigned int &distanceNorm) {
    std::vector<Vector3> samples;

    scalar cellSize = minRadius / sqrt(3.0);

    // Compute Bounding Box
    auto bbox = Common::computeBoundingBox(vertices);

    // Area of each triangle
    std::vector<scalar> areas;
    // Largest triangle area
    scalar maxArea = std::numeric_limits<scalar>::min();
    // Total area of all triangles
    auto totalArea = static_cast<scalar>(0.0);
    calculateTriangleAreas(areas, maxArea, totalArea, vertices, indices);

    const scalar circleArea = M_PI * minRadius * minRadius;
    const auto numInitialPoints = static_cast<uint>(initialPointsDensity * (totalArea / circleArea));

    std::vector<PossiblePoint> possiblePoints;
    possiblePoints.resize(numInitialPoints);

    std::vector<Eigen::Matrix<scalar, 3, 1>> faceNormals;
    computeFaceNormals(faceNormals, vertices, indices);

    // Generate the initial set of possible positions P
    generateInitialSetP(possiblePoints, areas, maxArea, vertices, indices);

    // Calculate the cell indices for all points in P
    const scalar factor = 1.0 / cellSize;

#pragma omp parallel for schedule(static)
    for (int i = 0; i < (int)possiblePoints.size(); i++)
    {
        const Vector3& v = possiblePoints[i].pos;
        const int cellPos1 = Common::floor((v.x() - bbox.min()[0]) * factor) + 1;
        const int cellPos2 = Common::floor((v.y() - bbox.min()[1]) * factor) + 1;
        const int cellPos3 = Common::floor((v.z() - bbox.min()[2]) * factor) + 1;
        possiblePoints[i].cP = CellPos(cellPos1, cellPos2, cellPos3);
    }

    // Sort Initial points for CellID
    sort(possiblePoints, 0, (int) possiblePoints.size() - 1);

    // PoissonSampling
    parallelUniformSurfaceSampling(samples, possiblePoints, numTrials, minRadius, distanceNorm, faceNormals);

    return samples;
}

/******************************************************
 * Private Functions
 *****************************************************/

void SurfaceSampler::computeFaceNormals(std::vector<Vector3> &faceNormals, const Matrix3X &vertices, const Indices &indices) {
    const uint numFaces = indices.cols();
    faceNormals.resize(numFaces);

#pragma omp parallel default(shared)
    {
#pragma omp for schedule(static)
        for (uint i = 0; i < numFaces; i++)
        {
            // Three triangle vertices forming the face
            const Vector3 &a = vertices.col(indices.col(i)[0]);
            const Vector3 &b = vertices.col(indices.col(i)[1]);
            const Vector3 &c = vertices.col(indices.col(i)[2]);

            // Calculate face normal
            Vector3 v1 = b - a;
            Vector3 v2 = c - a;

            faceNormals[i] = v1.cross(v2);
            faceNormals[i].normalize();
        }
    }
}

void SurfaceSampler::calculateTriangleAreas(std::vector<scalar> &areas, scalar &maxArea, scalar &totalArea,
                                            const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices,
                                            const Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> &indices) {
    const unsigned int numFaces = indices.cols();
    areas.resize(numFaces);
    scalar tmpTotalArea = static_cast<scalar>(0.0);
    scalar tmpMaxArea = std::numeric_limits<scalar>::min();

#pragma omp parallel default(shared)
    {
        // Compute area of each triangle
#pragma omp for reduction(+:totalArea) schedule(static)
        for (int i = 0; i < (int)numFaces; i++)
        {
            const Vector3 &a = vertices.col(indices.col(i)[0]);
            const Vector3 &b = vertices.col(indices.col(i)[1]);
            const Vector3 &c = vertices.col(indices.col(i)[2]);

            const Vector3 d1 = b - a;
            const Vector3 d2 = c - a;

            const scalar area = (d1.cross(d2)).norm() / 2.0;
            areas[i] = area;
            tmpTotalArea += area;

            if (area > tmpMaxArea)
            {
#pragma omp critical
                {
                    tmpMaxArea = std::max(area, tmpMaxArea);
                }
            }

        }
    }
    maxArea = std::max(tmpMaxArea, maxArea);
    totalArea = tmpTotalArea;
}

void SurfaceSampler::generateInitialSetP(std::vector<PossiblePoint> &possiblePoints, const std::vector<scalar> &areas,
                                         const scalar &maxArea, const Matrix3X &vertices, const Indices &indices) {
#pragma omp parallel default(shared)
    {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<scalar> uniformDist(0.0, 1.0);
        // Randomly generating possible positions on the surface
#pragma omp for schedule(static)
        for (int i = 0; i < (int)possiblePoints.size(); i++)
        {
            // Random barycentric coordinates
            scalar rand1 = sqrt(uniformDist(mt));
            scalar u = 1.0 - rand1;
            scalar v = uniformDist(mt) * rand1;
            scalar w = 1.0 - u - v;

            // Random triangle index with probability proportional to its area
            const uint randTriangleIndex = generateRandomTriangleIndex(areas, maxArea, mt, uniformDist);

            // Calculating point coordinates
            const Vector3 &a = vertices.col(indices.col(randTriangleIndex)[0]);
            const Vector3 &b = vertices.col(indices.col(randTriangleIndex)[1]);
            const Vector3 &c = vertices.col(indices.col(randTriangleIndex)[2]);

            possiblePoints[i].pos = u * a + v * b + w * c;
            possiblePoints[i].ID = randTriangleIndex;
        }
    }
}

unsigned int SurfaceSampler::generateRandomTriangleIndex(const std::vector<scalar> &areas,
                                                         const scalar &maxArea, std::mt19937 &mt,
                                                         std::uniform_real_distribution<scalar> &uniformDist) {
    bool notValid = true;
    uint index;
    while (notValid)
    {
        index = static_cast<uint>(static_cast<scalar>(areas.size()) * uniformDist(mt));
        if (uniformDist(mt) < areas[index] / maxArea)
            notValid = false;
    }
    return index;
}

void SurfaceSampler::parallelUniformSurfaceSampling(std::vector<Vector3> &samples, const std::vector<PossiblePoint> &possiblePoints, const unsigned int &numTrials,
                                                    const scalar &minRadius, const unsigned int &distanceNorm,
                                                    const std::vector<Eigen::Matrix<scalar, 3, 1>> &faceNormals) {
    std::vector<std::vector<CellPos>> phaseGroups;
    phaseGroups.resize(27);
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
                            if (!checkNeighbors(hMap, test, possiblePoints, minRadius, distanceNorm, faceNormals))
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
