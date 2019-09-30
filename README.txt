Working platform targets: x64
Explanation of all your controls: Right Click to rotate camera around. WASD while mantaining Right Click to move. Maintain Shift to go faster.
Known bugs, issues or not completed parts of the assignment: None
List of most interesting code snippets and shaders of the submitted project: 
For interesting code snippets: 
in model.cpp, line 162, you have the Process Mesh function, which processes the mesh loaded from Assimp
in renderer.cpp, line 168, you have the creation of the G-Buffer
in rednerer.cpp, line 338, you can see the two passes (Geometry and Lighting)


defferred.vs/fs and lighting_pas.vs/fs are the important shaders used.