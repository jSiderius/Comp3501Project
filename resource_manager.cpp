#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SOIL/SOIL.h>

#include "resource_manager.h"
#include "model_loader.h"

namespace game {

ResourceManager::ResourceManager(void){
}


ResourceManager::~ResourceManager(){
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size){

    Resource *res;

    res = new Resource(type, name, resource, size);

    resource_.push_back(res);
}


void ResourceManager::AddResource(ResourceType type, const std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size){

    Resource *res;

    res = new Resource(type, name, array_buffer, element_array_buffer, size);

    resource_.push_back(res);
}


void ResourceManager::LoadResource(ResourceType type, const std::string name, const char *filename){

    // Call appropriate method depending on type of resource
    if (type == Material){
        LoadMaterial(name, filename);
    } else if (type == Texture){
        LoadTexture(name, filename);
    } else if (type == Mesh){
        LoadMesh(name, filename);
    } else {
        throw(std::invalid_argument(std::string("Invalid type of resource")));
    }
}


Resource *ResourceManager::GetResource(const std::string name) const {

    // Find resource with the specified name
    for (int i = 0; i < resource_.size(); i++){
        if (resource_[i]->GetName() == name){
            return resource_[i];
        }
    }
    return NULL;
}


void ResourceManager::LoadMaterial(const std::string name, const char *prefix){

    // Load vertex program source code
    std::string filename = std::string(prefix) + std::string(VERTEX_PROGRAM_EXTENSION);
    std::string vp = LoadTextFile(filename.c_str());

    // Load fragment program source code
    filename = std::string(prefix) + std::string(FRAGMENT_PROGRAM_EXTENSION);
    std::string fp = LoadTextFile(filename.c_str());

    // Create a shader from the vertex program source code
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const char *source_vp = vp.c_str();
    glShaderSource(vs, 1, &source_vp, NULL);
    glCompileShader(vs);

    // Check if shader compiled successfully
    GLint status;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(vs, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error compiling vertex shader: ")+std::string(buffer)));
    }

    // Create a shader from the fragment program source code
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *source_fp = fp.c_str();
    glShaderSource(fs, 1, &source_fp, NULL);
    glCompileShader(fs);

    // Check if shader compiled successfully
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(fs, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error compiling fragment shader: ")+std::string(buffer)));
    }

    // Create a shader program linking both vertex and fragment shaders
    // together
    GLuint sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    // Check if shaders were linked successfully
    glGetProgramiv(sp, GL_LINK_STATUS, &status);
    if (status != GL_TRUE){
        char buffer[512];
        glGetShaderInfoLog(sp, 512, NULL, buffer);
        throw(std::ios_base::failure(std::string("Error linking shaders: ")+std::string(buffer)));
    }

    // Delete memory used by shaders, since they were already compiled
    // and linked
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Add a resource for the shader program
    AddResource(Material, name, sp, 0);
}


std::string ResourceManager::LoadTextFile(const char *filename){

    // Open file
    std::ifstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Read file
    std::string content;
    std::string line;
    while(std::getline(f, line)){
        content += line + "\n";
    }

    // Close file
    f.close();

    return content;
}



// Create the geometry for a cylinder
void ResourceManager::CreateCylinder(std::string object_name, float height, float circle_radius, int num_height_samples, int num_circle_samples) {

	// Create a cylinder

	// Number of vertices and faces to be created
	const GLuint vertex_num = num_height_samples * num_circle_samples + 2; // plus two for top and bottom
	const GLuint face_num = num_height_samples * (num_circle_samples - 1) * 2 + 2 * num_circle_samples; // two extra rings worth for top and bottom

																										// Number of attributes for vertices and faces
	const int vertex_att = 11;  // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
	const int face_att = 3; // Vertex indices (3)

							// Data buffers for the shape
	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	GLuint vbo, ebo;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att];
		face = new GLuint[face_num * face_att];
	}
	catch (std::exception &e) {
		throw e;
	}

	// Create vertices 
	float theta; // Angle for circle
	float h; // height
	float s, t; // parameters zero to one
	glm::vec3 loop_center;
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;

	for (int i = 0; i < num_height_samples; i++) { // along the side

		s = i / (float)num_height_samples; // parameter s (vertical)
		h = (-0.5 + s)*height;
		for (int j = 0; j < num_circle_samples; j++) { // small circle
			t = j / (float)num_circle_samples;
			theta = 2.0*glm::pi<GLfloat>()*t; // circle sample (angle theta)

											  // Define position, normal and color of vertex
			vertex_normal = glm::vec3(cos(theta), 0.0f, sin(theta));
			vertex_position = glm::vec3(cos(theta)*circle_radius, h, sin(theta)*circle_radius);
			vertex_color = glm::vec3(1.0 - s,
				t,
				s);
			vertex_coord = glm::vec2(s, t);

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i*num_circle_samples + j)*vertex_att + k] = vertex_position[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i*num_circle_samples + j)*vertex_att + 9] = vertex_coord[0];
			vertex[(i*num_circle_samples + j)*vertex_att + 10] = vertex_coord[1];
		}
	}

	int topvertex = num_circle_samples * num_height_samples;
	int bottomvertex = num_circle_samples * num_height_samples + 1; // indices for top and bottom vertex

	vertex_position = glm::vec3(0, (-0.5+(num_height_samples-1.0f)/num_height_samples)*height, 0); // location of top middle of cylinder
	vertex_normal = glm::vec3(0, 1, 0);
	vertex_color = glm::vec3(1, 0.6, 0.4);
	vertex_coord = glm::vec2(0, 0); // no good way to texture top and bottom

	for (int k = 0; k < 3; k++) {
		vertex[topvertex*vertex_att + k] = vertex_position[k];
		vertex[topvertex*vertex_att + k + 3] = vertex_normal[k];
		vertex[topvertex*vertex_att + k + 6] = vertex_color[k];
	}
	vertex[(topvertex)*vertex_att + 9] = vertex_coord[0];
	vertex[(topvertex)*vertex_att + 10] = vertex_coord[1];

	//================== bottom vertex
	vertex_position = glm::vec3(0, (-0.5)*height, 0); // location of bottom middle of cylinder
	vertex_normal = glm::vec3(0, -1, 0);
	// leave the color and uv alone

	for (int k = 0; k < 3; k++) {
		vertex[bottomvertex*vertex_att + k] = vertex_position[k];
		vertex[bottomvertex*vertex_att + k + 3] = vertex_normal[k];
		vertex[bottomvertex*vertex_att + k + 6] = vertex_color[k];
	}
	vertex[(bottomvertex)*vertex_att + 9] = vertex_coord[0];
	vertex[(bottomvertex)*vertex_att + 10] = vertex_coord[1];

	//===================== end of vertices

	// Create triangles
	for (int i = 0; i < num_height_samples - 1; i++) {
		for (int j = 0; j < num_circle_samples; j++) {
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_height_samples)*num_circle_samples + j,
				i*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + j);
			glm::vec3 t2(((i + 1) % num_height_samples)*num_circle_samples + j,
				((i + 1) % num_height_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + ((j + 1) % num_circle_samples));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(i*num_circle_samples + j)*face_att * 2 + k] = (GLuint)t1[k];
				face[(i*num_circle_samples + j)*face_att * 2 + k + face_att] = (GLuint)t2[k];
			}
		}
	}
	int cylbodysize = num_circle_samples * (num_height_samples - 1) * 2; // amount of array filled so far, start adding from here
																		 // triangles for top disc (fan shape)
	int i = num_height_samples - 1;
	for (int j = 0; j < num_circle_samples; j++) {
		// Bunch of wedges pointing to the centre
		glm::vec3 topwedge(
			i*num_circle_samples + j,
			topvertex,
			i*num_circle_samples + (j + 1) % num_circle_samples
		);

		// note order reversed so that all triangles point outward
		glm::vec3 botwedge(
			0 + (j + 1) % num_circle_samples,
			bottomvertex,
			0 + j
		);

		// Add the triangles to the data buffer
		for (int k = 0; k < 3; k++) {
			face[(cylbodysize + j)*face_att + k] = (GLuint)topwedge[k];
			face[(cylbodysize + j + num_circle_samples)*face_att + k] = (GLuint)botwedge[k];
		}
	}

	// Create OpenGL buffer for vertices
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	// Create OpenGL buffer for faces
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Free data buffers
	delete[] vertex;
	delete[] face;


	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);

}

