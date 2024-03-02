Kiera Pozzobon
101232098

Changes:
	game.h/cpp:
	- Modified the background object to be larger and set its tag to repeat to allow for tiling
	- Set the sprite attribute of the background object to the new tile class to allow for tiling
	- Changed collectible object collision behaviour to become a ghost object when collected
	- Modified the response to key inputs for player movement to switch to physics based movement
	- Set the view matrix to follow the player object

	game_object.h/cpp:
	- Added a ghost boolean to the base class to allow the fragment shader to render objects in greyscale
	- Added orbit variables to the base class to allow for orbiting objects using the vertex shader
	- Added a method to calculate the rotation matrix for orbiting objects
	- Modified how rotation matrix is sent to the shader depending if the object is orbiting or not
	- Modified the scale attribute to be a vec2 to allow for non-uniform scaling

	tile.h/cpp:
	- A modified version of the sprite class that splits the object into a grid of tiles

	player_game_object.h/cpp:
	- Added new methods and variables to allow for physics based movement



Notes:
	- Bonus audio not implemented
	- Player hp gets printed to the console on collision

Credit:
	- Credit for new textures are in .txt files in the textures folder
