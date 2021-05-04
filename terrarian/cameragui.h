#ifndef CAMERAGUI_H
#define CAMERAGUI_H


#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>

#include <QOpenGLFunctions>
#include <QQuaternion>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <QtMath>


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT };

// Default camera values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 200.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class CameraGui
{
	bool enableTraking = true;
public:

	// Camera Attributes
	QVector3D Position;
	QVector3D Front, baseFront;
	QVector3D Up;
	QVector3D Right, baseRight;
	QVector3D WorldUp;
	// Eular Angles
	GLfloat Yaw;
	GLfloat Pitch;
	// Camera options
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;
	GLfloat Zoom;

	// Constructor with vectors
	CameraGui(QVector3D position = QVector3D(0.0f, 0.0f, 0.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
		firstMouse = true;
		baseFront = Front;
		baseRight = Right;
	}
	// Constructor with scalar values
	CameraGui(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : Front(QVector3D(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
	{
		this->Position = QVector3D(posX, posY, posZ);
		this->WorldUp = QVector3D(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->updateCameraVectors();
		firstMouse = true;
		baseFront = Front;
		baseRight = Right;
	}

	// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
	QMatrix4x4 GetViewMatrix()
	{
		//Up->y*=-1;
		QMatrix4x4 view;
		view.lookAt(this->Position, this->Position + this->Front, this->Up);
		return view;
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)

	float setZnac(float word)
	{
		return word<0?-1:1;
	}
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime, float factor = 1.f)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime;
		velocity *= factor;

		QVector3D front;
		front.setX(cos(qDegreesToRadians(this->Yaw)));
		front.setY(0);
		front.setZ(sin(qDegreesToRadians(this->Yaw)));
		front.normalize();

		if (direction == FORWARD)
		{
//			QVector3D poscor(setZnac(Front.x()), 0, setZnac(Front.z()));
//			QVector3D res = this->Position + poscor * velocity;
			QVector3D res = this->Position + front * velocity;

			this->Position.setX(res.x());
			this->Position.setZ(res.z());
		}
		if (direction == BACKWARD)
		{
			QVector3D res = this->Position - front * velocity;
			this->Position.setX(res.x());
			this->Position.setZ(res.z());
		}
		if (direction == LEFT)
		{
			QVector3D res = this->Position - Right * velocity;
			this->Position.setX(res.x());
			this->Position.setZ(res.z());
		}
		if (direction == RIGHT)
		{
			QVector3D res = this->Position + this->Right * velocity;
			this->Position.setX(res.x());
			this->Position.setZ(res.z());
		}

	}

	GLfloat lastX = 400, lastY = 300;
	bool firstMouse = true;

//	GLfloat deltaTime = 0.0f;
//	GLfloat lastFrame = 0.0f;

	bool invertX = false, invertY = false;
	void setEnableTraking(bool val)
	{
		enableTraking = val;
		if (val == false)
		{
			firstMouse = true;
		}
	}

	bool isEnableTraking()
	{
		return enableTraking;
	}
	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xpos, GLfloat ypos, float delta, GLboolean constrainPitch = true)
	{
		if (!enableTraking)
			return;
		if(firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}


		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

		lastX = xpos;
		lastY = ypos;
		//		qDebug() << "mpoused";

        xoffset *= this->MouseSensitivity * delta * 40;
        yoffset *= this->MouseSensitivity * delta * 40;


		this->Yaw   += invertX ? -xoffset : xoffset;
		this->Pitch += invertY ? -yoffset : yoffset;

//		qDebug() << Yaw << Pitch << Position.x() << Position.y() << Position.z();
		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Eular angles
		this->updateCameraVectors();
	}

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(GLfloat yoffset, float deltaTime)
	{
        GLfloat velocity = 25 * yoffset * deltaTime;
        this->Position += this->Front * velocity;
//		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
//			this->Zoom -= yoffset;
//		if (this->Zoom <= 1.0f)
//			this->Zoom = 1.0f;
//		if (this->Zoom >= 45.0f)
//			this->Zoom = 45.0f;
	}

	// Calculates the front vector from the Camera's (updated) Eular Angles
	void updateCameraVectors()
	{
		// Calculate the new Front vector
		QVector3D front;
		front.setX(cos(qDegreesToRadians(this->Yaw)) * cos(qDegreesToRadians(Pitch)));
		front.setY(sin(qDegreesToRadians(this->Pitch)));
		front.setZ(sin(qDegreesToRadians(this->Yaw)) * cos(qDegreesToRadians(Pitch)));
		this->Front = front.normalized();
		// Also re-calculate the Right and Up vector
		this->Right = QVector3D::crossProduct(this->Front, this->WorldUp).normalized();  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		this->Up = QVector3D::crossProduct(this->Right, this->Front).normalized();
		//Up.y
//		qDebug() << Front << Right << Up;
	}
private:
};


// The MAIN function, from here we start our application and run our Game loop


// Moves/alters the camera positions based on user input



#endif // CAMERAGUI_H
