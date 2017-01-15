#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;

float BrickLength = 5,BrickFallTIme = 2,BeamLength=10,laserSpeed=4,laserDelayTime=0.5;
float MirrorWidth = 1, MirrorLength = 20;

typedef struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
}VAO;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;
typedef struct Buckets {
  float posx[2],posy[2],dx,dy;
  int moveright[2],moveleft[2];
  VAO *Bucket_red;
  VAO *Bucket_green;
}Buckets;
typedef struct Cannon {
  float posx,posy,dy;
  int move,rotate,angle;
  VAO *Base;
  VAO *Head;
}Cannon;
typedef struct LaserBeam {
  int angle;
  float x1,x2,y1,y2,dl;
  VAO *beam;
}LaserBeam;
typedef struct Brick {
  float x,y,l;
  int type;
  VAO *block;
}Bricks;
typedef struct Mirror {
  int angle;
  float x , y;
  VAO *object;
}Mirror;
vector < LaserBeam> lasers;
vector < LaserBeam>::iterator it;
vector < Brick> bricks;
vector < Bricks>::iterator bit;
vector < Mirror> mirrors;
vector < Mirror>::iterator mit;
Buckets buckets;
GLuint programID;
Cannon cannon;
VAO *PartitionLine;
double lastBeamTime;
///************** COLOURS ***************
static const GLfloat Mirror_buffer [] = {
  0,0,0,     MirrorLength,0,0,        MirrorLength,MirrorWidth,0,
  0,0,0,     0,MirrorWidth,0,        MirrorLength,MirrorWidth,0,
};
static const GLfloat brick_vertex_buffer [] = {
  2,0,0,  2,-5,0,   -2,-5,0,
  2,0,0,   -2,0,0,   -2,-5,0,
};
static const GLfloat beam_buffer [] = {
  0,-2,0, 0,2,0, 10,2,0,
  0,-2,0, 10,-2,0, 10,2,0,
};
static const GLfloat cannon_base_buffer [] = {
  0,0,0, 0,15,0, 7.5,12.99038,0,
  0,0,0, 12.99038,7.5,0, 7.5,12.99038,0,
  0,0,0, 12.99038,7.5,0, 15,0,0,
  0,0,0, 0,-15,0, 7.5,-12.99038,0,
  0,0,0, 12.99038,-7.5,0, 7.5,-12.99038,0,
  0,0,0, 12.99038,-7.5,0, 15,0,0,
};
static const GLfloat cannon_head_buffer [] = {
  0,-3.5,0, 0,3.5,0, 30,3.5,0,
  0,-3.5,0, 30,-3.5,0, 30,3.5,0,
};
static const GLfloat cannon_base_color [] = {
  0,0,1,0,0,1,0,0,1,
  0,0,1,0,0,1,0,0,1,
  0,0,1,0,0,1,0,0,1,
  0,0,1,0,0,1,0,0,1,
  0,0,1,0,0,1,0,0,1,
  0,0,1,0,0,1,0,0,1,
};
static const GLfloat cannon_head_color [] = {
  0,0,1,0,0,1,0,0,1,
  0,0,1,0,0,1,0,0,1,
};
static const GLfloat beam_color [] = {
  1,0,0,1,0,0,1,0,0,
  1,0,0,1,0,0,1,0,0,
};
static const GLfloat partition_buffer [] = {
  -100, -50.5, 0, 100, -50.5, 0, 100, -49.5, 0,
  -100, -50.5, 0, -100, -49.5, 0, 100, -49.5, 0,
};
static const GLfloat Bucket_buffer [] = {
  20,20,0,-20,20,0,-17.5,-17.5,0,
  20,20,0,17.5,-17.5,0,-17.5,-17.5,0,
};
static const GLfloat Black_rec [] = {
  0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,
};
static const GLfloat Red_rec [] = {
  1,0,0,1,0,0,1,0,0,
  1,0,0,1,0,0,1,0,0,
};
static const GLfloat Green_rec [] = {
  0,1,0,0,1,0,0,1,0,
  0,1,0,0,1,0,0,1,0,
};

