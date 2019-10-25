Working platform targets: x64
Explanation of all your controls: Right Click to rotate camera around. WASD while mantaining Right Click to move and Ctrl/Sapce Bar to go up/down. Maintain Shift to go slower.
You can click which texture to render on the left window. They are all named.
All buffers are updated with the data after the tessellation.
The last texture is the one that shows the refinement depth.
Known bugs, issues or not completed parts of the assignment: 
List of most interesting code snippets and shaders of the submitted project: All implementation was done in shaders, which are:
deferred.vert,deferred.frag,tessellation.tcs,tessellation.tes

You can add new lights by pressing any of the two buttons of IMGUI. You can choose which one to interact with with the index. The movement is fixed, but you can pause to move
and resume.

It's all set up so that it runs in Release, but if you want to run in Debug, copy the folders "data" and "resources" into the root (where .sln is).