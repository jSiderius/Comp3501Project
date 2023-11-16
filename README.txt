Group Members:
Alex de Carle, Sam Drennan, Josh Siderius

COMP 3501 Assignment 6

Changes:

height_map.txt:
- contains the float values for the height map generated from test_map.png

map_transfer.py:
- python program used to generate the height_map.txt file

camera.h and .cpp:
- Added the following functions:
	void SetFloorScale(glm::vec3 floor_scale);
	void SetFloorPos(glm::vec3 floor_pos);
	void SetImpassableMap(std::vector<std::vector<bool>> impassable_map);
- Added the following variables:
	glm::vec3 floor_scale_;
	glm::vec3 floor_pos_;
	std::vector<std::vector<bool>> impassable_map_;