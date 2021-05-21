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

#ifndef SAMPLER_TYPEDEF_H
#define SAMPLER_TYPEDEF_H

/**
 * \brief Global type defintions are in here
 */

#include <Eigen/Dense>

#define M_PI 3.14159265358979323846

/** \brief Defines the used scalar type (float or double) */
#ifdef USE_DOUBLE
typedef double scalar;
#else
typedef float scalar;
#endif

/** \brief Defines short definition of unsigned integers */
typedef unsigned int uint;

// Dense Matrices
template <int Rows, int Cols>

using MatrixT = Eigen::Matrix<scalar, Rows, Cols>;
// 2D column vector
typedef MatrixT<2, 1> Vector2;
// 2x2 matrix
typedef MatrixT<2, 2> Matrix22;
// 2x3 matrix
typedef MatrixT<2, 3> Matrix23;
// 3D column vector
typedef MatrixT<3, 1> Vector3;
// 3D integer column vector
typedef Eigen::Matrix<unsigned int, 3, 1> Vector3i;
// 3x2 matrix
typedef MatrixT<3, 2> Matrix32;
// 3x3 matrix
typedef MatrixT<3, 3> Matrix33;
// 3x4 matrix
typedef MatrixT<3, 4> Matrix34;
// 4D column vector
typedef MatrixT<4, 1> Vector4;
// 4x4 matrix
typedef MatrixT<4, 4> Matrix44;
// 3xN matrix
typedef MatrixT<3, Eigen::Dynamic> Matrix3X;
// Nx3 matrix
typedef MatrixT<Eigen::Dynamic, 3> MatrixX3;
// nD column vector
typedef MatrixT<Eigen::Dynamic, 1> VectorX;
// NxN matrix
typedef MatrixT<Eigen::Dynamic, Eigen::Dynamic> MatrixXX;
// 3xN matrix with unsigned integers for indices
typedef Eigen::Matrix<unsigned int, 3, Eigen::Dynamic> Indices;

#endif //SAMPLER_TYPEDEF_H
