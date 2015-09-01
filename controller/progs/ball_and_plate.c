#include <GL/glut.h>    // Header File For The GLUT Library 
#include <GL/gl.h>    // Header File For The OpenGL32 Library
#include <GL/glu.h>    // Header File For The GLu32 Library

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <sys/time.h>

#include <ctype.h>
#include <unistd.h>

/******************************************************************************
 * Neural Network Includes
 ******************************************************************************/

#include <kohonen.h>
#include <physical_model.h>
#include <reinforcement.h>



/******************************************************************************
 * DEFINEs
 ******************************************************************************/

#define VERSION "0.1a"

/* acceleration perturbation (by pressing up/down/left/right arrows */
#define ACCELERATION_DISTURB  (2.0*THETA_SENSIBILITY/TIME_TICK/TIME_TICK)/50.0

/* ascii codes for various special keys */
#define ESCAPE 27
#define PAGE_UP 73
#define PAGE_DOWN 81
#define UP_ARROW 72
#define DOWN_ARROW 80
#define LEFT_ARROW 75
#define RIGHT_ARROW 77

/* saturation for the angle view (degree)*/
#define X_SATURATION 15
#define Z_SATURATION 60

/* ratation step (degree) */
#define X_ROTATION 3
#define Z_ROTATION 3




/******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/*
 * Simulation state
 */
t_state x_state;
t_state z_state;
unsigned long int tick = 0; //global tick (~time)
char update_interval = 0;
double factor_range; //resizing factor for the valid range
double delta_t; //keeps trace of the time elapsed sincce the last control
char sim_pause = 1; //system in pause? 1 -> no, 0 -> yes
char show_menu = 0; //0-> display menu, 1->simulation
char blocked_plate = 0; //0-> the plate isn't blocked, 1-> the plate is blocked
double F_x = 0.0; //disturb force along X
double F_y = 0.0; //disturb force along Y


/*
 * Display management
 */
GLfloat x_rot = 0.0;
GLfloat z_rot = 0.0;
int window; // The number of our GLUT window
int fp; // F pressed (1 = yes, 0 = no)
GLfloat q1rot; // x rotation 
GLfloat q2rot; // y rotation 
GLfloat h_plate=0.05f; // plate thickness 
GLfloat h_ring=0.15f; // guard ring height
GLfloat t_ring=0.08f; // guard ring thickness
GLfloat lx_plate=4.0f; // x width plate
GLfloat lz_plate=4.0f; // z width plate
GLfloat bxpos;   // x rotation 
GLfloat bzpos;   // y rotation 
GLfloat ball_radius=0.13f;
GLfloat x_ball_speed=0.05f;
GLfloat z_ball_speed=0.02f;
GLfloat z=-12.0f; // depth into the screen.
GLUquadricObj *quadratic;       // Storage For Our Quadratic Objects
/* white ambient light at half intensity (rgba) */
GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
/* super bright, full intensity diffuse light. */
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
/* position of light (x, y, z, (position of light)) */
GLfloat LightPosition[] = { 0.0f, 0.0f, 5.0f, 1.0f };



/******************************************************************************
 * TEXTURE MANAGEMENT
 ******************************************************************************/

GLuint    texture;

typedef struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
} Image;

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // used to convert bgr to rgb color.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
    {
        printf("File Not Found : %s\n",filename);
        return 0;
    }

    //printf("\n", sizeof(unsigned long));
    image->sizeX = 0;
    image->sizeY = 0;

    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }
    printf("Width of %s: %lu = 0x%lx\n", filename, image->sizeX, image->sizeX);
    
    // read the height 
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }
    printf("Height of %s: %lu\n", filename, image->sizeY);
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
        printf("Error reading bpp from %s.\n", filename);
        return 0;
    }
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }
    
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    printf("Size of image stored in %s is %lu\n", filename, size);
    // read the data. 
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;    
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }

    // we're done.
    return 1;
}

