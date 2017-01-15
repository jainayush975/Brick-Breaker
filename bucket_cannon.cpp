#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

class BucketSet {
  private:
    string name;
    COLOR color;
    float x,y;
    VAO* object;
    int status;
float height,width;
float x_speed,y_speed;
float angle; //Current Angle (Actual rotated angle of the object)
int inAir;
float radius;
int fixed;
float friction; //Value from 0 to 1
int health;
int isRotating;
int direction; //0 for clockwise and 1 for anticlockwise for animation
float remAngle; //the remaining angle to finish animation
int isMovingAnim;
int dx;
int dy;
float weight;

  public:

}




class BucketSet
{
  private:
    float posx[2];
    float posy[2];
    static const GLfloat Bucket_buffer [] = {15,15,0,-15,15,0,-15,-15,0,15,15,0,15,-15,0,-15,-15,0,};
    static const GLfloat Red_buc [] = {1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,};
    static const GLfloat Green_buc [] = {0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,};
    float dx;
    float dy;
    VAO *Bucket_red;
    VAO *Bucket_green;
  public:
    BucketSet(float x , float y)
    {
      posx[0]=x[0];posx[1]=x[1];
      posy[0]=y[0];posy[1]=y[1];
      dx = 1;
      dy = 1;
    }

}

static const GLfloat cannon_buffer [] = {
  0,0,0, 0,10,0, 5.0,8.66,0,
  0,0,0, 8.66,5.0,0, 5.0,8.66,0,
  0,0,0, 8.66,5.0,0, 10,0,0,
  0,0,0, 0,-10,0, 5.0,-8.66,0,
  0,0,0, 8.66,-5.0,0, 5.0,-8.66,0,
  0,0,0, 8.66,-5.0,0, 10,0,0,
};

0,0,0, 0,20,0, 10.0,17.3205,0,
0,0,0, 17.3205,10.0,0, 10.0,17.3205,0,
0,0,0, 17.3205,10.0,0, 20,0,0,
0,0,0, 0,-20,0, 10.0,-17.3205,0,
0,0,0, 17.3205,-10.0,0, 10.0,-17.3205,0,
0,0,0, 17.3205,-10.0,0, 20,0,0,



for (it = lasers.begin() ; it != lasers.end() ; it++ )
  for (bit = bricks.begin() ; bit != bricks.end() ; bit++ )
    if(Collide(it->x2 , it->y2 , it->angle , bit->x , bit->y)==true){
        //lasers.erase(it); bricks.erase(bit);
      }
