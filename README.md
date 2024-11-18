# optimal-32-vertex-graph

This code uses igraph, a graph library in C. Please use VSCode for this.

In order to set this up, you must install CMake, which can be done so [here](https://cmake.org/download/).
You must add this to the global PATH by putting the directory of CMake into the PATH(Modify environment variables).

Now, get the source code file of igraph from [here](https://igraph.org/c/#downloads).
Then, change your directory to the place where this file is(using cd)
After this, make a directory called build with mkdir build, then change directory into build with cd build.
Then, run CMake with cmake ..


Finally, you can build, test, and install it with the following:

cmake -- build .

cmake --build . --config Release

cmake --build . --target check --config Release
