#include "../include/main.h"

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main()
{
	// fpsCamera.setLookAt(glm::normalize(glm::vec3(-30.012413, -430.210178, -230.977584)));
	// std::cout << "INITCAM" << std::endl;
	// std::cout << glm::to_string(fpsCamera.getLook()) << std::endl;
	if (!initSDL())
	{
		std::cerr << "SDL initialization failed" << std::endl;
	}

	if (!initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}
	setupScene();

	ShaderProgram lightingShader;
	lightingShader.loadShaders("shaders/lighting_dir_point_spot.vert", "shaders/lighting_dir_point_spot.frag");

	// // Point Light positions
	// glm::vec3 pointLightPos[3] = {
	// 	glm::vec3(-5.0f, 3.8f, 0.0f),
	// 	glm::vec3(0.5f, 3.8f, 0.0f),
	// 	glm::vec3(5.0f, 3.8, 0.0f)};

	double lastTime = glfwGetTime();

	// Rendering loop
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		// Poll for and process events
		glfwPollEvents();
		update(deltaTime);
		// fpsCamera.move(campos - glm::vec3(0.0f , 0.0f, -0.00000001f));

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (!cam_dest_reached && cam_start)
		{
			if (bezier_camera_param >= 0.90000001)
			{
				cam_dest_reached = true;
				cam_start = false;
				bezier_camera_param = 1.0;
			}
			else
			{
				bezier_camera_param += 0.0006;
				glm::vec3 new_cam_point = get_bezier_points(bezier_camera_param, &dynamic_camera_points[0].x);
				fpsCamera.move(new_cam_point - fpsCamera.getPosition());
			}
		}
		else if (cam_dest_reached)
		{
			// std::cout << fpsCamera.getFOV() << std::endl;
			if (fpsCamera.getFOV() >= 18.0f)
			{
				fpsCamera.bumpFOV(-0.03);
			}
		}

		else if (respawn_scene && bezier_camera_param > 0)
		{
			bezier_camera_param -= 0.006;
			glm::vec3 new_cam_point = get_bezier_points(bezier_camera_param, &dynamic_camera_points[0].x);
			// fpsCamera.move(fpsCamera.getPosition() );
			fpsCamera.move(new_cam_point - fpsCamera.getPosition());

			// fpsCamera.move(glm::vec3(0.0f, 0.4f, 0.8f));
		}

		glm::mat4 model(1.0), view(1.0), projection(1.0);

		// Create the View matrix
		view = fpsCamera.getViewMatrix();

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 1000.0f);

		// update the view (camera) position
		glm::vec3 viewPos;
		viewPos = fpsCamera.getPosition();

		// Must be called BEFORE setting uniforms because setting uniforms is done
		// on the currently active shader program.
		lightingShader.use();
		lightingShader.setUniform("model", glm::mat4(1.0)); // do not need to translate the models so just send the identity matrix
		lightingShader.setUniform("view", view);
		lightingShader.setUniform("projection", projection);
		lightingShader.setUniform("viewPos", viewPos);

		//Setup directional and point source of lights
		setUpDirectionalAndPointLights(lightingShader);
		setUpSpotLight(lightingShader);
		//draw the floor
		renderFloor(model, lightingShader);

		//debugging essential
		// std::cout << "POS" << std::endl;
		// std::cout << glm::to_string(fpsCamera.getPosition()) << std::endl;
		// std::cout << "LOOK" << std::endl;
		// std::cout << glm::to_string(fpsCamera.getLook()) << std::endl;

		// draw rest of the meshes shader is same for all
		// if (hit_pin)
		// {
		// 	SDL_PauseAudio(0);
		// }

		for (int i = 1; i < numModels; i++)
		{
			if (i == 11 && release_ball && !hit_ball)
			{
				// if (modelPos[i].x != 0.0)
				// {
				modelPos[i] = get_bezier_points(bezier_param, &dynamic_points[0].x);
				model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]) * glm::rotate(glm::mat4(1.0), glm::radians((float)(glfwGetTime() * 1000.0f)), glm::vec3(1.0f, 0.0f, 1.0f));
				if (bezier_param >= 0.90000001)
				{
					release_ball = false;
					hit_ball = true;
					hit_pin = true;
					std::cout << bezier_param << std::endl;
					SDL_PauseAudio(0);
				}
				else
				{
					bezier_param += 0.001;
				}
				// }
				// else
				// {
				// 	modelPos[i] = modelPos[i] + glm::vec3(0.0f, 0.0f, speed_factor);
				// 	model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]) * glm::rotate(glm::mat4(1.0), glm::radians((float)(glfwGetTime() * 1000.0f)), glm::vec3(1.0f, 0.0f, 1.0f));
				// 	if (modelPos[i].z <= 0.0)
				// 	{
				// 		release_ball = false;
				// 	}
				// }

				// modelPos[i] = modelPos[i] + glm::vec3(0.0f, 0.0f, speed_factor);
				// commented sin model = glm::translate(glm::mat4(1.0), modelPos[i] + glm::vec3(3 * -sinf(glfwGetTime() * 2.0f), 0.0f, 0.0f)) * glm::scale(glm::mat4(1.0), modelScale[i]) * glm::rotate(glm::mat4(1.0), glm::radians((float)(glfwGetTime() * 1000.0f)), glm::vec3(1.0f, 0.0f, 1.0f));
				// model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]) * glm::rotate(glm::mat4(1.0), glm::radians((float)(glfwGetTime() * 1000.0f)), glm::vec3(1.0f, 0.0f, 1.0f));
			}
			else if (i == 11 && !release_ball && !hit_ball)
			{
				model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]) * glm::rotate(glm::mat4(1.0), glm::radians((float)(modelPos[i].x * 30.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			else if (!hit_ball) // if the rest of the meshes not hit
			{
				model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
			}

			else if (hit_ball) // collision avoidance
			{
				if (i == 11) // ball hits the pin
				{
					ball_acceleration -= 0.0002;
					if (ball_acceleration <= 0 && ball_in_motion)
					{
						ball_in_motion = false;
					}
					if (ball_in_motion)
					{
						modelPos[i] += glm::vec3(-0.01, 0, -ball_acceleration);
						model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
					}
					else
					{
						model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
					}
				}
				else // for the pins
				{
					model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
				}
			}

			// else if (i >= 1 && i < 6)
			// {
			// 	model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]) * glm::rotate(glm::mat4(1.0), glm::radians((float)(glfwGetTime() * 1000.0f)), glm::vec3(1.0f, 0.0f, 1.0f));
			// }
			// else if (i > 6 && i < 11)
			// {
			// 	model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]) * glm::rotate(glm::mat4(1.0), glm::radians((float)(glfwGetTime() * 1000.0f)), glm::vec3(-1.0f, 0.0f, 1.0f));
			// }
			// model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
			// Set material properties
			lightingShader.setUniform("model", model);
			lightingShader.setUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			lightingShader.setUniformSampler("material.diffuseMap", 0);
			lightingShader.setUniform("material.specular", glm::vec3(0.8f, 0.8f, 0.8f));
			lightingShader.setUniform("material.shininess", 32.0f);
			texture[i].bind(0); // set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[i].draw();		// Render the OBJ mesh
			texture[i].unbind(0);
		}

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);
		mac_patch(gWindow);

		lastTime = currentTime;
	}

	glfwTerminate();
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);
	return 0;
}
void renderFloor(glm::mat4 model, ShaderProgram lightingShader)
{
	model = glm::translate(glm::mat4(1.0), modelPos[0]) * glm::scale(glm::mat4(1.0), modelScale[0]) * glm::rotate(glm::mat4(1.0), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightingShader.setUniform("model", model);

	// Set material properties
	lightingShader.setUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	lightingShader.setUniformSampler("material.diffuseMap", 0);
	lightingShader.setUniform("material.specular", glm::vec3(0.8f, 0.8f, 0.8f));
	lightingShader.setUniform("material.shininess", 32.0f);

	texture[0].bind(0); // set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
	mesh[0].draw();		// Render the OBJ mesh
	texture[0].unbind(0);
}