void ResourceManager::CreateTorus(std::string object_name, float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples) {

	// Create a torus
	// The torus is built from a large loop with small circles around the loop

	// Number of vertices and faces to be created
	// Check the construction algorithm below to understand the numbers
	// specified below
	const GLuint vertex_num = num_loop_samples * num_circle_samples;
	const GLuint face_num = num_loop_samples * num_circle_samples * 2;

	// Number of attributes for vertices and faces
	const int vertex_att = 11;
	const int face_att = 3;

	// Data buffers for the torus
	GLfloat *vertex = NULL;
	GLuint *face = NULL;

	// Allocate memory for buffers
	try {
		vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
		face = new GLuint[face_num * face_att]; // 3 indices per face
	}
	catch (std::exception &e) {
		throw e;
	}

	// Create vertices 
	float theta, phi; // Angles for circles
	glm::vec3 loop_center;
	glm::vec3 vertex_position;
	glm::vec3 vertex_normal;
	glm::vec3 vertex_color;
	glm::vec2 vertex_coord;

	for (int i = 0; i < num_loop_samples; i++) { // large loop

		theta = 2.0*glm::pi<GLfloat>()*i / num_loop_samples; // loop sample (angle theta)
		loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

		for (int j = 0; j < num_circle_samples; j++) { // small circle

			phi = 2.0*glm::pi<GLfloat>()*j / num_circle_samples; // circle sample (angle phi)

																 // Define position, normal and color of vertex
			
			vertex_normal = glm::vec3(cos(theta)*cos(phi) , sin(theta)*cos(phi), sin(phi) );
			vertex_position = loop_center + vertex_normal * circle_radius;
			vertex_normal = glm::vec3(cos(theta)*cos(phi)+(rand()%128)/256.0, sin(theta)*cos(phi)+(rand()%128)/256.0, sin(phi)+(rand()%128)/256.0); // messed up normals
			vertex_color = glm::vec3(1.0 - ((float)i / (float)num_loop_samples),
				(float)i / (float)num_loop_samples,
				(float)j / (float)num_circle_samples);
			vertex_coord = glm::vec2(theta / (2.0*glm::pi<GLfloat>()),
				phi / (2.0*glm::pi<GLfloat>()));

			// Add vectors to the data buffer
			for (int k = 0; k < 3; k++) {
				vertex[(i*num_circle_samples + j)*vertex_att + k] = vertex_position[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 3] = vertex_normal[k];
				vertex[(i*num_circle_samples + j)*vertex_att + k + 6] = vertex_color[k];
			}
			vertex[(i*num_circle_samples + j)*vertex_att + 9] = vertex_coord[0];
			vertex[(i*num_circle_samples + j)*vertex_att + 10] = vertex_coord[1];
		}
	}

	// Create triangles
	for (int i = 0; i < num_loop_samples; i++) {
		for (int j = 0; j < num_circle_samples; j++) {
			// Two triangles per quad
			glm::vec3 t1(((i + 1) % num_loop_samples)*num_circle_samples + j,
				i*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + j);
			glm::vec3 t2(((i + 1) % num_loop_samples)*num_circle_samples + j,
				((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
				i*num_circle_samples + ((j + 1) % num_circle_samples));
			// Add two triangles to the data buffer
			for (int k = 0; k < 3; k++) {
				face[(i*num_circle_samples + j)*face_att * 2 + k] = (GLuint)t1[k];
				face[(i*num_circle_samples + j)*face_att * 2 + k + face_att] = (GLuint)t2[k];
			}
		}
	}

	// Create OpenGL buffers and copy data
	//GLuint vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	GLuint vbo, ebo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

	// Free data buffers
	delete[] vertex;
	delete[] face;

	// Create resource
	AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}


void ResourceManager::CreateSphere(std::string object_name, float radius, int num_samples_theta, int num_samples_phi){

    // Create a sphere using a well-known parameterization

    // Number of vertices and faces to be created
    const GLuint vertex_num = num_samples_theta*num_samples_phi;
    const GLuint face_num = num_samples_theta*(num_samples_phi-1)*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers 
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    float theta, phi; // Angles for parametric equation
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;
   
    for (int i = 0; i < num_samples_theta; i++){
            
        theta = 2.0*glm::pi<GLfloat>()*i/(num_samples_theta-1); // angle theta
            
        for (int j = 0; j < num_samples_phi; j++){
                    
            phi = glm::pi<GLfloat>()*j/(num_samples_phi-1); // angle phi

            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*sin(phi), sin(theta)*sin(phi), -cos(phi));
            // We need z = -cos(phi) to make sure that the z coordinate runs from -1 to 1 as phi runs from 0 to pi
            // Otherwise, the normal will be inverted
            vertex_position = glm::vec3(vertex_normal.x*radius, 
                                        vertex_normal.y*radius, 
                                        vertex_normal.z*radius),
            vertex_color = glm::vec3(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi), ((float)j)/((float)num_samples_phi));
            vertex_coord = glm::vec2(((float)i)/((float)num_samples_theta), 1.0-((float)j)/((float)num_samples_phi));

            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*num_samples_phi+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*num_samples_phi+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*num_samples_phi+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*num_samples_phi+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*num_samples_phi+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create faces
    for (int i = 0; i < num_samples_theta; i++){
        for (int j = 0; j < (num_samples_phi-1); j++){
            // Two triangles per quad
            glm::vec3 t1(((i + 1) % num_samples_theta)*num_samples_phi + j, 
                         i*num_samples_phi + (j + 1),
                         i*num_samples_phi + j);
            glm::vec3 t2(((i + 1) % num_samples_theta)*num_samples_phi + j, 
                         ((i + 1) % num_samples_theta)*num_samples_phi + (j + 1), 
                         i*num_samples_phi + (j + 1));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*(num_samples_phi-1)+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*(num_samples_phi-1)+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    // Create OpenGL buffers and copy data
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}


void ResourceManager::LoadTexture(const std::string name, const char *filename){

    // Load texture from file
    GLuint texture = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
    if (!texture){
        throw(std::ios_base::failure(std::string("Error loading texture ")+std::string(filename)+std::string(": ")+std::string(SOIL_last_result())));
    }

    // Create resource
    AddResource(Texture, name, texture, 0);
}


void ResourceManager::LoadMesh(const std::string name, const char *filename){

    // First load model into memory. If that goes well, we transfer the
    // mesh to an OpenGL buffer
    TriMesh mesh;

    // Parse file
    // Open file
    std::ifstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Parse lines
    std::string line;
    std::string ignore(" \t\r\n");
    std::string part_separator(" \t");
    std::string face_separator("/");
    bool added_normal = false;
    while (std::getline(f, line)){
        // Clean extremities of the string
        string_trim(line, ignore);
        // Ignore comments
        if ((line.size() <= 0) ||
            (line[0] == '#')){
            continue;
        }
        // Parse string
        std::vector<std::string> part = string_split(line, part_separator);
        // Check commands
        if (!part[0].compare(std::string("v"))){
            if (part.size() >= 4){
                glm::vec3 position(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
                mesh.position.push_back(position);
            } else {
                throw(std::ios_base::failure(std::string("Error: v command should have exactly 3 parameters")));
            }
        } else if (!part[0].compare(std::string("vn"))){
            if (part.size() >= 4){
                glm::vec3 normal(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
                mesh.normal.push_back(normal);
                added_normal = true;
            } else {
                throw(std::ios_base::failure(std::string("Error: vn command should have exactly 3 parameters")));
            }
        } else if (!part[0].compare(std::string("vt"))){
            if (part.size() >= 3){
                glm::vec2 tex_coord(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()));
                mesh.tex_coord.push_back(tex_coord);
            } else {
                throw(std::ios_base::failure(std::string("Error: vt command should have exactly 2 parameters")));
            }
        } else if (!part[0].compare(std::string("f"))){
            if (part.size() >= 4){
                if (part.size() > 5){
                    throw(std::ios_base::failure(std::string("Error: f commands with more than 4 vertices not supported")));
                } else if (part.size() == 5){
                    // Break a quad into two triangles
                    Quad quad;
                    for (int i = 0; i < 4; i++){
                        std::vector<std::string> fd = string_split_once(part[i+1], face_separator);
                        if (fd.size() == 1){
                            quad.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            quad.t[i] = -1;
                            quad.n[i] = -1;
                        } else if (fd.size() == 2){
                            quad.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            quad.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            quad.n[i] = -1;
                        } else if (fd.size() == 3){
                            quad.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            if (std::string("").compare(fd[1]) != 0){
                                quad.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            } else {
                                quad.t[i] = -1;
                            }
                            quad.n[i] = str_to_num<float>(fd[2].c_str())-1;
                        } else {
                            throw(std::ios_base::failure(std::string("Error: f parameter should have 1 or 3 parameters separated by '/'")));
                        }
                    }
                    Face face1, face2;
                    face1.i[0] = quad.i[0]; face1.i[1] = quad.i[1]; face1.i[2] = quad.i[2];
                    face1.n[0] = quad.n[0]; face1.n[1] = quad.n[1]; face1.n[2] = quad.n[2];
                    face1.t[0] = quad.t[0]; face1.t[1] = quad.t[1]; face1.t[2] = quad.t[2];
                    face2.i[0] = quad.i[0]; face2.i[1] = quad.i[2]; face2.i[2] = quad.i[3];
                    face2.n[0] = quad.n[0]; face2.n[1] = quad.n[2]; face2.n[2] = quad.n[3];
                    face2.t[0] = quad.t[0]; face2.t[1] = quad.t[2]; face2.t[2] = quad.t[3];
                    mesh.face.push_back(face1);
                    mesh.face.push_back(face2);
                } else if (part.size() == 4){
                    Face face;
                    for (int i = 0; i < 3; i++){
                        std::vector<std::string> fd = string_split_once(part[i+1], face_separator);
                        if (fd.size() == 1){
                            face.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            face.t[i] = -1;
                            face.n[i] = -1;
                        } else if (fd.size() == 2){
                            face.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            face.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            face.n[i] = -1;
                        } else if (fd.size() == 3){
                            face.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            if (std::string("").compare(fd[1]) != 0){
                                face.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            } else {
                                face.t[i] = -1;
                            }
                            face.n[i] = str_to_num<float>(fd[2].c_str())-1;
                        } else {
                            throw(std::ios_base::failure(std::string("Error: f parameter should have 1, 2, or 3 parameters separated by '/'")));
                        }
                    }
                    mesh.face.push_back(face);
                }
            } else {
                throw(std::ios_base::failure(std::string("Error: f command should have 3 or 4 parameters")));
            }
        }
        // Ignore other commands
    }

    // Close file
    f.close();

    // Check if vertex references are correct
    for (unsigned int i = 0; i < mesh.face.size(); i++){
        for (int j = 0; j < 3; j++){
            if (mesh.face[i].i[j] >= mesh.position.size()){
                throw(std::ios_base::failure(std::string("Error: index for triangle ")+num_to_str<int>(mesh.face[i].i[j])+std::string(" is out of bounds")));
            }
        }
    }

    // Compute degree of each vertex
    std::vector<int> degree(mesh.position.size(), 0);
    for (unsigned int i = 0; i < mesh.face.size(); i++){
        for (int j = 0; j < 3; j++){
            degree[mesh.face[i].i[j]]++;
        }
    }

    // Compute vertex normals if no normals were ever added
    if (!added_normal){
        mesh.normal = std::vector<glm::vec3>(mesh.position.size(), glm::vec3(0.0, 0.0, 0.0));
        for (unsigned int i = 0; i < mesh.face.size(); i++){
            // Compute face normal
            glm::vec3 vec1, vec2;
            vec1 = mesh.position[mesh.face[i].i[0]] -
                        mesh.position[mesh.face[i].i[1]];
            vec2 = mesh.position[mesh.face[i].i[0]] -
                        mesh.position[mesh.face[i].i[2]];
            glm::vec3 norm = glm::cross(vec1, vec2);
            norm = glm::normalize(norm);
            // Add face normal to vertices
            mesh.normal[mesh.face[i].i[0]] += norm;
            mesh.normal[mesh.face[i].i[1]] += norm;
            mesh.normal[mesh.face[i].i[2]] += norm;
        }
        for (unsigned int i = 0; i < mesh.normal.size(); i++){
            if (degree[i] > 0){
                mesh.normal[i] /= degree[i];
            }
        }
    }

    // Debug
    //print_mesh(mesh);

    // If we got to this point, the file was parsed successfully and the
    // mesh is in memory
    // Now, transfer the mesh to OpenGL buffers
    // Create three new vertices for each face, in case vertex
    // normals/texture coordinates are not consistent over the mesh

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Create OpenGL buffers and copy data
    GLuint vbo, ebo;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, mesh.face.size() * 3 * vertex_att * sizeof(GLuint), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.face.size() * face_att * sizeof(GLuint), 0, GL_STATIC_DRAW);

    unsigned int vertex_index = 0;
    for (unsigned int i = 0; i < mesh.face.size(); i++){
        // Add three vertices and their attributes
        GLfloat att[3 * vertex_att] = { 0 };
        for (int j = 0; j < 3; j++){
            // Position
            att[j*vertex_att + 0] = mesh.position[mesh.face[i].i[j]][0];
            att[j*vertex_att + 1] = mesh.position[mesh.face[i].i[j]][1];
            att[j*vertex_att + 2] = mesh.position[mesh.face[i].i[j]][2];
            // Normal
            if (!added_normal){
                att[j*vertex_att + 3] = mesh.normal[mesh.face[i].i[j]][0];
                att[j*vertex_att + 4] = mesh.normal[mesh.face[i].i[j]][1];
                att[j*vertex_att + 5] = mesh.normal[mesh.face[i].i[j]][2];
            } else {
                if (mesh.face[i].n[j] >= 0){
                    att[j*vertex_att + 3] = mesh.normal[mesh.face[i].n[j]][0];
                    att[j*vertex_att + 4] = mesh.normal[mesh.face[i].n[j]][1];
                    att[j*vertex_att + 5] = mesh.normal[mesh.face[i].n[j]][2];
                }
            }
            // No color in (6, 7, 8)
            // Texture coordinates
            if (mesh.face[i].t[j] >= 0){
                att[j*vertex_att + 9] = mesh.tex_coord[mesh.face[i].t[j]][0];
                att[j*vertex_att + 10] = mesh.tex_coord[mesh.face[i].t[j]][1];
            }
        }

        // Copy attributes to buffer
        glBufferSubData(GL_ARRAY_BUFFER, i * 3 * vertex_att * sizeof(GLfloat), 3 * vertex_att * sizeof(GLfloat), att);

        // Add triangle
        GLuint findex[face_att] = { 0 };
        findex[0] = vertex_index;
        findex[1] = vertex_index + 1;
        findex[2] = vertex_index + 2;
        vertex_index += 3;

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, i * face_att * sizeof(GLuint), face_att * sizeof(GLuint), findex);
    }

    // Create resource
    AddResource(Mesh, name, vbo, ebo, mesh.face.size() * face_att);
}


void string_trim(std::string str, std::string to_trim){

    // Trim any character in to_trim from the beginning of the string str
    while ((str.size() > 0) && 
           (to_trim.find(str[0]) != std::string::npos)){
        str.erase(0);
    }

    // Trim any character in to_trim from the end of the string str
    while ((str.size() > 0) && 
           (to_trim.find(str[str.size()-1]) != std::string::npos)){
        str.erase(str.size()-1);
    }
}


std::vector<std::string> string_split(std::string str, std::string separator){

    // Initialize output
    std::vector<std::string> output;
    output.push_back(std::string(""));
    int string_index = 0;

    // Analyze string
    unsigned int i = 0;
    while (i < str.size()){
        // Check if character i is a separator
        if (separator.find(str[i]) != std::string::npos){
            // Split string
            string_index++;
            output.push_back(std::string(""));
            // Skip separators
            while ((i < str.size()) && (separator.find(str[i]) != std::string::npos)){
                i++;
            }
        } else {
            // Otherwise, copy string
            output[string_index] += str[i];
            i++;
        }
    }

    return output;
}


std::vector<std::string> string_split_once(std::string str, std::string separator){

    // Initialize output
    std::vector<std::string> output;
    output.push_back(std::string(""));
    int string_index = 0;

    // Analyze string
    unsigned int i = 0;
    while (i < str.size()){
        // Check if character i is a separator
        if (separator.find(str[i]) != std::string::npos){
            // Split string
            string_index++;
            output.push_back(std::string(""));
            // Skip single separator
            i++;
        } else {
            // Otherwise, copy string
            output[string_index] += str[i];
            i++;
        }
    }

    return output;
}


void print_mesh(TriMesh &mesh){

    for (unsigned int i = 0; i < mesh.position.size(); i++){
        std::cout << "v " << 
            mesh.position[i].x << " " <<
            mesh.position[i].y << " " <<
            mesh.position[i].z << std::endl;
    }
    for (unsigned int i = 0; i < mesh.normal.size(); i++){
        std::cout << "vn " <<
            mesh.normal[i].x << " " <<
            mesh.normal[i].y << " " <<
            mesh.normal[i].z << std::endl;
    }
    for (unsigned int i = 0; i < mesh.tex_coord.size(); i++){
        std::cout << "vt " <<
            mesh.tex_coord[i].x << " " <<
            mesh.tex_coord[i].y << std::endl;
    }
    for (unsigned int i = 0; i < mesh.face.size(); i++){
        std::cout << "f " << 
            mesh.face[i].i[0] << " " <<
            mesh.face[i].i[1] << " " <<
            mesh.face[i].i[2] << " " << std::endl;
    }
}


template <typename T> std::string num_to_str(T num){

    std::ostringstream ss;
    ss << num;
    return ss.str();
}


template <typename T> T str_to_num(const std::string &str){

    std::istringstream ss(str);
    T result;
    ss >> result;
    if (ss.fail()){
        throw(std::ios_base::failure(std::string("Invalid number: ")+str));
    }
    return result;
}


void ResourceManager::CreateWall(std::string object_name){

    // Definition of the wall
    // The wall is simply a quad formed with two triangles
    // GLfloat vertex[] = {
    //     // Position, normal, color, texture coordinates
    //     // Here, color stores the tangent of the vertex
    //     -1.0, -1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 0.0,
    //     -1.0,  1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 1.0,
    //      1.0,  1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 1.0,
    //      1.0, -1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 0.0};

    float depth = 2.0;
    GLfloat vertex[] = {
        // Position, normal, color, texture coordinates
        // Here, color stores the tangent of the vertex
        -1.0, -1.0, 0.0,  1.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 0.0,
        -1.0,  1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 1.0,
         1.0,  1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 1.0,
         1.0, -1.0, 0.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 0.0,

        -2.0, -2.0, 10.8,  1.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 0.0,
        -2.0,  2.0, 10.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  0.0, 1.0,
         2.0,  2.0, 10.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 1.0,
         2.0, -2.0, 10.0,  0.0, 0.0,  1.0,  1.0, 0.0, 0.0,  1.0, 0.0
    };
    GLuint face[] = {0, 2, 1,
                     0, 3, 2};

    // Create OpenGL buffers and copy data
    GLuint vbo, ebo;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * 11 * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(GLuint), face, GL_STATIC_DRAW);
 
    // 8 = vertex_num, 3 = vertex_att
    // 2 = face_num, 3 = face_att
    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, 2 * 3);
}


void ResourceManager::CreateSeamlessTorus(std::string object_name, float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples){

    // Create a torus
    // The torus is built from a large loop with small circles around the loop

    // Number of vertices and faces to be created
    // Check the construction algorithm below to understand the numbers
    // specified below
    const GLuint vertex_num = (num_loop_samples+1)*(num_circle_samples+1);
    const GLuint face_num = num_loop_samples*num_circle_samples*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers for the torus
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    float theta, phi; // Angles for circles
    glm::vec3 loop_center;
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;
	float s, t;
    for (int i = 0; i < num_loop_samples+1; i++){ // large loop
        
        theta = 2.0*glm::pi<GLfloat>()*i/num_loop_samples; // loop sample (angle theta)
        loop_center = glm::vec3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

        for (int j = 0; j < num_circle_samples+1; j++){ // small circle
            
            phi = 2.0*glm::pi<GLfloat>()*j/num_circle_samples; // circle sample (angle phi)
			s = theta / (2.0*glm::pi<GLfloat>());
			t = ((phi) / (2.0*glm::pi<GLfloat>()));

			phi += glm::pi<GLfloat>();

            // Define position, normal and color of vertex
            vertex_normal = glm::vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
            vertex_position = loop_center + vertex_normal*circle_radius;
            vertex_color = glm::vec3(1.0 - ((float) i / (float) num_loop_samples), 
                                            (float) i / (float) num_loop_samples, 
                                            (float) j / (float) num_circle_samples);
		
			vertex_coord = glm::vec2(s, t); // good parameterization but seam at last triangle
			// vertex_coord = glm::vec2(fabs(1-2*s),fabs(1-2*t)); // made seamless through mirroring
//			vertex_coord = glm::vec2((rand() % 2000) / 2000.0, (rand() % 2000) / 2000.0);
            // Add vectors to the data buffer
            for (int k = 0; k < 3; k++){
                vertex[(i*(num_circle_samples+1)+j)*vertex_att + k] = vertex_position[k];
                vertex[(i*(num_circle_samples+1)+j)*vertex_att + k + 3] = vertex_normal[k];
                vertex[(i*(num_circle_samples+1)+j)*vertex_att + k + 6] = vertex_color[k];
            }
            vertex[(i*(num_circle_samples+1)+j)*vertex_att + 9] = vertex_coord[0];
            vertex[(i*(num_circle_samples+1)+j)*vertex_att + 10] = vertex_coord[1];
        }
    }

    // Create triangles
    for (int i = 0; i < num_loop_samples; i++){
        for (int j = 0; j < num_circle_samples; j++){
            // Two triangles per quad
			glm::vec3 t1((i + 1)*(num_circle_samples + 1) + j,
				i*(num_circle_samples + 1) + (j + 1),
				i*(num_circle_samples + 1) + j);
			glm::vec3 t2((i + 1)*(num_circle_samples + 1) + j,
				(i + 1)*(num_circle_samples + 1) + (j + 1),
				i*(num_circle_samples + 1) + (j + 1));
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*(num_circle_samples)+j)*face_att*2 + k] = (GLuint) t1[k];
                face[(i*(num_circle_samples)+j)*face_att*2 + k + face_att] = (GLuint) t2[k];
            }
        }
    }

    // Create OpenGL buffers and copy data
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    //glBindVertexArray(vao);

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}


