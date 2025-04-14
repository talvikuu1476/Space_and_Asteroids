# Space_and_Asteroids
 An OpenGL-based simulator of space, planets and asteroids, developed in C++.
 
 ![Planet with Asteroids](https://github.com/user-attachments/assets/beeca90a-ead6-496c-981e-507a94499b92)


This project is still being developed.

To run the program, libraries of OpenGL, assimp, glad, GLFW, glm are necessary. Please set up your own include and library directories based on your local environment.
Please use Visual Studio 2022 to run the program (via Space_and_Asteroids.sln), and please DO NOT open the .exe file directly.

Key Features:
1. Camera System
   - move mouse to adjust vision cone orientation
   - WASD to move
   - SPACE and CTRL to up and down
   - SCROLL UP & DOWN to zoom in & out
2. Lighting System
   - applied Blinn-Phong reflection model on the planet and asteroid model
3. Skybox
   - cube mapping
4. Model Loading
   - assimp
5. Visualised Real-time FPS

Visual Optimisations:
1. Anti-aliasing
   - MSAA 8x
2. Post-processing Pipeline
   - the sence is rendered to a full screen quad (didn't apply any effects yet)
  
Performance Optimisations:
1. Face Culling
2. Instanced Rendering for Asteriods
3. Texture Reuse Validation