map < int , GLfloat*> color;

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}
void quit(GLFWwindow *window){
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL){
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    // Should be done after CreateWindow and before any other GL calls
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0.5,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0.5,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL){
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}
void draw3DObject (struct VAO* vao){
  /* Render the VBOs handled by VAO */

    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}
VAO* drawCircle( GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides ){

  int numberOfVertices = numberOfSides + 2;
  GLfloat twicePi = 2.0f * M_PI;

  GLfloat circleVerticesX[numberOfVertices+1];
  GLfloat circleVerticesY[numberOfVertices+1];
  GLfloat circleVerticesZ[numberOfVertices+1];
  cout << numberOfVertices << " numberOfVertices" << endl;
  for ( int i = 0; i < numberOfVertices; i++ )
  {
      circleVerticesX[i] = x + ( radius * cos( i *  twicePi / numberOfSides ) );
      circleVerticesY[i] = y + ( radius * sin( i * twicePi / numberOfSides ) );
      circleVerticesZ[i] = z;
  }
  circleVerticesX[numberOfVertices]=circleVerticesX[0];
  circleVerticesY[numberOfVertices]=circleVerticesY[0];
  circleVerticesZ[numberOfVertices]=circleVerticesZ[0];
  for ( int i = 0; i < numberOfVertices; i++ )
  {
      cout << circleVerticesX[i] << " , ";
      cout << circleVerticesY[i] << " , ";      cout << circleVerticesZ[i] << endl;
  }


  GLfloat allCircleVertices[( numberOfVertices ) * 9], allColor[( numberOfVertices ) * 9];

  for ( int i = 0; i < numberOfVertices; i++ )
  {
      allCircleVertices[i * 3] = 0;allCircleVertices[( i * 3 ) + 1] = 0;allCircleVertices[( i * 3 ) + 2] = 0;
      allCircleVertices[(i * 3) + 3] = circleVerticesX[i];allCircleVertices[( i * 3 ) + 4] = circleVerticesY[i];allCircleVertices[( i * 3 ) + 5] = circleVerticesZ[i];
      allCircleVertices[(i * 3) + 6] = circleVerticesX[i+1];allCircleVertices[( i * 3 ) + 7] = circleVerticesY[i+1];allCircleVertices[( i * 3 ) + 8] = circleVerticesZ[i+1];
      allColor[i*3]=0;allColor[(i*3) + 1] = 0;allColor[(i*3) + 2] = 0;
      allColor[(i*3) + 3]=0;allColor[(i*3) + 4] = 0;allColor[(i*3) + 5] = 0;
      allColor[(i*3) + 6]=0;allColor[(i*3) + 7] = 0;allColor[(i*3) + 8] = 0;
      cout << circleVerticesX[i] << ", " << circleVerticesY[i] << ", " << circleVerticesZ[i] << ", " << circleVerticesX[i+1] << ", " << circleVerticesY[i+1] << ", " << circleVerticesZ[i+1] << ", " << endl;
      cout << allCircleVertices[(i * 3) + 3] << ", ";
      cout << allCircleVertices[( i * 3 ) + 4] << ", " ;
      cout << allCircleVertices[( i * 3 ) + 5] << ", ";
      cout << allCircleVertices[(i * 3) + 6] << ", ";
      cout << allCircleVertices[( i * 3 ) + 7] << ", " ;
      cout << allCircleVertices[( i * 3 ) + 8] << ", " << endl;
  }

    for ( int i = 0; i < numberOfVertices; i++ )
    {
        cout << allCircleVertices[i * 3] << ", ";
        cout << allCircleVertices[( i * 3 ) + 1] << ", " ;
        cout << allCircleVertices[( i * 3 ) + 2] << ", " << endl;
        cout << allCircleVertices[(i * 3) + 3] << ", ";
        cout << allCircleVertices[( i * 3 ) + 4] << ", " ;
        cout << allCircleVertices[( i * 3 ) + 5] << ", " << endl;
        cout << allCircleVertices[(i * 3) + 6] << ", ";
        cout << allCircleVertices[( i * 3 ) + 7] << ", " ;
        cout << allCircleVertices[( i * 3 ) + 8] << ", " << endl;
    }
    cout << "Colour BUffer" << endl ;
    for ( int i = 0; i < numberOfVertices; i++ )
    {
      cout << allColor[i * 3] << ", ";
      cout << allColor[( i * 3 ) + 1] << ", " ;
      cout << allColor[( i * 3 ) + 2] << ", " << endl;
      cout << allColor[(i * 3) + 3] << ", ";
      cout << allColor[( i * 3 ) + 4] << ", " ;
      cout << allColor[( i * 3 ) + 5] << ", " << endl;
      cout << allColor[(i * 3) + 6] << ", ";
      cout << allColor[( i * 3 ) + 7] << ", " ;
      cout << allColor[( i * 3 ) + 8] << ", " << endl;
    }


  VAO *circle = create3DObject(GL_TRIANGLES, numberOfVertices, allCircleVertices, allColor, GL_FILL);

  for ( int i = 0; i < numberOfVertices; i++ )
  {
      cout << allCircleVertices[i * 3] << ", ";
      cout << allCircleVertices[( i * 3 ) + 1] << ", " ;
      cout << allCircleVertices[( i * 3 ) + 2] << ", " << endl;
      cout << allCircleVertices[(i * 3) + 3] << ", ";
      cout << allCircleVertices[( i * 3 ) + 4] << ", " ;
      cout << allCircleVertices[( i * 3 ) + 5] << ", " << endl;
      cout << allCircleVertices[(i * 3) + 6] << ", ";
      cout << allCircleVertices[( i * 3 ) + 7] << ", " ;
      cout << allCircleVertices[( i * 3 ) + 8] << ", " << endl;
  }
  cout << "Colour BUffer" << endl ;
  for ( int i = 0; i < numberOfVertices; i++ )
  {
    cout << allColor[i * 3] << ", ";
    cout << allColor[( i * 3 ) + 1] << ", " ;
    cout << allColor[( i * 3 ) + 2] << ", " << endl;
    cout << allColor[(i * 3) + 3] << ", ";
    cout << allColor[( i * 3 ) + 4] << ", " ;
    cout << allColor[( i * 3 ) + 5] << ", " << endl;
    cout << allColor[(i * 3) + 6] << ", ";
    cout << allColor[( i * 3 ) + 7] << ", " ;
    cout << allColor[( i * 3 ) + 8] << ", " << endl;
  }

  return circle;
}