// Load Bitmaps And Convert To Textures
GLvoid LoadGLTextures(GLvoid) {    
    // Load Texture
    Image *image1;
    
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }

    if (!ImageLoad("metal.bmp", image1)) {
        exit(1);
    }        

    // Create Textures    
    glGenTextures(3, &texture);

    // texture 3 (mipmapped scaling)
    glBindTexture(GL_TEXTURE_2D, texture);   // 2d texture (x and y size)
    // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    // scale linearly + mipmap when image smalled than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                                                      GL_LINEAR_MIPMAP_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
                                                GL_UNSIGNED_BYTE, image1->data);

    //2d texture,3 colors, width, height, RGB in that order, byte data, and data
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image1->sizeX, image1->sizeY, GL_RGB,
                                                GL_UNSIGNED_BYTE, image1->data); 
};



/******************************************************************************
 * TEXT MANAGEMENT
 ******************************************************************************/

void print_bitmap_string(void* font, char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutBitmapCharacter(font, *s);
         s++;
      }
   }
}



/******************************************************************************
 * SIMULATION FUNCTIONS
 ******************************************************************************/

/*
 * Execute a simulation step
 * Compute:
 *   1. Kohonen output
 *   2. ASE output
 *   3. update the state
 */
void simulation_step (double delta_t, t_state* state, double F)
{
    int box;
    double input_kn[4];
    double u;
    double var;

    model_state_copy_kn(input_kn, state);

    box = kohonen_get (input_kn);
    u = ase_get (box);

    if (blocked_plate==1)
        var = state->theta;
    else
        var = THETA_SENSIBILITY*u+state->theta;
    model_simulate(state, delta_t, var, F);
}


/*
 * Reset the simulation
 */
void reset()
{
    double x_pos, z_pos;

    x_pos = ((double)rand()/RAND_MAX) * (BALL_RIGHT_LIMIT-BALL_LEFT_LIMIT) + 
                                                                BALL_LEFT_LIMIT;
    z_pos = ((double)rand()/RAND_MAX) * (BALL_RIGHT_LIMIT-BALL_LEFT_LIMIT) +
                                                                BALL_LEFT_LIMIT;
    model_initialize(x_pos, 0, 0, 0, 0, 0, &x_state);
    model_initialize(z_pos, 0, 0, 0, 0, 0, &z_state);

    printf("reset:\n");
    printf("\tx_pos: %f\n", x_pos);
    printf("\tz_pos: %f\n", z_pos);

    q1rot = 0;
    q2rot = 0;
    tick = 0;
}



/******************************************************************************
 * DRAWING FUNCTIONS
 ******************************************************************************/


GLvoid DrawMenu (GLvoid)
{
    char string[256];

    glBegin(GL_QUADS);
    glColor3f(.8f, .8f, .8f);
    glVertex3f( -5.5f, -4.f, -1.0f);
    glVertex3f( -5.5f,  4.f, -1.0f);
    glVertex3f(  5.5f,  4.f, -1.0f);
    glVertex3f(  5.5f, -4.f, -1.0f);
    glEnd();

    glColor3f(.0f, .0f, .0f);
    sprintf(string, "Ball&Plate controller menu:");
    glRasterPos2f(-5., 3.0);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   h: to display/hide this menu");
    glRasterPos2f(-5., 2.0);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   c: to switch between optimal and real sampling");
    glRasterPos2f(-5., 1.5);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   p: to pause the simulation");
    glRasterPos2f(-5., 1.0);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   r: to reset the simulation");
    glRasterPos2f(-5., 0.5);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   w/s: to rotate the view along the X axis");
    glRasterPos2f(-5., 0.0);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   a/d: to rotate the view along the Z axis");
    glRasterPos2f(-5., -0.5);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   up/down/left/right arrows: to disturb the ball");
    glRasterPos2f(-5., -1.0);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "   b: to block/unblock the plate");
    glRasterPos2f(-5., -1.5);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);
}


