# SFML_Chess
Chess implemented in C++ using SFML. My first Coding project ever; it is unfinished. Pawns only promote to queens, but it is a fully functional chess game otherwise.
An "Engine" plays moves against you using a recursive search function with a depth of 3-4 to select moves. The engine has an estimated elo of 400-600 (it's pretty easy to beat if you know a thing or two about chess).

To get this running, you need to set the additional include libraries path and additioan library dependencies path in order for the program to use the SFML libraries.

To do this:

Right click on the project "Neural Network" in the seolution explorer tab
Click "Properties"
Navigate to Linker > General > Additional Library Directories
Click the drop down arrow on the right, and click edit
Double click the path that's already there, and click the button with three dots that apperas to the right
Navigate to the project folder, then navigate to sfml > lib, click select folder, click OK
Now navigate to C/C++ > General > Additional Include Directories
Repeat steps 4-6, except in step 6 navigate to sfml > include instead
Click apply, click OK
The project should build and run successfully now.
