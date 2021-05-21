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
 *       @date:   08.03.21
 *****************************************************/

#ifndef MESHSAMPLER_COMMON_H
#define MESHSAMPLER_COMMON_H

#include <Eigen/Dense>

namespace Common {
#ifdef USE_DOUBLE
    typedef double scalar;
#else
    typedef float scalar;
#endif

    typedef Eigen::Vector3i CellPos;

    /**
     * Hash Function (Spatial Hashing)
     */
    struct HashFunc
    {
        std::size_t operator()(const CellPos& k) const
        {
            const int p1 = 73856093 * k[0];
            const int p2 = 19349663 * k[1];
            const int p3 = 83492791 * k[2];
            return (size_t)(p1 + p2 + p3);
        }
    };

    /**
     * Information about the randomly generated initial possible sampling positions
     */
    struct PossiblePoint
    {
        // Cell Position
        CellPos cP;
        // Actual spatial position
        Eigen::Matrix<scalar, 3, 1> pos;
        // Triangle ID
        unsigned int ID;
    };

    /**
     * Struct for the hash entry
     */
    struct HashEntry
    {
        HashEntry(){};
        // Valid samples for cell
        std::vector<unsigned int> samples;
        // First index of sorted possible points for cell corresponding to hash entry
        unsigned int startIndex;
    };

    static int floor(const scalar v)
    {
        return (int)(v + 32768.f) - 32768;			// Shift to get positive values
    }

    /**
     * Calculates the bounding box of given vertices
     * @param vertices vertices
     * @return bounding box
     */
    static Eigen::AlignedBox<scalar,3> computeBoundingBox(const Eigen::Matrix<scalar, 3, Eigen::Dynamic> &vertices) {
        Eigen::AlignedBox<scalar, 3> box;
        box.min() = vertices.col(0);
        box.max() = box.min();
        box.setEmpty();
        for (uint i = 1; i < vertices.cols(); i++) {
            box.extend(vertices.col(i));
        }
        return box;
    }

    static bool compareCellID(CellPos& a, CellPos& b) {
        for (unsigned int i = 0; i < 3; i++)
        {
            if (a[i] < b[i]) return true;
            if (a[i] > b[i]) return false;
        }

        return false;
    }

    static int partition(std::vector<PossiblePoint> &possiblePoints, const int &left, const int &right) {
        int i = left;
        int j = right;
        Eigen::Matrix<scalar, 3, 1> tmpPos;
        CellPos tmpCell;
        PossiblePoint tmpPoint;
        CellPos pivot = possiblePoints[left + (right - left) / 2].cP;

        while (i <= j)
        {
            while (compareCellID(possiblePoints[i].cP, pivot))
                i++;

            while (compareCellID(pivot, possiblePoints[j].cP))
                j--;

            if (i <= j)
            {
                tmpPoint = possiblePoints[i];
                possiblePoints[i] = possiblePoints[j];
                possiblePoints[j] = tmpPoint;
                i++;
                j--;
            }
        }
        return i;
    }

    static void sort(std::vector<PossiblePoint> &possiblePoints, const int &left, const int &right) {
        if (left < right)
        {
            int index = partition(possiblePoints, left, right);
            sort(possiblePoints, left, index - 1);
            sort(possiblePoints, index, right);
        }
    }

    static bool checkCell(const std::unordered_map<CellPos, HashEntry, HashFunc>& hMap, const CellPos& cell, const PossiblePoint& point,
                          const std::vector<PossiblePoint> &possiblePoints, const scalar &minRadius, const unsigned int &distanceNorm,
                          const std::vector<Eigen::Matrix<scalar, 3, 1>> &faceNormals) {
        const auto nbEntryIt = hMap.find(cell);
        if (nbEntryIt != hMap.end())
        {
            const HashEntry& nbEntry = nbEntryIt->second;
            for(unsigned int sample : nbEntry.samples)
            {
                const PossiblePoint &point2 = possiblePoints[sample];
                scalar dist;
                if (distanceNorm == 0 || point.ID == point2.ID)
                {
                    dist = (point.pos - point2.pos).norm();
                }
                else if (distanceNorm == 1)
                {
                    Eigen::Matrix<scalar, 3, 1> v = (point2.pos - point.pos).normalized();
                    scalar c1 = faceNormals[point.ID].dot(v);
                    scalar c2 = faceNormals[point2.ID].dot(v);

                    dist = (point.pos - point2.pos).norm();
                    if (std::abs(c1 - c2) > static_cast<scalar>(0.00001))
                        dist *= (asin(c1) - asin(c2)) / (c1 - c2);
                    else
                        dist /= (sqrt(1.0 - c1*c1));
                }
                else
                {
                    return true;
                }

                if (dist < minRadius)
                    return true;
            }
        }
        return false;
    }

    static bool checkNeighbors(const std::unordered_map<CellPos, HashEntry, HashFunc>& hMap, const PossiblePoint& point,
                               const std::vector<PossiblePoint> &possiblePoints, const scalar &minRadius, const unsigned int &distanceNorm,
                               const std::vector<Eigen::Matrix<scalar, 3, 1>> &faceNormals) {
        CellPos nbPos = point.cP;

        // check neighboring cells inside to outside
        if (checkCell(hMap, nbPos, point, possiblePoints, minRadius, distanceNorm, faceNormals))
            return true;
        for (int l = 1; l < 3; l++)
        {
            for (int k = -l; k < l + 1; k += 2 * l)
            {
                for (int i = -l + 1; i < l ; i++)
                {
                    for (int j = -l + 1; j < l ; j++)
                    {
                        nbPos = CellPos(i, k, j) + point.cP;
                        if (checkCell(hMap, nbPos, point, possiblePoints, minRadius, distanceNorm, faceNormals))
                            return true;
                    }
                }

                for (int i = -l; i < l + 1; i++)
                {
                    for (int j = -l + 1; j < l ; j++)
                    {
                        nbPos = CellPos(j, i, k) + point.cP;
                        if (checkCell(hMap, nbPos, point, possiblePoints, minRadius, distanceNorm, faceNormals))
                            return true;
                    }

                    for (int j = -l; j < l + 1; j++)
                    {
                        nbPos = CellPos(k, i, j) + point.cP;
                        if (checkCell(hMap, nbPos, point, possiblePoints, minRadius, distanceNorm, faceNormals))
                            return true;
                    }
                }
            }
        }
        return false;
    }
}

#endif //MESHSAMPLER_COMMON_H