/*
 * Draw the plate
 */
GLvoid DrawPlate (GLvoid)
{
    glBegin(GL_QUADS);
//    glColor3f(0.0f, 0.0f, 0.0f);
    // top
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f( lx_plate, h_plate, -lz_plate); 
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-lx_plate, h_plate, -lz_plate); 
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-lx_plate, h_plate,  lz_plate); 
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f( lx_plate, h_plate,  lz_plate); 
    glColor3f(1.0f, 0.0f, 0.0f);
    // bottom
    glVertex3f( lx_plate,    0.0f,  lz_plate);
    glVertex3f( lx_plate,    0.0f, -lz_plate);
    glVertex3f(-lx_plate,    0.0f, -lz_plate); 
    glVertex3f(-lx_plate,    0.0f,  lz_plate); 

    // front
    glVertex3f( lx_plate, h_plate,  lz_plate); 
    glVertex3f(-lx_plate, h_plate,  lz_plate); 
    glVertex3f(-lx_plate,    0.0f,  lz_plate); 
    glVertex3f( lx_plate,    0.0f,  lz_plate); 

    // back
    glVertex3f(-lx_plate,    0.0f, -lz_plate);
    glVertex3f(-lx_plate, h_plate, -lz_plate); 
    glVertex3f( lx_plate, h_plate, -lz_plate);
    glVertex3f( lx_plate,    0.0f, -lz_plate); 

    // left
    glVertex3f(-lx_plate,    0.0f,  lz_plate);
    glVertex3f(-lx_plate, h_plate,  lz_plate); 
    glVertex3f(-lx_plate, h_plate, -lz_plate); 
    glVertex3f(-lx_plate,    0.0f, -lz_plate);

    // right
    glVertex3f( lx_plate,    0.0f, -lz_plate);
    glVertex3f( lx_plate, h_plate, -lz_plate);
    glVertex3f( lx_plate, h_plate,  lz_plate); 
    glVertex3f( lx_plate,    0.0f,  lz_plate); 

    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    // ring front
    glBegin(GL_POLYGON);
    glVertex3f(-lx_plate, h_ring+t_ring, lz_plate);
    glVertex3f( lx_plate, h_ring+t_ring, lz_plate);
    glVertex3f( lx_plate,        h_ring, lz_plate);
    glVertex3f(-lx_plate,        h_ring, lz_plate);
    glEnd();

    // ring front
    glBegin(GL_POLYGON);
    glVertex3f(-lx_plate, h_ring+t_ring,-lz_plate);
    glVertex3f( lx_plate, h_ring+t_ring,-lz_plate);
    glVertex3f( lx_plate,        h_ring, -lz_plate);
    glVertex3f(-lx_plate,        h_ring, -lz_plate);
    glEnd();

    // ring left
    glBegin(GL_POLYGON);
    glVertex3f(-lx_plate, h_ring+t_ring, -lz_plate);
    glVertex3f(-lx_plate, h_ring+t_ring,  lz_plate);
    glVertex3f(-lx_plate,        h_ring,  lz_plate);
    glVertex3f(-lx_plate,        h_ring, -lz_plate);
    glEnd();

    // ring right
    glBegin(GL_POLYGON);
    glVertex3f( lx_plate, h_ring+t_ring, -lz_plate);
    glVertex3f( lx_plate, h_ring+t_ring,  lz_plate);
    glVertex3f( lx_plate,        h_ring,  lz_plate);
    glVertex3f( lx_plate,        h_ring, -lz_plate);
    glEnd();
}

/*
 * Write the simulation info
 */
