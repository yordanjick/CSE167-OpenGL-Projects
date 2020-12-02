#include "Model3D.h"

Model3D::Model3D(std::string objFilename)
{
	std::ifstream objFile(objFilename); // The obj file we are reading.

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		std::string line; // A line in the file.

		// Read lines from the file.
		while (std::getline(objFile, line))
		{
			// Turn the line into a string stream for processing.
			std::stringstream ss;
			ss << line;

			// Read the first word of the line.
			std::string label;
			ss >> label;

			// If the line is about vertex (starting with a "v").
			if (label == "v")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;

				// Process the point. For example, you can save it to a.
				points.push_back(point);
			}
			// If the line is about vertex normal (starting with a "vn").
			else if (label == "vn")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;

				// Process the point.
				points.push_back(normal);
			}
			// If line is about a face (starting with "f")
			else if (label == "f")
			{
				std::string face;
				std::getline(ss, face, '/');

				// get index of the face and store it as an int in faces
				// subtract 1 since indices start at 1 in .obj file
				faces.push_back(std::stoul(face, nullptr) - 1);
			}

		}
	}
	else
	{
		std::cerr << "Can't open the file " << objFilename << std::endl;
	}

	objFile.close();

	// set initial max & min values to 0th point in array
	float max_x = points[0].x;
	float max_y = points[0].y;
	float max_z = points[0].z;

	float min_x = max_x;
	float min_y = max_y;
	float min_z = max_z;

	float max_dist_abs = fabs(max_x);

	// iterate through all points and find min and max values of all the points
	for (glm::vec3& point : points)
	{
		if (point.x > max_x)
			max_x = point.x;
		else if (point.x < min_x)
			min_x = point.x;

		if (point.y > max_y)
			max_y = point.y;
		else if (point.y < min_y)
			min_y = point.y;

		if (point.z > max_z)
			max_z = point.z;
		else if (point.z < min_z)
			min_z = point.z;

		float t_max = sqrtf(point.x * point.x + point.y * point.y + point.z * point.z);

		if (t_max > max_dist_abs)
		{
			max_dist_abs = t_max;
		}
	}

	// find midpoints of all the max and mins
	float mid_x = (max_x + min_x) / 2;
	float mid_y = (max_y + min_y) / 2;
	float mid_z = (max_z + min_z) / 2;

	glm::vec3 midpoint(mid_x, mid_y, mid_z);

	// then iterate through all points again, and subtract midpoint
	for (glm::vec3& point : points)
	{
		point -= midpoint;
	}

	// find how far away the furthest point is
	float max_dist = fmax(fmax(fabs(max_x - mid_x), fabs(min_x - mid_x)),
		fmax(fmax(fabs(max_y - mid_y), fabs(min_y - mid_y)),
			fmax(fabs(max_z - mid_z), fabs(min_z - mid_z))));

	// after normalizing points to a radius of 1, scale back up
	float scaling_factor = 8.4f;

	// iterate through all points, normalize all to a radius of 1 based on the pt furthest away, then scale back up
	for (glm::vec3& point : points)
	{
		point /= max_dist;
		point *= scaling_factor;
	}


	 // Set the model matrix to an identity matrix. 
	model = glm::mat4(1);
	// Set the color. 
	color = glm::vec3(1, 0, 0);

	// Generate a vertex array (VAO) and a vertex buffer objects (VBO).
	// vertex normals buffer objects (VNBO), element buffer objects (EBO)
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vnbo);
	glGenBuffers(1, &ebo);

	// Bind to the VAO.
	glBindVertexArray(vao);

	// Bind to the first VBO. We will use it to store the points.
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	
	// Pass in the data.
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(),
		points.data(), GL_STATIC_DRAW);

	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	// bind VNBO, 2nd vbo
	glBindBuffer(GL_ARRAY_BUFFER, vnbo);

	// pass in data
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(),
		normals.data(), GL_STATIC_DRAW);

	// Enable vertex attribute 0. 
	// We will be able to access points through it.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind from the VBO.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind from the VAO.
	glBindVertexArray(0);

	// Bind EBO as an element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* faces.size(),
		faces.data(), GL_STATIC_DRAW);

}

Model3D::~Model3D()
{
	// Delete the VBO, VNBO, EBO, and the VAO.
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vnbo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Model3D::draw()
{
	// bind vao
	glBindVertexArray(vao);
	// draw elements
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	// unbind vao
	glBindVertexArray(0);
}

void Model3D::update()
{
}

void Model3D::spin(float deg)
{
}