/**************************
 * Customizable functions *
 **************************/

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;

void createLaser() {

  double newTime = glfwGetTime();
  if((newTime - lastBeamTime)>laserDelayTime)
  {
    lastBeamTime = newTime;
    LaserBeam newlaser;
    newlaser.angle = cannon.angle;
    newlaser.x1 = 30*cos(newlaser.angle * M_PI/180.0f) + cannon.posx; newlaser.y1 = 30*sin(newlaser.angle * M_PI/180.0f) + cannon.posy;
    newlaser.x2 = 20*cos(newlaser.angle * M_PI/180.0f) + cannon.posx; newlaser.y2 = 20*sin(newlaser.angle * M_PI/180.0f) + cannon.posy;
    newlaser.dl = laserSpeed;
    newlaser.beam = create3DObject(GL_TRIANGLES, 6 , beam_buffer , beam_color , GL_FILL);
    lasers.push_back(newlaser);
  }
  return ;
}

/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods){
     // Function is called first on GLFW_PRESS.

    if (action == GLFW_RELEASE) {
        switch (key) {
            case GLFW_KEY_UP:
                buckets.moveright[0]=0;
                break;
            case GLFW_KEY_DOWN:
                buckets.moveleft[0]=0;
                break;
            case GLFW_KEY_RIGHT:
                buckets.moveright[1]=0;
                break;
            case GLFW_KEY_LEFT:
                buckets.moveleft[1]=0;
                break;
            case GLFW_KEY_W: case GLFW_KEY_S:
                cannon.move=0;
                break;

            case GLFW_KEY_A: case GLFW_KEY_D:
                cannon.rotate=0;
                break;
            default:
                break;
        }
    }
    else if (action == GLFW_PRESS) {
        switch (key) {
          case GLFW_KEY_UP:
              buckets.moveright[0]=1;
              break;
          case GLFW_KEY_DOWN:
              buckets.moveleft[0]=1;
              break;
          case GLFW_KEY_RIGHT:
              buckets.moveright[1]=1;
              break;
          case GLFW_KEY_LEFT:
              buckets.moveleft[1]=1;
              break;
          case GLFW_KEY_W:
              cannon.move=1;
              break;
          case GLFW_KEY_S:
              cannon.move=-1;
              break;
          case GLFW_KEY_A:
              cannon.rotate=1;
              break;
          case GLFW_KEY_D:
              cannon.rotate=-1;
              break;
          case GLFW_KEY_SPACE:
              createLaser();
              break;
          default:
              break;
        }
    }
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key){
	switch (key) {
		case 'Q':
		case 'q':
            quit(window);
            break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods){
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_RELEASE)
                triangle_rot_dir *= -1;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE) {
                rectangle_rot_dir *= -1;
            }
            break;
        default:
            break;
    }
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height){
    int fbwidth=width, fbheight=height;
    /* With Retina display on Mac OS X, GLFW's FramebufferSize
     is different from WindowSize */
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 500.0f);
}
float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;