void DrawText ()
{
/*
   char* bitmap_font_names[7] = {
      "GLUT_BITMAP_9_BY_15",
      "GLUT_BITMAP_8_BY_13",
      "GLUT_BITMAP_TIMES_ROMAN_10",
      "GLUT_BITMAP_TIMES_ROMAN_24",
      "GLUT_BITMAP_HELVETICA_10",
      "GLUT_BITMAP_HELVETICA_12",
      "GLUT_BITMAP_HELVETICA_18"     
   };
*/
    GLfloat x, y, ystep, yild;
    char string[256];
    /* Draw the strings, according to the current mode and font. */
    glColor4f(1.0, 1.0, 1.0, 0.0);
    x = -6.;//-225.0;
    y = 6.;//70.0;
    ystep  = 0;//100.0;
    yild   = 0;//20.0;
    glRasterPos2f(-6., 6.);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, "Ball&Plate controller");

    sprintf(string, "  Tick: %ld", tick);
    glRasterPos2f(-6., 5.5);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "  Sampling time: %f s (%s)", delta_t, 
                                         (update_interval==0?"optimal":"real"));
    glRasterPos2f(-6., 5);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "  Valid range: [-%f, %f]", factor_range, factor_range);
    glRasterPos2f(-6., 4.5);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);

    sprintf(string, "  Press h to display the menu");
    glRasterPos2f(-6., 4);
    print_bitmap_string(GLUT_BITMAP_TIMES_ROMAN_24, string);
}

/*
 * Draw the valid area
 */
void DrawConstraintArea ()
{
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f( lx_plate*factor_range, h_plate+0.1, -lz_plate*factor_range);
    glVertex3f(-lx_plate*factor_range, h_plate+0.1, -lz_plate*factor_range);

    glVertex3f(-lx_plate*factor_range, h_plate+0.1, -lz_plate*factor_range);
    glVertex3f(-lx_plate*factor_range, h_plate+0.1,  lz_plate*factor_range);

    glVertex3f(-lx_plate*factor_range, h_plate+0.1,  lz_plate*factor_range);
    glVertex3f( lx_plate*factor_range, h_plate+0.1,  lz_plate*factor_range);

    glVertex3f( lx_plate*factor_range, h_plate+0.1,  lz_plate*factor_range);
    glVertex3f( lx_plate*factor_range, h_plate+0.1, -lz_plate*factor_range);
    glEnd();
}


/*
 * The function called when our window is resized
 * (which shouldn't happen, because we're fullscreen)
 */
GLvoid ReSizeGLScene(GLsizei Width, GLsizei Height)
{
    if (Height==0) // Prevent A Divide By Zero If The Window Is Too Small
        Height=1;

    // Reset The Current Viewport And Perspective Transformation
    glViewport(0, 0, Width, Height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
}




/******************************************************************************
 * INPUT MANAGEMENT
 ******************************************************************************/

/*
 * The function called whenever a special key is pressed.
 * It is used to move the plate and perturb the system
 */
void specialKeyPressed(int key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case GLUT_KEY_UP:
		F_x = -ACCELERATION_DISTURB*sin((-z_rot * 3.14) / 180);
		F_y = -ACCELERATION_DISTURB*cos((-z_rot * 3.14) / 180);
		//x_state.ball_acl -= 200.*sin((-z_rot * 3.14) / 180);
		//z_state.ball_acl -= 200.*cos((-z_rot * 3.14) / 180);
//        z_state.theta += THETA_SENSIBILITY;
//        if (z_state.theta>THETA_RIGHT_LIMIT)
//            z_state.theta = THETA_RIGHT_LIMIT;
        break;
    case GLUT_KEY_DOWN:
		F_x = ACCELERATION_DISTURB*sin((-z_rot * 3.14) / 180);
		F_y = ACCELERATION_DISTURB*cos((-z_rot * 3.14) / 180);
//		x_state.ball_acl += 200.*sin((-z_rot * 3.14) / 180);
//		z_state.ball_acl += 200.*cos((-z_rot * 3.14) / 180);
//        z_state.theta -= THETA_SENSIBILITY;
//        if (z_state.theta<THETA_LEFT_LIMIT)
//            z_state.theta = THETA_LEFT_LIMIT;
        break;
    case GLUT_KEY_LEFT:
		F_x = -ACCELERATION_DISTURB*cos((z_rot * 3.14) / 180);
		F_y = -ACCELERATION_DISTURB*sin((z_rot * 3.14) / 180);
//		x_state.ball_acl -= 200.*cos((z_rot * 3.14) / 180);
//		z_state.ball_acl -= 200.*sin((z_rot * 3.14) / 180);
//        x_state.theta += THETA_SENSIBILITY;
//        if (x_state.theta>THETA_RIGHT_LIMIT)
//                x_state.theta = THETA_RIGHT_LIMIT;
        break;
    case GLUT_KEY_RIGHT:
		F_x = ACCELERATION_DISTURB*cos((z_rot * 3.14) / 180);
		F_y = ACCELERATION_DISTURB*sin((z_rot * 3.14) / 180);
//		x_state.ball_acl += 200.*cos((z_rot * 3.14) / 180);
//		z_state.ball_acl += 200.*sin((z_rot * 3.14) / 180);
//        x_state.theta -= THETA_SENSIBILITY;
//        if (x_state.theta<THETA_LEFT_LIMIT)
//                x_state.theta = THETA_LEFT_LIMIT;
        break;
    default:
        break;
    }
//	printf("Acceleration: %lf, %lf, about ball: %lf, %lf\n", F_x, F_y, x_state.ball_acl, z_state.ball_acl);
}

