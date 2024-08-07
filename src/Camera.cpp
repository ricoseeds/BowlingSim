
#include "../include/Camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
// Default camera values
const float DEF_FOV = 45.0f; // degrees

//------------------------------------------------------------
// Base Camera class constructor
//------------------------------------------------------------
Camera::Camera()
	: mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	  mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
	  mUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	  mRight(1.0f, 0.0f, 0.0f),
	  WORLD_UP(0.0f, 1.0f, 0.0f),
	  mYaw(glm::pi<float>()),
	  mPitch(0.0f),
	  mFOV(DEF_FOV)
{
}

void Camera::bumpFOV(float val)
{
	mFOV += val;
}
void Camera::setFOV(float val)
{
	mFOV = val;
}
//------------------------------------------------------------
// Base Camera - Returns view matrix
//------------------------------------------------------------
glm::mat4 Camera::getViewMatrix() const
{
	// std::cout << "Inside getViewMatrix" << std::endl;
	// std::cout << glm::to_string(mPosition) << std::endl;
	// std::cout << glm::to_string(mTargetPos) << std::endl;
	return glm::lookAt(mPosition, mTargetPos, mUp);
}

//------------------------------------------------------------
// Base Camera - Returns camera's local look vector
//------------------------------------------------------------
const glm::vec3 &Camera::getLook() const
{
	return mLook;
}

//------------------------------------------------------------
// Base Camera - Returns camera's local right vector
//------------------------------------------------------------
const glm::vec3 &Camera::getRight() const
{
	return mRight;
}

//------------------------------------------------------------
// Base Camera - Returns camera's local up vector
//------------------------------------------------------------
const glm::vec3 &Camera::getUp() const
{
	return mUp;
}

//------------------------------------------------------------
// Base Camera - Returns camera's position
//------------------------------------------------------------
const glm::vec3 &Camera::getPosition() const
{
	return mPosition;
}

//-----------------------------------------------------------------------------
// FPSCamera - Constructor
//-----------------------------------------------------------------------------
// FPSCamera::FPSCamera(glm::vec3 position, float yaw, float pitch)
// {
// 	mPosition = position;
// 	mYaw = yaw;
// 	mPitch = pitch;
// }
// FPSCamera::FPSCamera(glm::vec3 position, glm::vec3 target, float yaw, float pitch)
// {
// 	mPosition = position;
// 	mTargetPos = target;
// 	mYaw = yaw;
// 	mPitch = pitch;
// }
FPSCamera::FPSCamera(glm::vec3 position, glm::vec3 target)
{
	mPosition = position;
	mTargetPos = target;

	// Calculate the vector that looks at the target from the camera position
	// glm::vec3 lookDir = position - target;
	glm::vec3 lookDir = target - position;

	// Now Calculate the pitch and yaw from the target look vector. (radians)
	mPitch = -atan2(lookDir.y, sqrt(lookDir.x * lookDir.x + lookDir.z * lookDir.z));
	mYaw = atan2(lookDir.x, lookDir.z) + glm::pi<float>();
	dirflag = true;
	updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the camera position in world space
//-----------------------------------------------------------------------------
void FPSCamera::setPosition(const glm::vec3 &position)
{
	mPosition = position;
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the incremental position of the camera in world space
//-----------------------------------------------------------------------------
void FPSCamera::move(const glm::vec3 &offsetPos)
{
	mPosition += offsetPos;
	updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Sets the incremental orientation of the camera
//-----------------------------------------------------------------------------
void FPSCamera::rotate(float yaw, float pitch)
{
	mYaw += glm::radians(yaw);
	mPitch += glm::radians(pitch);

	// Constrain the pitch
	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
	updateCameraVectors();
}

//-----------------------------------------------------------------------------
// FPSCamera - Calculates the front vector from the Camera's (updated) Euler Angles
//-----------------------------------------------------------------------------
void FPSCamera::updateCameraVectors()
{
	// Spherical to Cartesian coordinates
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system (NOTE: Our coordinate sys has Y up not Z)

	// Calculate the view direction vector based on yaw and pitch angles (roll not considered)
	// radius is 1 for normalized length
	glm::vec3 look;
	look.x = cosf(mPitch) * sinf(mYaw);
	look.y = sinf(mPitch);
	look.z = cosf(mPitch) * cosf(mYaw);

	mLook = glm::normalize(look);

	// Re-calculate the Right and Up vector.  For simplicity the Right vector will
	// be assumed horizontal w.r.t. the world's Up vector.
	mRight = glm::normalize(glm::cross(mLook, WORLD_UP));
	mUp = glm::normalize(glm::cross(mRight, mLook));
	if (!dirflag)
	{
		mTargetPos += mPosition + mLook;
	}

	// mTargetPos = mPosition + mLook;
}