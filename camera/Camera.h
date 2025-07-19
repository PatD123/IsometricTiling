enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:

	glm::vec3 m_pos;
	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_worldUp;
	
	float m_yaw = YAW;
	float m_pitch = PITCH;
	float m_camSpeed = 10.0f;
	float m_mouseSensitivity = 0.1f;

	Camera(glm::vec3 pos);

	void setPosition(glm::vec3 pos);

	void processMovement(GLFWwindow* window, float deltaTime);

	void updateCameraVectors();

	glm::mat4 getViewMat();

	void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
};