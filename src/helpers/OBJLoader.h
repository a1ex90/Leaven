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

#ifndef SAMPLER_OBJLOADER_H
#define SAMPLER_OBJLOADER_H

#include <string>
#include <array>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include "typedef.h"

class OBJLoader
{
public:

    /** This function loads an OBJ file.
      * Only triangulated meshes are supported.
      */

    static void loadObj(const std::string &filename, Matrix3X &vertices, Indices &indices, Matrix3X &normals, const Vector3 &scale = {1.0, 1.0, 1.0}, const Vector3 &posOffset = {0.0, 0.0, 0.0})
    {
        // set locale so dots are interpreted as decimal seperators
        std::locale::global(std::locale("en_US.UTF-8"));

        std::ifstream filestream;
        filestream.open(filename.c_str());
        if (filestream.fail())
        {
            std::cerr << "Failed to open file: " << filename;
            return;
        }

        std::string line_stream;
        bool vn = false;
        bool vt = false;

        std::vector<std::string> pos_buffer;
        std::vector<std::string> f_buffer;

        std::vector<Vector3> vertBuffer;
        std::vector<Vector3> normBuffer;
        std::vector<Eigen::Vector3i> indBuffer;

        while (getline(filestream, line_stream))
        {
            std::stringstream str_stream(line_stream);
            std::string type_str;
            str_stream >> type_str;

            if (type_str == "v")
            {
                Vector3 pos;
                pos_buffer.clear();
                std::string parse_str = line_stream.substr(line_stream.find("v") + 1);
                tokenize(parse_str, pos_buffer);
                for (unsigned int i = 0; i < 3; i++)
                    pos[i] = stof(pos_buffer[i]) * scale[i] + posOffset[i];

                vertBuffer.push_back(pos);
            }
            else if (type_str == "vt")
            {
                vt = true;
                // Texture Coords handling in here
            }
            else if (type_str == "vn")
            {
                Vector3 nor;
                pos_buffer.clear();
                std::string parse_str = line_stream.substr(line_stream.find("vn") + 2);
                tokenize(parse_str, pos_buffer);
                for (unsigned int i = 0; i < 3; i++)
                    nor[i] = stof(pos_buffer[i]);

                normBuffer.push_back(nor);
                vn = true;
            }
            else if (type_str == "f")
            {
                Eigen::Vector3i face;
                if (vn || vt)
                {
                    f_buffer.clear();
                    std::string parse_str = line_stream.substr(line_stream.find("f") + 1);
                    tokenize(parse_str, f_buffer);
                    for (int i = 0; i < 3; ++i)
                    {
                        pos_buffer.clear();
                        tokenize(f_buffer[i], pos_buffer, "/");
                        face[i] = stoi(pos_buffer[0]);
                    }
                }
                else
                {
                    f_buffer.clear();
                    std::string parse_str = line_stream.substr(line_stream.find("f") + 1);
                    tokenize(parse_str, f_buffer);
                    for (int i = 0; i < 3; ++i)
                    {
                        face[i] = stoi(f_buffer[i]);
                    }
                }
                indBuffer.push_back(face);
            }
        }
        filestream.close();

        vertices = Matrix3X::Zero(3, vertBuffer.size());
        normals = Matrix3X::Zero(3, normBuffer.size());
        indices = Indices::Zero(3, indBuffer.size());

        for(int i = 0; i < vertBuffer.size(); i++) {
            vertices.col(i) = vertBuffer[i];
        }
        for(int i = 0; i < normBuffer.size(); i++) {
            normals.col(i) = normBuffer[i];
        }
        for(int i = 0; i < indBuffer.size(); i++) {
            // reduce indice by one
            for(int c = 0; c < 3; c++) {
                indices.col(i)[c] = indBuffer[i][c] - 1;
            }

        }
    }

    static void loadPly(const std::string &filename, Matrix3X &vertices, const Vector3 &scale = {1.0, 1.0, 1.0}, const Vector3 &posOffset = {0.0, 0.0, 0.0}) {
        // set locale so dots are interpreted as decimal seperators
        std::locale::global(std::locale("en_US.UTF-8"));

        std::ifstream filestream;
        filestream.open(filename.c_str());
        if (filestream.fail())
        {
            std::cerr << "Failed to open file: " << filename;
            return;
        }

        std::string line_stream;

        std::vector<std::string> pos_buffer;
        bool headerEnd = false;

        std::vector<Vector3> vertBuffer;

        while (getline(filestream, line_stream))
        {
            std::stringstream str_stream(line_stream);
            std::string type_str;
            str_stream >> type_str;

            if (headerEnd) {
                Vector3 pos;
                pos_buffer.clear();
                tokenize(line_stream, pos_buffer);
                for (unsigned int i = 0; i < 3; i++)
                    pos[i] = stof(pos_buffer[i]) * scale[i] + posOffset[i];
                vertBuffer.push_back(pos);
            }

            if (type_str == "end_header") {
                headerEnd = true;
            }
        }
        filestream.close();

        vertices = Matrix3X::Zero(3, vertBuffer.size());

        for(int i = 0; i < vertBuffer.size(); i++) {
            vertices.col(i) = vertBuffer[i];
        }
    }

private:
    static void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ")
    {
        std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        std::string::size_type pos = str.find_first_of(delimiters, lastPos);

        while (std::string::npos != pos || std::string::npos != lastPos)
        {
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            lastPos = str.find_first_not_of(delimiters, pos);
            pos = str.find_first_of(delimiters, lastPos);
        }
    }

};

#endif //SAMPLER_OBJLOADER_H