void ResourceManager::CreateTerrain(std::string object_name, float length, float width, int num_length_samples, int num_width_samples){

    // Create a torus
    // The torus is built from a large loop with small circles around the loop

    // Number of vertices and faces to be created
    // Check the construction algorithm below to understand the numbers
    // specified below
    const GLuint vertex_num = num_length_samples*num_width_samples;//(num_length_samples+1)*(num_width_samples+1);
    const GLuint face_num = num_length_samples*num_width_samples*2;

    // Number of attributes for vertices and faces
    const int vertex_att = 11;
    const int face_att = 3;

    // Data buffers for the torus
    GLfloat *vertex = NULL;
    GLuint *face = NULL;

    // Allocate memory for buffers
    try {
        vertex = new GLfloat[vertex_num * vertex_att]; // 11 attributes per vertex: 3D position (3), 3D normal (3), RGB color (3), 2D texture coordinates (2)
        face = new GLuint[face_num * face_att]; // 3 indices per face
    }
    catch  (std::exception &e){
        throw e;
    }

    // Create vertices 
    glm::vec3 vertex_position;
    glm::vec3 vertex_normal;
    glm::vec3 vertex_color;
    glm::vec2 vertex_coord;
	float s, t;
    s = 1.0;
    t = 1.0;

    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(-0.08, 0.08);

    glm::vec3 midpoint = glm::vec3(length/2, width/2, 0);

    std::vector<std::vector<int>> height_map = ReadHeightMap("C:\\Users\\Josh\\School\\Code\\COMP3501\\Project\\Project\\height_map.txt");
    for(int i = 0; i < num_length_samples; i++){
        for(int j = 0; j < num_width_samples; j++){
            vertex_normal = glm::vec3(0,0,1);
            vertex_position = glm::vec3(static_cast<float>(i)* length/num_length_samples, static_cast<float>(j)*width/num_width_samples, -static_cast<float>(height_map[i][j])/15.0f); //distribution(gen));
            // float distance = glm::length(vertex_position - midpoint);
            // vertex_position.z = distance/4;

            vertex_color = glm::vec3(1.0, 1.0, 1.0);
            // vertex_coord = glm::vec2(s,t);
            vertex_coord = glm::vec2(static_cast<float>(i) / num_length_samples, static_cast<float>(j) / num_width_samples);

            int index = i*num_width_samples*vertex_att + j * vertex_att;
            for(int k = 0; k < 3; k++){
                vertex[index + k] = vertex_position[k]; //num_length_samples + 1 ? 
                vertex[index + k + 3] = vertex_normal[k];
                vertex[index + k + 6] = vertex_color[k];
            }
            vertex[index + 9] = vertex_coord[0];
            vertex[index + 10] = vertex_coord[1];
        }
    }
    // for(int i = 0; i < num_length_samples; i++){
    //     for(int j = 0; j < num_width_samples; j++){
    //         std::cout<<"i: " << i << " j: "<< j << " [";
    //         for(int k = 0; k < vertex_att; k++){
    //             std::cout<<vertex[i*vertex_att*num_width_samples + j*vertex_att + k]<<" ";
    //         }std::cout<<"]"<<std::endl;
    //     }
    // }

    for(int i = 0; i < num_length_samples- 1; i++){
        for(int j = 0; j < num_width_samples -1; j++){
            glm::vec3 t1(i * num_width_samples + j,
                     i * num_width_samples + (j + 1),
                     (i + 1) * num_width_samples + j);

            glm::vec3 t2(i * num_width_samples + (j + 1),
                        (i + 1) * num_width_samples + (j + 1),
                        (i + 1) * num_width_samples + j);
            // Add two triangles to the data buffer
            for (int k = 0; k < 3; k++){
                face[(i*(num_width_samples)+j)*face_att*2 + k] = static_cast<GLuint>(t1[k]);
                face[(i*(num_width_samples)+j)*face_att*2 + k + face_att] = static_cast<GLuint>(t2[k]);
            }
        }
    }

    // std::vector<std::vector<int>> heightMap = ReadHeightMap("C:\\Users\\Josh\\School\\Code\\COMP3501\\Project\\Project\\height_map.txt");
    // for (const auto& row : heightMap){
    //     for(int value : row){
    //         std::cout << value << " ";
    //     }
    //         std::cout << std::endl;
    // }

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * vertex_att * sizeof(GLfloat), vertex, GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_num * face_att * sizeof(GLuint), face, GL_STATIC_DRAW);

    // Free data buffers
    delete [] vertex;
    delete [] face;

    // Create resource
    AddResource(Mesh, object_name, vbo, ebo, face_num * face_att);
}

std::vector<std::vector<int>> ResourceManager::ReadHeightMap(const std::string& filename){
    std::vector<std::vector<int>> heightMap;

    std::ifstream file(filename);
    if(!file.is_open()){
        std::cerr << "Error opening file: " << filename << std::endl; 
        exit(0);
    }

    std::string line;
    while(std::getline(file, line)){
        std::vector<int> row; 
        std::istringstream iss(line);

        int value;
        while(iss >> value){
            row.push_back(value);
        }

        heightMap.push_back(row);
    }

    file.close();

    return heightMap;
}
// float ResourceManager::perlin_noise(int x, int y){

// }
} // namespace game;
