![](https://raw.githubusercontent.com/a1ex90/Leaven/master/assets/img/header.png)
# LEAVEN
LEAVEN is an easy-to-use and lightweight surface and volume mesh sampling standalone application tailored for the needs of particle-based simulation. It aims to lower one entry barrier for starting with particle-based simulations while still pose a benefit to advanced users. LEAVEN has methods for random uniform surface sampling, as well as random uniform volume sampling and grid-based volume sampling. LEAVEN's methods can also be included as a library in other projects. It is the implementation from [SS21].

**Author:** Alexander Sommer,    **License:** MIT

## Getting Started
### Dependencies
- [CMake](https://cmake.org/) 3.10
- C++ 14
- [Eigen](https://eigen.tuxfamily.org/) 3.3
- [QT5](https://www.qt.io/) (only for UI not Library)
- [OpenMP](https://www.openmp.org/) (optional)

### Build Instructions
The project ist based on [CMake](https://cmake.org/). Makefiles can be generated using [CMake](https://cmake.org/) and compiled with a C++ compiler. The project has been tested on Ubuntu and Windows 10 with Visual Studio.
 
## Usage UI
![](https://raw.githubusercontent.com/a1ex90/Leaven/master/assets/img/ui.png)
### View Manipulation

Input | Action
------------ | -------------
Left Mousebutton + Movement | Rotating (Arcball)
Middle Mousebutton + Movement | Panning
Right Mousebutton + Movement | Zooming

## Usage Library
Add this module to your project CMake file:
```
add_subdirectory(Leaven/lib)
include_directories(Leaven/lib)
...
target_link_libraries(...
	LeavenLib
	)
```
Static methods for surface/volume sampling are found in:
```
#include "volumeSampler.h"
#include "surfaceSampler.h"
```
Per example this generates a randomized volume sampling from given vertices and face indices:
```
Eigen::Matrix<float, 3,-1> vertices = ...
Eigen::Matrix<unsigned int, 3, -1> indices = ...
float particleRadius = ...
std::vector<Eigen::Matrix<float, 3, 1>> sampling = VolumeSampler::sampleMeshRandom(vertices, indices, particleRadius);
```

## References
- [SS21] A. Sommer and U. Schwanecke, 2021. "LEAVEN - Lightweight Surface and Volume Mesh Sampling Application for Particle-based Simulations", WSCG 2021: full papers proceedings: 29. International Conference in Central Europe on Computer Graphics, Visualization and Computer Vision, p. 155-160.
- [KDBB17] D. Koschier, C. Deul, M. Brand and J. Bender, 2017. "An hp-Adaptive Discretization Algorithm for Signed Distance Field Generation", IEEE Transactions on Visualiztion and Computer Graphics 23, 10, 2208-2221.
- [BWWM10] J. Bowers, R. Wang, L. Wei and D. Maletz, 2010. "Parallel Poisson Disk Sampling with Spectrum Analysis on Surfaces", ACM Trans. Graph 29.

