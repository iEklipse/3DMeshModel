COMP 371 - Computer Graphics
Winter 2016

Tri-Luong Steven Dien
27415281

Programming Assignment 1
=============================================================================================
Features and Functionalities

In this programming assignment, the program display on the screen a 3D mesh model using
the sweep surfaces method. The mesh model can be displayed using points or lines or triangles.

The input for the different shapes has been imported from a list of sample input files made
available to the student.
The input files can be found in the test_surfaces_v3 folder.

The program handle different user inputs (keyboard and mouse):

-The user can look around the scene (the camera is stationary) by moving around the mouse.
-The user can move the camera move foward/backward by pressing the left mouse button + moving the mouse foward/backward.
-The user can move around the model by pressing the "M" key on the keyboard,
 then by pressing:
	- "R" (The camera go foward)
	- "F" (The camera go backward)
	- "D" (The camera move to the left)
	- "G" (The camera move to the right)
	By pressing The "M" key, the user can move around the model like in first person shooter game.
-The user can come back to the normal mode by pressing "N", the RDFG keys will be disabled.
-The user can rotate the mesh model by pressing the arrow keys.
-The user can choose which rendering mode he/she would like to see by pressing:
	- "P" (Points)
	- "W" (Lines)
	- "T" (Triangles)
==============================================================================================