/*
 * Capture the mosue position
 */
GLvoid mouse (int x, int y)
{
    //GLint m_v[4];
    //glGetIntegerv(GL_VIEWPORT, m_v);

    //q2rot = (3.14/6.0)*(m_v[2]/2.0 - x)/(m_v[2]/2.0);
    //q1rot = (3.14/6.0)*(-m_v[3]/2.0 + y)/(m_v[3]/2.0);

//    fprintf(stdout, "\nx=%d nx=%f rt=%f q2rot=%f over %f",
//           x, m_v[2]/2.0 - x, (m_v[2]/2.0 - x)/(m_v[2]/2.0), q2rot, 3.14/6.0);
//    fprintf(stdout, "\nx=%d nx=%f rt=%f q1rot=%f over %f",
//           y, m_v[3]/2.0 - y, (m_v[3]/2.0 - x)/(m_v[3]/2.0), q1rot, 3.14/6.0);
}

/*
 * The function called whenever a normal key is pressed.
 * It is used to move the point of view of the camera
 */
void keyPressed(unsigned char key, int x, int y) 
{
    /* avoid thrashing this procedure */
    usleep(100);

    switch (key) {    
    case ESCAPE: //shut down
        glutDestroyWindow(window); 
        exit(1);                       
//  case 108: // switch the lighting.
//        printf("L/l pressed; light is: %d\n", light);
        // switch the current value of light, between 0 and 1.
//        light = light ? 0 : 1;
//        if (!light) {
//            glDisable(GL_LIGHTING);
//        } else {
//            glEnable(GL_LIGHTING);
//        }
//        break;
    case 'r': //reset the simulation
    case 'R':
        reset();
        break;
    case 'P':
    case 'p': //pause
        sim_pause = sim_pause==0?1:0;
        break;
    case 'c': //switch between optiomal and real sampling
    case 'C':
        update_interval = update_interval==0?1:0;
        break;
    case 'a': //rotate camera along Z
    case 'A':
        z_rot += Z_ROTATION;
        if (z_rot>Z_SATURATION)
            z_rot = Z_SATURATION;
        break;
    case 'w': //rotate camera along X
    case 'W':
        x_rot += X_ROTATION;
        if (x_rot>X_SATURATION)
            x_rot = X_SATURATION;
        break;
    case 'd': //rotate camera along Z
    case 'D':
        z_rot -= Z_ROTATION;
        if (z_rot<-Z_SATURATION)
            z_rot = -Z_SATURATION;
        break;
    case 's': //rotate camera along X
    case 'S':
        x_rot -= X_ROTATION;
        if (x_rot<-X_SATURATION)
            x_rot = -X_SATURATION;
        break;
    case 'h':
    case 'H':
        show_menu = show_menu==0?1:0;
        break;
    case 'b':
    case 'B':
        blocked_plate = blocked_plate==0?1:0;
        break;
    default:
        break;
    }    
}



