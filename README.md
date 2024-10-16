# ECS focused 2D Game Engine and Editor

The following project is my final project for CS4850. I built a 2D based SDL game engine and editor. The Rendering core uses SDL with the editor windows and panels using ImGUI. The architecutre and documention is featured on my website below.

## Game/Engine Publicity

**Project Website**: https://jritchie02.github.io/

## Documentation Instructions
To generate documentation for this project please run
```bash
cd docs

./doxygen
```

Ensure that graphviz is installed to generate UML diagrams in the documentation

## Compilation Instructions

### Mac OS / Linux
This project uses Make to compile and build. DearImGui source code is provided in the thirdParty folder

**Note:** Box2D is installed as a Git submodule, you may need to run the following command after cloning the repository to initialize and update submodules:

```bash
git submodule update --init --recursive

cd thirdParty/Box2d
./build.sh
```

After building Box2d the Makefile should be able to compile and link the project to Box2D.

This project also requires pybind. Please install pybind to your machine before building

To build the Engine

```bash
cd bin

make -C .. all                                                                      
```

which generates ./bin/blockbyte.so

To run levels and the level editor


```bash
cd bin

python3 level1.py -o blockbyte.so    
python3 level2.py -o blockbyte.so    
python3 level3.py -o blockbyte.so    
python3 levelBuilder.py -o blockbyte.so                                                                        
```

## Project Hieararchy
### ./Engine Directory Organization

- Docs 
    - Source Code Documentation
- Assets
    - Art assets (With the Sub directories music, sound, images, and anything else)
- src
    - source code(.cpp files) The make file or any build scripts that automate the building of your project should reside here.
- include
    - header files(.h and .hpp files)
- lib
    - libraries (.so, .dll, .a, .dylib files). Note this is a good place to put SDL
- bin
    - This is the directory where your built executable(.exe for windows, .app for Mac, or a.out for Linux) and any additional generated files are put after each build.
- EngineBuild (Optional)
    - You may optionally put a .zip to you final deliverable. One should be able to copy and paste this directory, and only this directory onto another machine and be able to run the game. This is optional because for this course we will be building your projects from source. However, in the game industry it is useful to always have a build of a game ready for testers, thus a game project hieararchy would likely have this directory in a repo or other storage medium.
- ThirdParty
    - Code that you have not written if any.