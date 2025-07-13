class Camera {
public:

	// Position
	glm::vec3 m_pos;
	
	// Direction it's looking

	Camera(glm::vec3 pos);

	void setPosition(glm::vec3 pos);
};