/******************************************************************************
 * MAIN DRAWING FUNCTION
 ******************************************************************************/

GLvoid DrawGLScene(GLvoid)
{
    struct timeval curr;
    long seconds, useconds;
    static struct timeval prev;

    gettimeofday(&curr, NULL);

    if (update_interval==0)
        delta_t = TIME_TICK;
    else {
        seconds  = curr.tv_sec - prev.tv_sec;
        useconds = curr.tv_usec - prev.tv_usec;
        delta_t = seconds + useconds/1000000.0;
    }

    if (MAX_REDUCTION_ITERATIONS>tick)
        factor_range = 0.2 + 0.8*(1-(float)tick/MAX_REDUCTION_ITERATIONS);
    else
        factor_range = 0.2;


    if (sim_pause!=0 && show_menu==0) {

        simulation_step(delta_t, &x_state, F_x);
		F_x = 0.0;
        simulation_step(delta_t, &z_state, F_y);
		F_y = 0.0;

        tick++;
        q2rot = x_state.theta;
        q1rot = -z_state.theta;

        if (model_failure(&x_state, tick)||model_failure(&z_state, tick)) {
            printf("failure after %ld ticks\n", tick);
            reset();
        }

        bxpos = x_state.ball_position*10.0;
        bzpos = z_state.ball_position*10.0;

    }

    prev = curr;

    if (update_interval==0)
        usleep(60000);
    else
        usleep(TIME_TICK*1000000);

    // Clear The Screen And The Depth Buffer
    glClearColor(0.5, 0.5, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();            // Reset The View


    if (show_menu==0) {

        glTranslatef(0.0f,-2.0f,z);         // move z units out from the screen.

        DrawText();

        glRotatef(180*q1rot/3.14+x_rot,1.0f,0.0f,0.0f);  // Rotate On The X Axis
        glRotatef(180*q2rot/3.14,0.0f,0.0f,1.0f);    // Rotate On The Y Axis
        glRotatef(z_rot,0.0f,1.0f,0.0f);    // Rotate On The X Axis

        glBindTexture(GL_TEXTURE_2D, texture);

        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        DrawPlate();
        glDisable(GL_TEXTURE_2D);
        DrawConstraintArea();


        glTranslatef( 0.0f, h_plate+ball_radius, 0.0f);

        glTranslatef(bxpos, 0.0f, bzpos);
        glColor3f(1.0f, 1.0f, 1.0f);                   
        gluSphere(quadratic, ball_radius, 32, 32);
    } else {

        glTranslatef(0.0f, 0.0f, z);        // move z units out from the screen.

        DrawMenu();
    }

    //glPopMatrix();
    glutSwapBuffers();

}




/******************************************************************************
 * INITIALIZATION FUNCTION
 ******************************************************************************/

/*
 * A general OpenGL initialization function. Sets all of the initial parameters.
 */
int InitGL(GLsizei Width, GLsizei Height, int *argcp, char **argv, char* title)
{
    int window;

    /* Initialize GLUT state - glut will take any command line arguments 
       that pertain to it or X Windows */
    glutInit(argcp, argv);

    /* Select type of Display mode:
     Double buffer
     RGBA color
     Alpha components supported
     Depth buffer */
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);

    /* get a 640 x 480 window */
    glutInitWindowSize(Width, Height);

    /* the window starts at the upper left corner of the screen */
    glutInitWindowPosition(0, 0);  

    /* Open a window */  
    window = glutCreateWindow(title);

    /* Register the function to do all our OpenGL drawing. */
    glutDisplayFunc(&DrawGLScene);  

    /* Go fullscreen.  This is as soon as possible. */
//    glutFullScreen();

    /* Even if there are no events, redraw our gl scene. */
    glutIdleFunc(&DrawGLScene);

    /* Register the function called when our window is resized. */
    glutReshapeFunc(&ReSizeGLScene);

    /* Register the function called when the keyboard is pressed. */
    glutKeyboardFunc(&keyPressed);

    //Register the function called when special keys (arrows, page down, etc)
    //are pressed.
    glutSpecialFunc(&specialKeyPressed);

    //glutMouseFunc(&mouse);
    glutPassiveMotionFunc(&mouse);
    // load the textures.
    LoadGLTextures();
    // Enable texture mapping.
    glEnable(GL_TEXTURE_2D);
    // This Will Clear The Background Color To Black
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Enables Clearing Of The Depth Buffer
    glClearDepth(1.0);
    // The Type Of Depth Test To Do
    glDisable(GL_DEPTH_TEST);         
    //glDepthFunc(GL_LESS);
    // Enables Depth Testing
    glEnable(GL_DEPTH_TEST);
    // Enables Smooth Color Shading
    glShadeModel(GL_SMOOTH);    
    glMatrixMode(GL_PROJECTION);
    // Reset The Projection Matrix
    glLoadIdentity();    
    // Calculate The Aspect Ratio Of The Window
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    
    glMatrixMode(GL_MODELVIEW);
    // Create A Pointer To The Quadric Object ( NEW )
    quadratic = gluNewQuadric();

    // Can also use GLU_NONE, GLU_FLAT
    // Create Smooth Normals
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    // Create Texture Coords ( NEW )
    gluQuadricTexture(quadratic, GL_TRUE);

    // set up light number 1.
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);  // add lighting. (ambient)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);  // add lighting. (diffuse).
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition); // set light position.
    glEnable(GL_LIGHT1);                             // turn light 1 on.

    return window;
}





