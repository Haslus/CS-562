Working platform targets: x64
Explanation of all your controls: Right Click to rotate camera around. WASD while mantaining Right Click to move and Ctrl/Sapce Bar to go up/down. Maintain Shift to go faster.
You can click which texture to render on the left window.
Known bugs, issues or not completed parts of the assignment: I have some warnings on glm files because of a constant that I don't think I should try to change.
List of most interesting code snippets and shaders of the submitted project:
In renderer.cpp from 248 to 445 you have the creation of all buffers/textures
In renderer.cpp 462, you have the main loop. It has all the passes that I do, divided by comments. 

You can add new lights by pressing any of the two buttons of IMGUI. You can choose which one to interact with with the index. The movement is fixed, but you can pause to move
and resume.

Also copy the folders "data" and "resources" into the folder "bin".