void render(VAO *object , glm::mat4 VP , float angle , float x , float y , float z){
  Matrices.model = glm::mat4(1.0f);
  glm::mat4 translateObject = glm::translate (glm::vec3(x, y, z)); // glTranslatef
  glm::mat4 rotateObject = glm::rotate((float)(angle*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
  glm::mat4 objectTransform = translateObject * rotateObject;
  Matrices.model *= objectTransform;
  glm::mat4 MVP = VP * Matrices.model; // MVP = p * V * M

  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  draw3DObject(object);
}



/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw (){
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);

  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (0, 0, 0);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (0, 1, 0);

  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!
  Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  //  Don't change unless you are sure!!
  glm::mat4 MVP;	// MVP = Projection * View * Model

  render(PartitionLine,VP,0.0f,0.0f,0.0f,0.0f);

  render(buckets.Bucket_red,VP,0.0f,buckets.posx[0],buckets.posy[0],0.0f);
  render(buckets.Bucket_green,VP,0.0f,buckets.posx[1],buckets.posy[1],0.0f);
  for (mit = mirrors.begin() ; mit != mirrors.end() ; mit++)
  render(mit->object , VP, mit->angle, mit->x, mit->y, 0.0f);
  render(cannon.Base, VP, 0.0f, cannon.posx, cannon.posy, 0.0f);
  //drawCircle(buckets.posx[1],buckets.posy[1],0.0f,15.0,362);
  for (it = lasers.begin() ; it != lasers.end() ; it++)
    render(it->beam, VP, it->angle, it->x2, it->y2, 0.0f);
  render(cannon.Head, VP, cannon.angle, cannon.posx+1.0, cannon.posy, 0.0f);
  for (bit = bricks.begin(); bit != bricks.end(); bit++)
    render(bit->block, VP, 0.0f, bit->x, bit->y, 0.0f);
}

void makeMirrors() {
  Mirror newMirror;
  newMirror.x=-40 ; newMirror.y=-40 ; newMirror.angle = 45;
  newMirror.object = create3DObject(GL_TRIANGLES, 6, Mirror_buffer, cannon_head_color, GL_FILL);
  mirrors.push_back(newMirror);
  newMirror.x=50 ; newMirror.y=-5 ; newMirror.angle = 45;
  newMirror.object = create3DObject(GL_TRIANGLES, 6, Mirror_buffer, cannon_head_color, GL_FILL);
  mirrors.push_back(newMirror);
  newMirror.x=-40 ; newMirror.y=80 ; newMirror.angle = -45;
  newMirror.object = create3DObject(GL_TRIANGLES, 6, Mirror_buffer, cannon_head_color, GL_FILL);
  mirrors.push_back(newMirror);
  newMirror.x=50 ; newMirror.y=60 ; newMirror.angle = -45;
  newMirror.object = create3DObject(GL_TRIANGLES, 6, Mirror_buffer, cannon_head_color, GL_FILL);
  mirrors.push_back(newMirror);
}
void makeEnvironment(){
  PartitionLine = create3DObject(GL_TRIANGLES, 6, partition_buffer, Black_rec, GL_FILL);
  buckets.posx[0] = -50;buckets.posx[1] = 50;
  buckets.posy[0] = buckets.posy[1] = -71;
  buckets.Bucket_green = create3DObject(GL_TRIANGLES, 6, Bucket_buffer, Green_rec, GL_FILL);
  buckets.Bucket_red = create3DObject(GL_TRIANGLES, 6, Bucket_buffer, Red_rec, GL_FILL);
  //cannon.Base = drawCircle(0,0,0,15.0,10);
  makeMirrors();
  cannon.Base = create3DObject(GL_TRIANGLES, 18, cannon_base_buffer, cannon_base_color, GL_LINE);
  cannon.Head = create3DObject(GL_TRIANGLES, 6, cannon_head_buffer, cannon_head_color, GL_FILL);
}


void MoveBuckets(){
  buckets.posx[0] += buckets.moveright[0]*buckets.dx - buckets.moveleft[0]*buckets.dx;
  buckets.posx[1] += buckets.moveright[1]*buckets.dx - buckets.moveleft[1]*buckets.dx;
  if(buckets.posx[0]<=-80) buckets.posx[0] = -80;
  if(buckets.posx[0]>=80) buckets.posx[0] = 80;
  if(buckets.posx[1]<=-80) buckets.posx[1] = -80;
  if(buckets.posx[1]>=80) buckets.posx[1] = 80;
}
void CannonMotions() {
  cannon.posy += cannon.move * cannon.dy;
  cannon.angl
  void Reflection(float lx, float ly, int angle, float mx, float my) {e += cannon.rotate;
  if(cannon.posy<=-50) cannon.posy = -50; if(cannon.posy>=90) cannon.posy = 90;
  if(cannon.angle>=89) cannon.angle = 89; if(cannon.angle<=-90) cannon.angle = -90;
}
void MoveLasers() {
  double dl;int angle;
  for (it = lasers.begin() ;it != lasers.end() ; it++){
    dl = it->dl; angle = it->angle ;
    it->x1 += dl*cos(angle*M_PI/180.0);it->y1 += dl*sin(angle*M_PI/180.0);
    it->x2 += dl*cos(angle*M_PI/180.0);it->y2 += dl*sin(angle*M_PI/180.0);
  }
}
void MoveBricks() {
  for(bit = bricks.begin(); bit != bricks.end(); bit++){
    bit->y -= 0.2;
    if(bit->y<=-46)
      bricks.erase(bit);
  }
}
void CreateNewBrick() {
  Brick newBrick;
  newBrick.x= (rand()%155) - 60; newBrick.y = 90; newBrick.l = BrickLength; newBrick.type = rand()%3;
  newBrick.block = create3DObject(GL_TRIANGLES, 6, brick_vertex_buffer, color[newBrick.type], GL_FILL);
  bricks.push_back(newBrick);
}
bool Collide(double lx , double ly , int angle , double bx , double by) {

  double lx1 = lx + BeamLength * cos(angle * M_PI / 180.0f), m = tan(angle * M_PI /108.0f);
  double x1 = lx + sin(angle * M_PI / 180.0f) , x2 = lx - sin(angle * M_PI / 180.0f);
  double y1 = ly - cos(angle * M_PI / 180.0f) , y2 = ly + cos(angle * M_PI / 180.0f);
  double c1 = y1 - (m * x1) , c2 = y2 - (m * x2);

  if( (((lx-bx+2)*(lx1-bx+2))>0) && (((lx-bx-2)*(lx1-bx-2))>0))
    return false;
  else if ( (((by-(m*bx)-c1)*(by-(m*bx)-c1-BrickLength))>0) && (((by-(m*bx)-c2)*(by-(m*bx)-c2-BrickLength))>0) )
    return false;
  else
    return true;
}

void CheckAllCollisions(){
  it = lasers.begin();
  while(it != lasers.end()){
    bit = bricks.begin();
    while( bit != bricks.end()) {
      if(Collide(it->x2 , it->y2 , it->angle , bit->x , bit->y)==true){
          it--;
          lasers.erase(it+1); bricks.erase(bit);
          break;
        }
        bit++;
    }
    it++;
  }
}

bool Reflection(float lx, float ly, int angle, float mx, float my) {
  double lx1 = lx + BeamLength * cos(angle * M_PI / 180.0f), ly1 = ly + BeamLength * sin(angle * M_PI / 180.0f) , m = tan(angle * M_PI /108.0f);
  double mx1 = mx1 + MirrorLength * cos(45 * M_PI / 180.0f), my1 = my + MirrorLength * sin(45 * M_PI / 180.0f);
  double c1 = ly - (m * lx) , c2 = my - mx;

  if( ((ly-lx-c2)*(ly1-lx1-c2))>0  )
    return false;
  else if (  ((my-(m*mx)-c1)*(my1-(m*mx1)-c1))>0  )
    return false;
  else
    return true;
}
void CheckAllReflection() {
  it = lasers.begin();
  while(it != lasers.end()){
    mit = mirrors.begin();
    while( mit != mirrors.end()) {
      if( ){
          it--;
          lasers.erase(it+1); mirrors.erase(mit);
          break;
        }
        mit++;
    }
    it++;
  }
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height){
    GLFWwindow* window; // window desciptor/handle

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Sample OpenGL 3.3 Application", NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval( 1 );

    /* --- register callbacks with GLFW --- */

    /* Register function to handle window resizes */
    /* With Retina display on Mac OS X GLFW's FramebufferSize
     is different from WindowSize */
    glfwSetFramebufferSizeCallback(window, reshapeWindow);
    glfwSetWindowSizeCallback(window, reshapeWindow);

    /* Register function to handle window close */
    glfwSetWindowCloseCallback(window, quit);

    /* Register function to handle keyboard input */
    glfwSetKeyCallback(window, keyboard);      // general keyboard input
    glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

    /* Register function to handle mouse click */
    glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height){

  buckets.dx=0.5;buckets.dy=0.5;buckets.moveleft[0]=buckets.moveleft[1]=buckets.moveright[0]=buckets.moveright[1]=0;
  buckets.posx[0] = -50;buckets.posx[1] = 50;
  buckets.posy[0] = buckets.posy[1] = -75;
  cannon.posx = -99.999; cannon.posy = 25; cannon.dy = 0.5; cannon.move = cannon.rotate = 0.0; cannon.angle = 0;
  //color.insert(p1); color.insert(p2);color.insert(p3);
  color[1]=(float*)Black_rec; color[0] = (float*)Red_rec ; color[2] = (float*)Green_rec;
  makeEnvironment();

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

    // Background color of the scene
	glClearColor (1.0f, 1.0f, 1.0f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;

    GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

    double last_update_time = glfwGetTime(), current_time;

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        MoveBuckets();
        CannonMotions();
        CheckAllCollisions();
        MoveLasers();
        MoveBricks();
        CheckAllReflection();

        draw();

        // Swap Frame Buffer in double buffering
        glfwSwapBuffers(window);

        // Poll for Keyboard and mouse events
        glfwPollEvents();

        // Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
        current_time = glfwGetTime(); // Time in seconds
        if ((current_time - last_update_time) >= BrickFallTIme) {
            CreateNewBrick();
            last_update_time = current_time;
        }
    }

    glfwTerminate();
   exit(EXIT_SUCCESS);
}
