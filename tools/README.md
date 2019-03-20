These tools provide basic visualization methods for the the vector output of the genetic algorithm code. They both use the graph
visualization package Graphviz [https://graphviz.gitlab.io](https://graphviz.gitlab.io).

Python version
==============

The `plot.py` tool requires a working python installation of Graphviz. For conda users a working environment can be 

```
conda env create -f environment.yml
conda activate acse-4-p3
```

or you can (conda) install the packages yourself
```
conda install graphviz python-graphviz
```

Run the tool like

```
python plot.py output 0 1 2
```

This will create a file `output.pdf` visualising the circuit (in this case the 1 unit circuit).
For a more complicated circuit (from the problem specification)

```
python plot.py output 0 1 3 2 0 4 3 0 5
```

C++ version
===========

A C++ stub is also provided, linking against the Graphviz C library.

On CX1, this can be built with

```
module load graphviz
make plot
```

And run as

```
./plot output 0 1 3 2 0 4 3 0 5
```

On a Windows build, it will be necessary to download and build graphviz, and then modify the included `plot.vcxproj` appropriately.

Brief graphviz build instructions are

 1. Download the source code with  ` git clone --recurse-submodules https://gitlab.com/graphviz/graphviz.git`
 2. Add `%HomePath%\graphviz\windows\dependencies\graphviz-build-utilities\` to your path
 3. Start Visual Studio, open `graphviz\graphviz.sln` and build the package.