/******************************************************************************
 * MAIN FUNCTION
 ******************************************************************************/

int main(int argc, char **argv) 
{  
    struct stat statbuf;

    char *kohonen_filename = NULL;
    char *ase_filename = NULL;
    char run_flag = 0;

    //Parse the parameters
    while (1) {
        char c = getopt (argc, argv, "vk:s:r");
        if (c==-1)
            break;
        switch(c) {
        case 'v':
            printf("Version: %s\n", VERSION);
            exit(0);
        case 'k':
            kohonen_filename = malloc(sizeof(char)*strlen(optarg));
            strcpy(kohonen_filename, optarg);
            printf("kohonen: %s\n", optarg);
            break;
        case 's':
            ase_filename = malloc(sizeof(char)*strlen(optarg));
            strcpy(ase_filename, optarg);
            printf("ase: %s\n", optarg);
            break;
        case 'r':
            run_flag = 1;
            break;
        default:
            break;
        };
    }

    if (run_flag==0 || ase_filename==NULL || stat(ase_filename, &statbuf)==-1 ||
               kohonen_filename==NULL || stat(kohonen_filename, &statbuf)==-1) {
        printf("\\***** BALL and PLATE simulator *****\\\n");
        printf("version: %s\n\n", VERSION);
        printf("Info options:\n");
        printf("-r           :\trun the simulation\n");
        printf("-k [filename]:\tKohonen network to use for input and output\n");
        printf("-s [filename]:\tASE network to use for input and output\n");
        printf("\n If you are working under Cygwin, please make sure to provide double\n");
        printf(" backslashs in the networks path, for instance 'networks//kohonen.nn'.\n");  
        exit(0);
    }

    kohonen_deserialize(kohonen_filename);
    ase_deserialize(ase_filename);

    srand(time(0));
    reset();

    /* Initialize our window. */
    window = InitGL(640, 480, &argc, argv, "Ball&Plate controller");

    /* Start Event Processing Engine */  
    glutMainLoop();  

    return 1;
}

