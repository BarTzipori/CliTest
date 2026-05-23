Binary Tree UI (Win32)

This is a minimal Win32 GUI that lets a user enter comma-separated integers and visualizes a Binary Search Tree built by insertion order.

Build (Visual Studio + CMake):


mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

Run the produced BinaryTreeUI.exe. Use the top input box to enter values like: 1,2,3,4,5,6 and press "Build Tree". Use "Export PNG" to save visualization as binary_tree_export.png.
