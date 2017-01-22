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
float zoom_camera=1,x_change=0,y_change=0;
float BrickLength = 5,BrickFallTIme = 2,BeamLength=10,BeamWidth=2,laserSpeed=4,laserDelayTime=0.5;
float MirrorWidth = 1, MirrorLength = 20, cannonWidth = 5, cannonLength = 25, cannonBaseRadius = 12;
double mouse_pos_x, mouse_pos_y;
double new_mouse_pos_x, new_mouse_pos_y;
double curx=0,cury=0;
double oldx=0,oldy=0;
int life=3,GameScore=0,right_mouse_clicked = 0;
double ref[4],randomx , randomc;

typedef struct Point {
  float x,y;
}Point;
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
  int moveright[2],moveleft[2],grab[2];
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

Bricks MagicBrick;
Buckets buckets;
GLuint programID;
Cannon cannon;

VAO *PartitionLine;
VAO* border;
double lastBeamTime;
///************** COLOURS ***************
static const GLfloat Border_buffer [] = {
  -100,0.5,0,          100,0.5,0,       100,-0.5,0,
  -100,0.5,0,          -100,-0.5,0,     100,-0.5,0,
};
static const GLfloat Mirror_buffer [] = {
  0,0,0,     MirrorLength,0,0,        MirrorLength,MirrorWidth,0,
  0,0,0,     0,MirrorWidth,0,        MirrorLength,MirrorWidth,0,
};
static const GLfloat brick_vertex_buffer [] = {
  2,0,0,  2,-5,0,   -2,-5,0,
  2,0,0,   -2,0,0,   -2,-5,0,
};
static const GLfloat beam_buffer [] = {
  0,-1*BeamWidth/2.0,0,            0,BeamWidth/2.0,0,                   BeamLength,BeamWidth/2.0,0,
  0,-1*BeamWidth/2.0,0,            BeamLength,-1*BeamWidth/2.0,0,       BeamLength,BeamWidth/2.0,0,
};
static const GLfloat cannon_head_buffer [] = {
  0,-1*cannonWidth/2,0,          0,cannonWidth/2,0,             cannonLength,cannonWidth/2,0,
  0,-1*cannonWidth/2,0,          cannonLength,-1*cannonWidth/2,0,         cannonLength,cannonWidth/2,0,
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

/**************************
 * Customizable functions *
 **************************/

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
VAO* createCircle (float r, int NoOfParts, int fill)
{
    int parts = NoOfParts;
    float radius = r;
    GLfloat vertex_buffer_data[parts*9];
    GLfloat color_buffer_data[parts*9];
    int i,j;
    float angle=(2*M_PI/parts);
    float current_angle = 0;
    for(i=0;i<parts;i++){
        for(j=0;j<3;j++){
            color_buffer_data[i*9+j*3]=0;
            color_buffer_data[i*9+j*3+1]=0;
            color_buffer_data[i*9+j*3+2]=1;
        }
        vertex_buffer_data[i*9]=0;
        vertex_buffer_data[i*9+1]=0;
        vertex_buffer_data[i*9+2]=0;
        vertex_buffer_data[i*9+3]=radius*cos(current_angle);
        vertex_buffer_data[i*9+4]=radius*sin(current_angle);
        vertex_buffer_data[i*9+5]=0;
        vertex_buffer_data[i*9+6]=radius*cos(current_angle+angle);
        vertex_buffer_data[i*9+7]=radius*sin(current_angle+angle);
        vertex_buffer_data[i*9+8]=0;
        current_angle+=angle;
    }
    VAO* circle;
    if(fill==1)
        circle = create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_FILL);
    else
        circle = create3DObject(GL_TRIANGLES, (parts*9)/3, vertex_buffer_data, color_buffer_data, GL_LINE);
    return circle;
}


