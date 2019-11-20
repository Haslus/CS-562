Working platform targets: x64
Explanation of all your controls: Right Click to rotate camera around. WASD while mantaining Right Click to move and Ctrl/Sapce Bar to go up/down. Maintain Shift to go slower.
You can click which texture to render on the main viewport on the left window.

Known bugs, issues or not completed parts of the assignment: Ambient Occlusion might not be 100% perfect.

List of most interesting code snippets and shaders of the submitted project: All important implementation was done in shaders, which are:
bilateralfilter.vert/frag and hbao.frag/vert

You can add new lights by pressing any of the two buttons of IMGUI. You can choose which one to interact with with the index. The movement is fixed, but you can pause to move
and resume.
You have all the parameters that you asked on IMGUI.

It's all set up so that it runs in Release, but if you want to run in Debug, copy the folders "data" and "resources" into the root (where .sln is).

Changing the sceneDecals.json, properly changes the scene now.