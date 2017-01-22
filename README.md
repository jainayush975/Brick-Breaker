#####Brick Breaker

    This is the brick breaking game build in cpp using openGL library for graphics.
    This is a 2D game.


#####Game description :
  * Game provides 3 lives at start.
  * You need to make your High Score.
  * When ever your lives ends, Game Over.
  * Points are displayed at termial.
  * Lives remaining are displyed at terminal.
  * Game has laser gun which is vertically movable and can shoot at different angles.
  * Laser gun takes 0.5 second to shoot again.
  * Laser is absorbed by the wall of the game window.
  * Game consists of four types of bricks -
        (i)Red Bricks
        (ii)Green Bricks
        (iii)Black Bricks
        (iv)Bonus Bricks(changing color)
  * For red and green bricks there are red and green buckets respectively.
  * If the bricks are collected in the corresponding buckets this will increment the score by 1.
  * If the Black brick crosses the danger line, it will cost one life penalty.
  * If you hit the red or green brick with the laser, it will cost -1.
  * If you hit the Black brick with laser one point is awarded.
  * If Bonus Brick is catched by any bucket you will be awarded with +5 and drops outside any bucket will add -5 on score.
  * There are four mirrors, Laser undergoes reflection from the mirrors it fall upon.

#####Game Controls:

    Keyboard:
      * up , down - To move the red bucket right and left respectively.
      * alt+right,alt+left - To move the green bucket right and left respectively.
      * w , s - To move the laser gun up and down respectively.
      * a , d - To turn laser shoot direction up and down respectively.
      * m , n - To increase and decrease brick fall speed respectively.
      * mouse scroll -  To zoom in and zoom out the game window respectively.
      * right clicked drag - To shift pan to left and right respectively.
      * Spacebar - fire laser.(laser takes 1 second to recharge).

    Mouse:
      * To drag buckets horizontally, player can click on them (highlighted on click) and move the mouse keeping the left mouse pressed.
      * To drag laser gun vertically, player can click on them (highlighted on click) and move the mouse keeping the left mouse pressed.
      * Left click shoots the laser(if the click is in shooting area) in the direction of cursor at the time of click.
      * Scroll Up zooms in the game window.
      * Scroll Down zooms out the game window.
      * To pan the game window, press the right click on game window and drag the window with cursor(keeping the right click pressed).

####Game End:
      * After losing all lives game will be over.
####Cheers enjoy the game!!


####Additional Features
  * Added sound features to make game more fun.
  * Colour changing Bonous Bricks which makes game interesting.
  * Customizable bricks falling speed to make game fun for all.
  * Moving the buckets with the help of mouse.
  * double key controls (ex. alt+right).
  * 4 Mirrors for making game more interesting.
  * Mouse + keyboard controls.

### Dependencies:
##### Linux/Windows/ Mac OSX - Dependencies: (Recommended)
* GLFW
* GLAD
* GLM

##### Linux - Dependencies: (alternative)
* FreeGLUT
* GLEW
* GLM