void grabBucket(double x, double y) {
    x=(x/3.0)-100.0; y=100.0-(y/3.0);
    if(y<-50){
      if((x>buckets.posx[1]-20)&&(x<buckets.posx[1]+20)){
        buckets.grab[1]=1;
        oldx=x;oldy=y;
      }
      else if((x>buckets.posx[0]-20)&&(x<buckets.posx[0]+20)){
        buckets.grab[0]=1;
        oldx=x;oldy=y;
      }
    }
}
void check_pan(){
    if(x_change-110.0f/zoom_camera<-110)
        x_change=-110+110.0f/zoom_camera;
    else if(x_change+110.0f/zoom_camera>110)
        x_change=110-110.0f/zoom_camera;
    if(y_change-110.0f/zoom_camera<-110)
        y_change=-110+110.0f/zoom_camera;
    else if(y_change+110.0f/zoom_camera>110)
        y_change=110-110.0f/zoom_camera;
}
void createLaser() {

  double newTime = glfwGetTime();
  if((newTime - lastBeamTime)>laserDelayTime){
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
void AlignCannon(float x, float y){
  x=(x/3.0)-100.0; y=100.0-(y/3.0);
  cannon.angle = (int)((atan((cannon.posy-y)/(cannon.posx-x))/M_PI)* 180.0f);
  createLaser();
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
            if(action == GLFW_RELEASE){
              buckets.grab[0]=0; buckets.grab[1]=0;
            }
            if(action == GLFW_PRESS){
              glfwGetCursorPos(window, &curx, &cury);
              if(cury<450)  AlignCannon(curx,cury);
              grabBucket(curx,cury);
            }
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            if (action == GLFW_RELEASE)
                right_mouse_clicked = 0;
            else if(action == GLFW_PRESS)
                right_mouse_clicked = 1;
            break;
        default:
            break;
    }
}

void mousescroll(GLFWwindow* window, double xoffset, double yoffset){
    if (yoffset==-1) {
        zoom_camera /= 1.1; //make it bigger than current size
    }
    else if(yoffset==1){
        zoom_camera *= 1.1; //make it bigger than current size
    }
    if (zoom_camera<=1) {
        zoom_camera = 1;
    }
    if (zoom_camera>=4) {
        zoom_camera=4;
    }
    if(x_change-100.0f/zoom_camera<-100)
        x_change=-100+100.0f/zoom_camera;
    else if(x_change+100.0f/zoom_camera>100)
        x_change=100-100.0f/zoom_camera;
    if(y_change-100.0f/zoom_camera<-100)
        y_change=-100+100.0f/zoom_camera;
    else if(y_change+100.0f/zoom_camera>100)
        y_change=100-100.0f/zoom_camera;
    Matrices.projection = glm::ortho((float)(-100.0f/zoom_camera+x_change), (float)(100.0f/zoom_camera+x_change), (float)(-100.0f/zoom_camera+y_change), (float)(100.0f/zoom_camera+y_change), 0.1f, 500.0f);
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
  /*
	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0);
	// Store the projection matrix in a variable for future use
    // Perspective projection for 3D views
    // Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

    // Ortho projection for 2D views
    */
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
void draw (GLFWwindow* window){
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

  glfwGetCursorPos(window, &new_mouse_pos_x, &new_mouse_pos_y);
  if(right_mouse_clicked==1){
      x_change+=new_mouse_pos_x-mouse_pos_x;
      y_change-=new_mouse_pos_y-mouse_pos_y;
      check_pan();
  }
  Matrices.projection = glm::ortho((float)(-100.0f/zoom_camera+x_change), (float)(100.0f/zoom_camera+x_change), (float)(-100.0f/zoom_camera+y_change), (float)(100.0f/zoom_camera+y_change), 0.1f, 500.0f);
  glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);


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
  render(border, VP, 0.0f, 0.0f, 100.0f, 0.0f);
  render(border, VP, 0.0f, 0.0f, -100.0f, 0.0f);
  render(border, VP, 90.0f, 100.0f, 0.0f, 0.0f);
  render(border, VP, 90.0f, -100.0f, 0.0f, 0.0f);
  render(PartitionLine,VP,0.0f,0.0f,0.0f,0.0f);

  render(buckets.Bucket_red,VP,0.0f,buckets.posx[0],buckets.posy[0],0.0f);
  render(buckets.Bucket_green,VP,0.0f,buckets.posx[1],buckets.posy[1],0.0f);
  for (mit = mirrors.begin() ; mit != mirrors.end() ; mit++)
  render(mit->object , VP, mit->angle, mit->x, mit->y, 0.0f);
  render(cannon.Base, VP, 0.0f, cannon.posx, cannon.posy, 0.0f);
  for (it = lasers.begin() ; it != lasers.end() ; it++)
    render(it->beam, VP, it->angle, it->x2, it->y2, 0.0f);
  render(cannon.Head, VP, cannon.angle, cannon.posx+1.0, cannon.posy, 0.0f);
  for (bit = bricks.begin(); bit != bricks.end(); bit++)
    render(bit->block, VP, 0.0f, bit->x, bit->y, 0.0f);
  if (MagicBrick.block != NULL){
    render(MagicBrick.block, VP, 0.0f, MagicBrick.x, MagicBrick.y, 0.0f);
    MagicBrick.type = (MagicBrick.type+1)%3;
    MagicBrick.block = create3DObject(GL_TRIANGLES, 6, brick_vertex_buffer, color[MagicBrick.type], GL_FILL);
  }
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
  border = create3DObject(GL_TRIANGLES, 6, Border_buffer, Black_rec, GL_FILL);
  PartitionLine = create3DObject(GL_TRIANGLES, 6, partition_buffer, Black_rec, GL_FILL);
  buckets.posx[0] = -50;buckets.posx[1] = 50;
  buckets.posy[0] = buckets.posy[1] = -71;
  buckets.Bucket_green = create3DObject(GL_TRIANGLES, 6, Bucket_buffer, Green_rec, GL_FILL);
  buckets.Bucket_red = create3DObject(GL_TRIANGLES, 6, Bucket_buffer, Red_rec, GL_FILL);
  //cannon.Base = drawCircle(0,0,0,15.0,10);
  makeMirrors();
  cannon.Base = createCircle(cannonBaseRadius, 180, 2);
  cannon.Head = create3DObject(GL_TRIANGLES, 6, cannon_head_buffer, cannon_head_color, GL_FILL);
}
void MoveBuckets(GLFWwindow* window){
  buckets.posx[0] += buckets.moveright[0]*buckets.dx - buckets.moveleft[0]*buckets.dx;
  buckets.posx[1] += buckets.moveright[1]*buckets.dx - buckets.moveleft[1]*buckets.dx;
  int bucket = 3;
  if(buckets.grab[0]==1) bucket = 0;
  else if (buckets.grab[1]==1) bucket = 1;
    double x,y;
    glfwGetCursorPos(window, &x, &y);
    x=(x/3.0)-100.0; y=100.0-(y/3.0);
    if(bucket<2)
      buckets.posx[bucket]=x;
  if(buckets.posx[0]<=-80) buckets.posx[0] = -80;
  if(buckets.posx[0]>=80) buckets.posx[0] = 80;
  if(buckets.posx[1]<=-80) buckets.posx[1] = -80;
  if(buckets.posx[1]>=80) buckets.posx[1] = 80;
}
void CannonMotions() {
  cannon.posy += cannon.move * cannon.dy;
  cannon.angle += cannon.rotate;
  if(cannon.posy<=-50) cannon.posy = -50; if(cannon.posy>=90) cannon.posy = 90;
  if(cannon.angle>=89) cannon.angle = 89; if(cannon.angle<=-90) cannon.angle = -90;
}
void MoveLasers() {
  double dl;int angle;
  for (it = lasers.begin() ;it != lasers.end() ; it++){
    dl = it->dl; angle = it->angle ;
    it->x1 += dl*cos(angle*M_PI/180.0);it->y1 += dl*sin(angle*M_PI/180.0);
    it->x2 += dl*cos(angle*M_PI/180.0);it->y2 += dl*sin(angle*M_PI/180.0);
    if((it->x1>105) || (it->x1<-105) || (it->y1>105) || (it->y1<-50)){
      it--;
      lasers.erase(it+1);
    }
  }
}
void MoveMagicBrick() {
  if(MagicBrick.block != NULL){
    MagicBrick.y -= 0.2;
    if(MagicBrick.y<=-46){
      if( ((MagicBrick.x>(buckets.posx[0]-19.5))&&((MagicBrick.x<(buckets.posx[0]+19.5)))) || ((MagicBrick.x>(buckets.posx[1]-19.5))&&((MagicBrick.x<(buckets.posx[1]+19.5)))) )
        GameScore += 5;
      else
        GameScore -= 5;
      MagicBrick.block = NULL;
      cout << "Game Score :" << GameScore << "  " << "Lifes Remaining : " << life << endl;
    }
  }
}
void MoveBricks() {
  for(bit = bricks.begin(); bit != bricks.end(); bit++){
    bit->y -= 0.2;
    if(bit->y<=-46){
      if((bit->x>(buckets.posx[bit->type]-19.5))&&((bit->x<(buckets.posx[bit->type]+19.5))))
        GameScore += 1;
      else
        GameScore -= 1;
      if(bit->type == 2){
        life--;
        cout << "Game Score :" << GameScore << "  " << "Lifes Remaining : " << life << endl;
      }
      bricks.erase(bit);
      cout << "Game Score :" << GameScore << "  " << "Lifes Remaining : " << life << endl;
    }
  }
}
void CreateMagicBrick() {
  MagicBrick.x= (rand()%155) - 60; MagicBrick.y = 90; MagicBrick.l = BrickLength; MagicBrick.type = 0;
  MagicBrick.block = create3DObject(GL_TRIANGLES, 6, brick_vertex_buffer, color[MagicBrick.type], GL_FILL);
}
void CreateNewBrick() {
  Brick newBrick;
  newBrick.x= (randomx) - 60; newBrick.y = 90; newBrick.l = BrickLength; newBrick.type = randomc;
  newBrick.block = create3DObject(GL_TRIANGLES, 6, brick_vertex_buffer, color[newBrick.type], GL_FILL);
  bricks.push_back(newBrick);
}
bool Collide(double lx , double ly , int angle , double bx , double by) {

  double lx1 = lx + BeamLength * cos(angle * M_PI / 180.0f), m = tan(angle * M_PI /108.0f);
  double x1 = lx + (BeamWidth/2.0)*sin(angle * M_PI / 180.0f) , x2 = lx - (BeamWidth/2.0)*sin(angle * M_PI / 180.0f);
  double y1 = ly - (BeamWidth/2.0)*cos(angle * M_PI / 180.0f) , y2 = ly + (BeamWidth/2.0)*cos(angle * M_PI / 180.0f);
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
          if(bit->type==2)
            GameScore++;
          else
            GameScore--;
          cout << "Game Score :" << GameScore << "  " << "Lifes Remaining : " << life << endl;
          it--;
          lasers.erase(it+1); bricks.erase(bit);
          break;
        }
        bit++;
    }
    it++;
  }
}
bool Reflection(float lx, float ly, int angle, float mx, float my, int theta ) {

  double lx1 = lx + BeamLength * cos(angle * M_PI / 180.0f), ly1 = ly + BeamLength * sin(angle * M_PI / 180.0f);
  double mm = tan(theta * M_PI /180.0f) , m = tan(angle * M_PI /108.0f);
  double mx1 = mx + MirrorLength * cos(theta * M_PI / 180.0f), my1 = my + MirrorLength * sin(theta * M_PI / 180.0f);
  double c1 = ly - (m * lx) , c2 = my - (mm*mx);

  if( ((ly-(mm*lx)-c2)*(ly1-(mm*lx1)-c2))>0  )
    return false;
  else if (  ((my-(m*mx)-c1)*(my1-(m*mx1)-c1))>0  )
    return false;
  return true;
}
void CheckAllReflection() {
  it = lasers.begin();
  while(it != lasers.end()){
    mit = mirrors.begin();
    int mir=0;
    while( mit != mirrors.end()) {
      if( (Reflection(it->x2,it->y2,it->angle,mit->x,mit->y,mit->angle) == true) && (glfwGetTime()-ref[mir]>1) ){
          it->x2=it->x1 ; it->y2=it->y1;
          it->angle = 2 * mit->angle - it->angle;
          ref[mir]=glfwGetTime();
      }
        mit++;
        mir++;
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
    glfwSetScrollCallback(window, mousescroll); // mouse scroll

    return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height){

  buckets.dx=0.5;buckets.dy=0.5;buckets.moveleft[0]=buckets.moveleft[1]=buckets.moveright[0]=buckets.moveright[1]=0;
  buckets.posx[0] = -50;buckets.posx[1] = 50; buckets.grab[0]=0; buckets.grab[1]=0;
  buckets.posy[0] = buckets.posy[1] = -75;
  cannon.posx = -99.999; cannon.posy = 25; cannon.dy = 0.5; cannon.move = cannon.rotate = 0.0; cannon.angle = 0;
  //color.insert(p1); color.insert(p2);color.insert(p3);
  color[2]=(float*)Black_rec; color[0] = (float*)Red_rec ; color[1] = (float*)Green_rec;
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
    double magic_brick_time = glfwGetTime();
    ref[0]=ref[1]=ref[2]=ref[3]=glfwGetTime();
    glfwGetCursorPos(window, &mouse_pos_x, &mouse_pos_y);
    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        randomx = rand()%155; randomc = rand()%3;
        while(((randomx>20)&&(randomx<50))||((randomx>110)&&(randomx<140))){
          randomx = rand()%155; randomc = rand()%3;
        }
        MoveBuckets(window);
        CannonMotions();
        CheckAllCollisions();
        MoveLasers();
        MoveBricks();
        CheckAllReflection();
        MoveMagicBrick();


        draw(window);
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

        if((current_time - magic_brick_time) >= 15) {
          CreateMagicBrick();
          magic_brick_time = current_time;
        }
        if(life<=0)
          break;
    }
    cout << "Game Over ....!" << endl;
    glfwTerminate();
   exit(EXIT_SUCCESS);
}
