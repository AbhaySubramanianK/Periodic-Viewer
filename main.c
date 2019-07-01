/*******************************************************************************************


This program displays the shell structure of all known elements found in the periodic
table.

/******************************************************************************************/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <fstream.h>
#include <math.h>
#include <conio.h>
#include <windows.h>
#include <GL/glut.h>

#define PI 3.1415926535898

//function declarations
void display(void);
void idle(void);
void reshape(int w, int h);
void init(void);
void keyboard(unsigned char key, int x, int y);
void build_atom(int atom_val);
void build_stage_x(int stage_num);
void build_stage_y(int stage_num);
void build_stage_z(int stage_num);
void set_rotate_method();
void load_atom(int i);
void main_menu(int value);
void sub_menu(int value);
void display_type_menu(int value);

//global variables
float rotate=0;
int drawn=0, stage_distance=6, speed=1, flat=1, animation=1, current_atom=1, chosen=0;
const double PIXELS_PER_INCH = 50;

GLfloat ruby_ambient[] = {0.1745, 0.01175, 0.01175, 0.55};
GLfloat ruby_diffuse[] = {0.61424, 0.04136, 0.04136, 0.55};
GLfloat ruby_specular[] = {0.727811, 0.626959, 0.626959, 0.55};

GLfloat emerald_ambient[] = {0.0215, 0.1745, 0.0215, 0.55};
GLfloat emerald_diffuse[] = {0.07568, 0.61424, 0.07568, 0.55};
GLfloat emerald_specular[] = {0.633, 0.727811, 0.633, 0.55};

GLfloat grey_ambient[] = {0.3, 0.3, 0.3, 0.55};
GLfloat grey_diffuse[] = {0.3, 0.3, 0.3, 0.55};
GLfloat grey_specular[] = {0.3, 0.3, 0.3, 0.55};

GLfloat red_light_ambient[]= {1.0,0.0,0.0,1.0};
GLfloat red_light_diffuse[]= {1.0,0.0,0.0,1.0};
GLfloat red_light_specular[]={1.0,0.0,0.0,1.0};

GLfloat blue_light_ambient[]= {0.0,1.0,0.0,1.0};
GLfloat blue_light_diffuse[]= {0.0,1.0,0.0,1.0};
GLfloat blue_light_specular[]={0.0,1.0,0.0,1.0};

GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = {25.0, 25.0, 50.0, 0.0};

GLfloat Camera[] = {0.0,0.0,-65.0,0.0,0.0,0.0};  //X,Y,Z,X-Rotate,Y-Rotate,Z-Rotate

//struct used to hold stage coordinates
struct element_stage_info
{
	float elements;
	float rot_x;
	float rot_y;
	float rot_z;
	float x[120];
	float y[120];
	float z[120];
};

struct ProgramState
{
	 int w;
	 int h;
	 GLdouble RotationY;
	 double eye;
	 double zscreen;
	 double znear;
	 double zfar;
	 double RotationIncrement;
	 int solidmodel;
	 bool bStereo;
	 bool bInterlaced;
	 bool bServerMode;
};
struct elements_info
{
 char *name;
 char *symbol;
 float eno[7];
};



struct element_stage_info stage[9];
struct ProgramState ps;
struct elements_info e[116];
void elements_structure(void);
/******************************************************************************
Function: main()
Description: standard main
******************************************************************************/
int main (int argc, char *argv[])
{
	int choice;

    elements_structure();
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);

	choice = IDNO;//MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION);
	if (choice == IDNO)
	{
		glutInitWindowSize(500, 500);			// Windowed Mode
		glutInitWindowPosition(100, 100);
		glutCreateWindow ("ATOMIZER");
	}
	else if(choice == 0)
	{
		glutCreateWindow ("ATOMIZER");
		glutFullScreen();
	}

	init();
	glutDisplayFunc (display);
	glutKeyboardFunc (keyboard);
	glutIdleFunc (idle);
	glutReshapeFunc (reshape);

	glutMainLoop ();

	return 0;
}//end main()

/******************************************************************************
Function: display()
Description: openGL standard display function
******************************************************************************/
void display (void)
{
	double xfactor=1.0, yfactor=1.0;
	double Eye =0.0;
	int i, test;

	if(ps.bStereo)
		test = 2;
	else
		test = 1;


	if(ps.w < ps.h)
	{
		xfactor = 1.0;
		yfactor = ps.h/ps.w;
	}
	else if(ps.h < ps.w)
	{
		xfactor = ps.w/ps.h;
		yfactor = 1.0;
	}

	glClear (GL_COLOR_BUFFER_BIT);


	for(i=0;i<test;i++)
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		if(!ps.bStereo)
		{
			glEnable(GL_LIGHT0);
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			Eye = 0.0;
			glStencilFunc(GL_ALWAYS,1,1);
		}

		else //anaglyph mode
		{
			glEnable(GL_LIGHT0+ i+1);
			glDisable(GL_LIGHT0);
			glStencilFunc(GL_ALWAYS,1,1);
			if(i==0) /* left eye - RED */
			{
				Eye = ps.eye;
				glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_TRUE);
			}
			else	/* right eye - GREEN  <--Change to BLUE for blue lenses  */
			{
				Eye = -ps.eye;
				glColorMask(GL_FALSE,GL_TRUE,GL_FALSE,GL_TRUE);
			}
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(
			(-(ps.w/(2.0*PIXELS_PER_INCH))+Eye)*(ps.znear/ps.zscreen)*xfactor,
			(ps.w/(2.0*PIXELS_PER_INCH)+Eye)*(ps.znear/ps.zscreen)*xfactor,
			-(ps.h/(2.0*PIXELS_PER_INCH))*(ps.znear/ps.zscreen)*yfactor,
			(ps.h/(2.0*PIXELS_PER_INCH))*(ps.znear/ps.zscreen)*yfactor,
			ps.znear, ps.zfar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(Eye,0.0,0.0);
		glTranslated(0,0,-ps.zscreen);
		glTranslatef(Camera[0],Camera[1],Camera[2]);

		glRotatef(Camera[3],.1,0,0);
		glRotatef(Camera[4],0,.1,0);
		glRotatef(Camera[5],0,0,.1);

		if (chosen == 1)
		{
			glPushMatrix();
			if(!ps.bStereo)
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, emerald_specular);
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, emerald_ambient);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, emerald_diffuse);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 76.0);
			}
			else
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, grey_specular);
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey_ambient);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grey_diffuse);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 76.0);
			}

			glutSolidSphere(4.5, 15, 15);
			glPopMatrix();
		}

		//enable electron material
		if(!ps.bStereo)
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ruby_specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ruby_ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ruby_diffuse);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 76.0);
		}
		else
		{
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, grey_specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grey_ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grey_diffuse);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 76.0);
		}

		for (int i=1; stage[i].elements != 0; i++)
		{

			glPushMatrix();
			glRotatef(rotate, stage[i].rot_x, stage[i].rot_y, stage[i].rot_z);
			for (int j=0; j < stage[i].elements; j++)
			{
				glTranslatef(stage[i].x[j], stage[i].y[j], stage[i].z[j]);
				glutSolidSphere(2.0, 10, 10);
				glTranslatef(stage[i].x[j]*(-1), stage[i].y[j]*(-1), stage[i].z[j]*(-1));
			}//end for
			glPopMatrix();

		}//end for
		glDisable(GL_LIGHT0 + i+1);
	}
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glutSwapBuffers ();

}//end display()

/******************************************************************************
Function: idle()
Description: openGL idle function
******************************************************************************/
void idle (void)
{
	if (animation == 1)
		rotate = rotate + speed;

	glutPostRedisplay();

}//end idle()

/******************************************************************************
Function: reshape()
Description: openGL reshape function
******************************************************************************/
void reshape (int w, int h)
{
	ps.w = w;
	ps.h = h;
	glViewport (0.0, 0.0, (GLfloat) w, (GLfloat) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45.0, (GLfloat) w / (GLfloat) h, 1.0, 1000.0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

}//end reshape()

/******************************************************************************
Function: init()
Description: openGL init function
******************************************************************************/
void init(void)
{
	int menuId, sub1, sub2, sub3, sub4;

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	glLightfv(GL_LIGHT0, GL_AMBIENT, white_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT1, GL_AMBIENT, red_light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, red_light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, red_light_specular);
	glLightfv(GL_LIGHT2, GL_AMBIENT, blue_light_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blue_light_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, blue_light_specular);

	//attenuation setup
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.3);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);

	ps.eye=0.80;
	ps.zscreen = 10.0;
	ps.znear = 0.1;
	ps.zfar = 1000000.0;
	ps.RotationY = 0.0;
	ps.RotationIncrement = 4.0;
	ps.solidmodel = 1;
	ps.bInterlaced = true;
	ps.bStereo = false;

	glutCreateMenu(display_type_menu);
	glutAddMenuEntry ("2-D", 1);
	glutAddMenuEntry ("3-D", 2);
	glutAttachMenu (GLUT_LEFT_BUTTON);

	sub1 = glutCreateMenu(sub_menu);
	glutAddMenuEntry ("Hydrogen", 1);
	glutAddMenuEntry ("Helium", 2);
	glutAddMenuEntry ("Lithium", 3);
	glutAddMenuEntry ("Beryllium", 4);
	glutAddMenuEntry ("Boron", 5);
	glutAddMenuEntry ("Carbon", 6);
	glutAddMenuEntry ("Nitrogen", 7);
	glutAddMenuEntry ("Oxygen", 8);
	glutAddMenuEntry ("Fluorine", 9);
	glutAddMenuEntry ("Neon", 10);
	glutAddMenuEntry ("Sodium", 11);
	glutAddMenuEntry ("Magnesium", 12);
	glutAddMenuEntry ("Aluminum", 13);
	glutAddMenuEntry ("Silicon", 14);
	glutAddMenuEntry ("Phosphorus", 15);
	glutAddMenuEntry ("Sulfur", 16);
	glutAddMenuEntry ("Chlorine", 17);
	glutAddMenuEntry ("Argon", 18);
	glutAddMenuEntry ("Potassium", 19);
	glutAddMenuEntry ("Calcium", 20);
	glutAddMenuEntry ("Scandium", 21);
	glutAddMenuEntry ("Titanium", 22);
	glutAddMenuEntry ("Vanadium", 23);
	glutAddMenuEntry ("Chromium", 24);
	glutAddMenuEntry ("Manganese", 25);
	glutAddMenuEntry ("Iron", 26);
	glutAddMenuEntry ("Cobalt", 27);
	glutAddMenuEntry ("Nickel", 28);
	glutAddMenuEntry ("Copper", 29);
	glutAddMenuEntry ("Zinc", 30);

	sub2 = glutCreateMenu(sub_menu);
	glutAddMenuEntry ("Gallium", 31);
	glutAddMenuEntry ("Germanium", 32);
	glutAddMenuEntry ("Arsenic", 33);
	glutAddMenuEntry ("Selenium", 34);
	glutAddMenuEntry ("Bromine", 35);
	glutAddMenuEntry ("Krypton", 36);
	glutAddMenuEntry ("Rubidium", 37);
	glutAddMenuEntry ("Strontium", 38);
	glutAddMenuEntry ("Yttrium", 39);
	glutAddMenuEntry ("Zirconium", 40);
	glutAddMenuEntry ("Niobium", 41);
	glutAddMenuEntry ("Molybdenum", 42);
	glutAddMenuEntry ("Technetium", 43);
	glutAddMenuEntry ("Ruthenium", 44);
	glutAddMenuEntry ("Rhodium", 45);
	glutAddMenuEntry ("Palladium", 46);
	glutAddMenuEntry ("Silver", 47);
	glutAddMenuEntry ("Cadmium", 48);
	glutAddMenuEntry ("Indium", 49);
	glutAddMenuEntry ("Tin", 50);
	glutAddMenuEntry ("Antimony", 51);
	glutAddMenuEntry ("Tellurium", 52);
	glutAddMenuEntry ("Iodine", 53);
	glutAddMenuEntry ("Xenon", 54);
	glutAddMenuEntry ("Cesium", 55);
	glutAddMenuEntry ("Barium", 56);
	glutAddMenuEntry ("Lanthanum", 57);
	glutAddMenuEntry ("Cerium", 58);
	glutAddMenuEntry ("Praseodymium", 59);
	glutAddMenuEntry ("Neodymium", 60);

	sub3 = glutCreateMenu(sub_menu);
	glutAddMenuEntry ("Promethium", 61);
	glutAddMenuEntry ("Samarium", 62);
	glutAddMenuEntry ("Europium", 63);
	glutAddMenuEntry ("Gadolinium", 64);
	glutAddMenuEntry ("Terbium", 65);
	glutAddMenuEntry ("Dysprosium", 66);
	glutAddMenuEntry ("Holmium", 67);
	glutAddMenuEntry ("Erbium", 68);
	glutAddMenuEntry ("Thulium", 69);
	glutAddMenuEntry ("Ytterbium", 70);
	glutAddMenuEntry ("Lutetium", 71);
	glutAddMenuEntry ("Hafnium", 72);
	glutAddMenuEntry ("Tantalum", 73);
	glutAddMenuEntry ("Tungsten", 74);
	glutAddMenuEntry ("Rhenium", 75);
	glutAddMenuEntry ("Osmium", 76);
	glutAddMenuEntry ("Iridium", 77);
	glutAddMenuEntry ("Platinum", 78);
	glutAddMenuEntry ("Gold", 79);
	glutAddMenuEntry ("Mercury", 80);
	glutAddMenuEntry ("Thallium", 81);
	glutAddMenuEntry ("Lead", 82);
	glutAddMenuEntry ("Bismuth", 83);
	glutAddMenuEntry ("Polonium", 84);
	glutAddMenuEntry ("Astatine", 85);
	glutAddMenuEntry ("Radon", 86);
	glutAddMenuEntry ("Francium", 87);
	glutAddMenuEntry ("Radium", 88);
	glutAddMenuEntry ("Actinium", 89);
	glutAddMenuEntry ("Thorium", 90);

	sub4 = glutCreateMenu(sub_menu);
	glutAddMenuEntry ("Protactinium", 91);
	glutAddMenuEntry ("Uranium", 92);
	glutAddMenuEntry ("Neptunium", 93);
	glutAddMenuEntry ("Plutonium", 94);
	glutAddMenuEntry ("Americium", 95);
	glutAddMenuEntry ("Curium", 96);
	glutAddMenuEntry ("Berkelium", 97);
	glutAddMenuEntry ("Californium", 98);
	glutAddMenuEntry ("Einsteinium", 99);
	glutAddMenuEntry ("Fermium", 100);
	glutAddMenuEntry ("Mendelevium", 101);
	glutAddMenuEntry ("Nobelium", 102);
	glutAddMenuEntry ("Lawrencium", 103);
	glutAddMenuEntry ("Rutherfordium", 104);
	glutAddMenuEntry ("Dubnium", 105);
	glutAddMenuEntry ("Bohrium", 106);
	glutAddMenuEntry ("Seaborgium", 107);
	glutAddMenuEntry ("Hassium", 108);
	glutAddMenuEntry ("Meitnerium", 109);
	glutAddMenuEntry ("Ununnilium", 110);
	glutAddMenuEntry ("Unununium", 111);
	glutAddMenuEntry ("Ununbium", 112);
	glutAddMenuEntry ("Ununquadium", 113);
	glutAddMenuEntry ("Ununhexium", 114);
	glutAddMenuEntry ("Ununoctium", 115);
	glutAddMenuEntry ("Not Real Element", 116);

	menuId = glutCreateMenu(main_menu);
	glutAddSubMenu( "Elements 1-30", sub1);
	glutAddSubMenu( "Elements 31-60", sub2);
	glutAddSubMenu( "Elements 61-90", sub3);
	glutAddSubMenu( "Elements 90-117", sub4);
   	glutSetMenu(menuId);
	glutAttachMenu (GLUT_RIGHT_BUTTON);

}//end init()

/******************************************************************************
Function: keyboard()
Description: openGL reshape function
******************************************************************************/
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'q':
		case 'Q':
			exit(0);
		case '.':
			speed=speed+1;
			break;
		case ',':
			if (speed > 0)
				speed--;
			break;
		case 'u':
			animation=1;
			break;
		case 'p':
			animation=0;
			break;
		case 'M':
		case 'm': /* mono */
			ps.bStereo = false;
			ps.eye = 0.0;
			reshape(ps.w, ps.h);
			//reshape(ps.h, ps.w);
			break;
		case 'S':
		case 's': /* stereo */
			ps.bStereo = true;
			ps.eye = 0.80;
			reshape(ps.w, ps.h);
			//reshape(ps.h, ps.w);
			break;
		case '-': /* Camera focus plane foward */
			ps.eye -=.1;
			break;
		case '+': /* Camera focus plane foward */
			ps.eye +=.1;
			break;
		case 'F':
		case 'f': /* Camera focus plane foward */
			ps.zscreen+=.1;
			break;
		case 'V':
		case 'v': /* Camera focus plane foward */
			ps.zscreen-=.1;
			break;
		case 'A': /* Z double step */
			Camera[2]+=.5;
			break;
		case 'a': /* Camera Z foward */
			Camera[2]+= 0.1;
			break;
		case 'Z': /* Z double step backward */
			Camera[2]-= 0.5;
			break;
		case 'z': /* Camera Z backward */
			Camera[2]-= 0.1;
			break;
		case '[': /* Camera Y counter clockwise rotate */
			Camera[4]-= 1;
			break;
		case ']': /* Camera Y clockwise rotate */
			Camera[4]+= 1;
			break;
	}
}//end keyboard

/******************************************************************************
Function: build_atom()
Description: calls build_stage() to build each stage of the atom.
******************************************************************************/
void build_atom(int atom_val)
{
	chosen = 1;					//variable set to 1, to let program know an element
								//has been chosen
	set_rotate_method();		//sets the rotation axis for each stage

	load_atom(atom_val);		//loads in the specified element parameters

	current_atom = atom_val;	//sets the current atom value

	if (flat == 0)
	{
		if (stage[1].elements != 0)
			build_stage_y(1);
		if (stage[2].elements != 0)
			build_stage_x(2);
		if (stage[3].elements != 0)
			build_stage_z(3);
		if (stage[4].elements != 0)
			build_stage_y(4);
		if (stage[5].elements != 0)
			build_stage_x(5);
		if (stage[6].elements != 0)
			build_stage_y(6);
		if (stage[7].elements != 0)
			build_stage_x(7);
	}

	if (flat == 1)
	{
		if (stage[1].elements != 0)
			build_stage_z(1);
		if (stage[2].elements != 0)
			build_stage_z(2);
		if (stage[3].elements != 0)
			build_stage_z(3);
		if (stage[4].elements != 0)
			build_stage_z(4);
		if (stage[5].elements != 0)
			build_stage_z(5);
		if (stage[6].elements != 0)
			build_stage_z(6);
		if (stage[7].elements != 0)
			build_stage_z(7);
	}


}//end build atom()

/******************************************************************************
Function: build_stage_x()
Description: computes the value of each 'electron' in a stage around the x-axis
******************************************************************************/
void build_stage_x(int n)
{
	float angle = .001, angle_size;
	int radius;
	double two_pi = 3.1415926535*2;

	angle_size = 360/stage[n].elements;
	radius = n * stage_distance;

	for (int i=0; i < stage[n].elements; i++)
	{
		if (angle > 180)
		{
			stage[n].y[i] = (cos((two_pi*angle)/360)) * radius;
			stage[n].z[i] = sqrt(( (radius*radius) - (stage[n].y[i]*stage[n].y[i]) ));
			stage[n].x[i] = 0;
		}
		else
		{
			stage[n].y[i] = (cos((two_pi*angle)/360)) * radius;
			stage[n].z[i] = (-1)*sqrt(( (radius*radius) - (stage[n].y[i]*stage[n].y[i]) ));
			stage[n].x[i] = 0;
		}

		angle = angle + angle_size;

	}//end for

}//end build_stage_x()

/******************************************************************************
Function: build_stage_y()
Description: computes the value of each 'electron' in a stage around the y-axis
******************************************************************************/
void build_stage_y(int n)
{
	float angle = .001, angle_size;
	int radius;
	double two_pi = 3.1415926535*2;

	angle_size = 360/stage[n].elements;
	radius = n * stage_distance;

	for (int i=0; i < stage[n].elements && angle <= 360; i++)
	{
		if (angle > 180)
		{
			stage[n].x[i] = (cos((two_pi*angle)/360)) * radius;
			stage[n].z[i] = sqrt(( (radius*radius) - (stage[n].x[i]*stage[n].x[i]) ));
			stage[n].y[i] = 0;
		}
		else
		{
			stage[n].x[i] = (cos((two_pi*angle)/360)) * radius;
			stage[n].z[i] = (-1)*sqrt(( (radius*radius) - (stage[n].x[i]*stage[n].x[i]) ));
			stage[n].y[i] = 0;
		}

		angle = angle + angle_size;

	}//end for

}//end build_stage_y()

/******************************************************************************
Function: build_stage_z()
Description: computes the value of each 'electron' in a stage around the z-axis
******************************************************************************/
void build_stage_z(int n)
{
	float angle = 0.0, angle_size;
	int radius;
	double two_pi = 3.1415926535*2;

	angle_size = 360/stage[n].elements;

	radius = n * stage_distance;

	for (int i=0; i < stage[n].elements; i++)
	{
		if (angle > 180)
		{
			stage[n].y[i] = (cos((two_pi*angle)/360)) * radius;
			stage[n].x[i] = sqrt(( (radius*radius) - (stage[n].y[i]*stage[n].y[i]) ));
			stage[n].z[i] = 0;
		}
		else
		{
			stage[n].y[i] = (cos((two_pi*angle)/360)) * radius;
			stage[n].x[i] = (-1)*sqrt(( (radius*radius) - (stage[n].y[i]*stage[n].y[i]) ));
			stage[n].z[i] = 0;
		}

		angle = angle + angle_size;

	}//end for

}//end build_stage_z()

/******************************************************************************
Function: set_rotate_method()
Description: sets the rotation of each stage
******************************************************************************/
void set_rotate_method()
{
	if (flat == 1)
	{
		stage[1].rot_x = 0.0; stage[1].rot_y = 0.0; stage[1].rot_z = 1.0;
		stage[2].rot_x = 0.0; stage[2].rot_y = 0.0; stage[2].rot_z = -1.0;
		stage[3].rot_x = 0.0; stage[3].rot_y = 0.0; stage[3].rot_z = 1.0;
		stage[4].rot_x = 0.0; stage[4].rot_y = 0.0; stage[4].rot_z = -1.0;
		stage[5].rot_x = 0.0; stage[5].rot_y = 0.0; stage[5].rot_z = 1.0;
		stage[6].rot_x = 0.0; stage[6].rot_y = 0.0; stage[6].rot_z = -1.0;
		stage[7].rot_x = 0.0; stage[7].rot_y = 0.0; stage[7].rot_z = 1.0;
	}

	if (flat == 0)
	{
		stage[1].rot_x = 0.0; stage[1].rot_y = 1.0; stage[1].rot_z = 0.0;
		stage[2].rot_x = 1.0; stage[2].rot_y = 0.0; stage[2].rot_z = 0.0;
		stage[3].rot_x = 0.0; stage[3].rot_y = 0.0; stage[3].rot_z = 1.0;
		stage[4].rot_x = 0.0; stage[4].rot_y = 1.0; stage[4].rot_z = 1.0;
		stage[5].rot_x = 1.0; stage[5].rot_y = 0.0; stage[5].rot_z = 1.0;
		stage[6].rot_x = 0.0; stage[6].rot_y = -1.0; stage[6].rot_z = -1.0;
		stage[7].rot_x = -1.0; stage[7].rot_y = 0.0; stage[7].rot_z = -1.0;
	}

}//end set_rotate_method();

/******************************************************************************
Function: load_atom()
Description: loads the specified atom from the database text file 'elements.db'
******************************************************************************/
void load_atom(int i)
{
	char name[18], symbol[4];

	//ifstream infile;
	//infile.open("elements.db");

	//for (int j=1; j != i; j++)
	//{
	//	infile>>name=;
	//	infile>>symbol;
	//	infile>>stage[1].elements;
	//	infile>>stage[2].elements;
	//	infile>>stage[3].elements;
	//	infile>>stage[4].elements;
	//	infile>>stage[5].elements;
	//	infile>>stage[6].elements;
	//	infile>>stage[7].elements;
//
//	}

	strcpy(name,e[i].name);
	strcpy(symbol,e[i].symbol);
	stage[1].elements=e[i].eno[1];
	stage[2].elements=e[i].eno[2];
	stage[3].elements=e[i].eno[3];
    stage[4].elements=e[i].eno[4];
	stage[5].elements=e[i].eno[5];
	stage[6].elements=e[i].eno[6];
	stage[7].elements=e[i].eno[7];

	//infile.close();

}//end load_atom()

void main_menu(int value)
{

}

/******************************************************************************
Function: sub_menu()
Description: sub menus to select elements
******************************************************************************/
void sub_menu(int value)
{
	build_atom(value);

}//end main_menu

/******************************************************************************
Function: main_menu()
Description: secondary menu toggles 2-D and 3-D
******************************************************************************/
void display_type_menu(int value)
{
	switch(value)
	{
		case 1:
			flat = 1;
			build_atom(current_atom);
			break;
		case 2:
			flat = 0;
			build_atom(current_atom);
			break;
	}
}//end menu_two


void elements_structure()
{
        e[1].name="Hydrogen";
	e[1].symbol="H";
	e[1].eno[1]=1;
	e[1].eno[2]=0;
	e[1].eno[3]=0;
	e[1].eno[4]=0;
	e[1].eno[5]=0;
	e[1].eno[6]=0;
	e[1].eno[7]=0;
	e[2].name="Helium";
	e[2].symbol="He";
	e[2].eno[1]=2;
	e[2].eno[2]=0;
	e[2].eno[3]=0;
	e[2].eno[4]=0;
	e[2].eno[5]=0;
	e[2].eno[6]=0;
	e[2].eno[7]=0;
	e[3].name="Lithium";
	e[3].symbol="Li";
	e[3].eno[1]=2;
	e[3].eno[2]=1;
	e[3].eno[3]=0;
	e[3].eno[4]=0;
	e[3].eno[5]=0;
	e[3].eno[6]=0;
	e[3].eno[7]=0;
        e[4].name="Beryllium";
	e[4].symbol="Be";
	e[4].eno[1]=2;
	e[4].eno[2]=2;
	e[4].eno[3]=0;
	e[4].eno[4]=0;
	e[4].eno[5]=0;
	e[4].eno[6]=0;
	e[4].eno[7]=0;
	e[5].name="Boron";
	e[5].symbol="B";
	e[5].eno[1]=2;
	e[5].eno[2]=3;
	e[5].eno[3]=0;
	e[5].eno[4]=0;
	e[5].eno[5]=0;
	e[5].eno[6]=0;
	e[5].eno[7]=0;
	e[6].name="Carbon";
	e[6].symbol="C";
	e[6].eno[1]=2;
	e[6].eno[2]=4;
	e[6].eno[3]=0;
	e[6].eno[4]=0;
	e[6].eno[5]=0;
	e[6].eno[6]=0;
	e[6].eno[7]=0;
        e[7].name="Nitrogen";
	e[7].symbol="N";
	e[7].eno[1]=2;
	e[7].eno[2]=5;
	e[7].eno[3]=0;
	e[7].eno[4]=0;
	e[7].eno[5]=0;
	e[7].eno[6]=0;
	e[7].eno[7]=0;
        e[8].name="Oxygen";
	e[8].symbol="O";
	e[8].eno[1]=2;
	e[8].eno[2]=6;
	e[8].eno[3]=0;
	e[8].eno[4]=0;
	e[8].eno[5]=0;
	e[8].eno[6]=0;
	e[8].eno[7]=0;
        e[9].name="Fluorin";
	e[9].symbol="F";
	e[9].eno[1]=2;
	e[9].eno[2]=7;
	e[9].eno[3]=0;
	e[9].eno[4]=0;
	e[9].eno[5]=0;
	e[9].eno[6]=0;
	e[9].eno[7]=0;
        e[10].name="Neon";
	e[10].symbol="Ne";
	e[10].eno[1]=2;
	e[10].eno[2]=8;
	e[10].eno[3]=0;
	e[10].eno[4]=0;
	e[10].eno[5]=0;
	e[10].eno[6]=0;
	e[10].eno[7]=0;
        e[11].name="Sodium";
	e[11].symbol="Na";
	e[11].eno[1]=2;
	e[11].eno[2]=8;
	e[11].eno[3]=1;
	e[11].eno[4]=0;
	e[11].eno[5]=0;
	e[11].eno[6]=0;
	e[11].eno[7]=0;
        e[12].name="Magnesium";
	e[12].symbol="Mg";
	e[12].eno[1]=2;
	e[12].eno[2]=8;
	e[12].eno[3]=2;
	e[12].eno[4]=0;
	e[12].eno[5]=0;
	e[12].eno[6]=0;
	e[12].eno[7]=0;
        e[13].name="Aluminum";
	e[13].symbol="Al";
	e[13].eno[1]=2;
	e[13].eno[2]=8;
	e[13].eno[3]=3;
	e[13].eno[4]=0;
	e[13].eno[5]=0;
	e[13].eno[6]=0;
	e[13].eno[7]=0;
        e[14].name="Silicon";
	e[14].symbol="Si";
	e[14].eno[1]=2;
	e[14].eno[2]=8;
	e[14].eno[3]=4;
	e[14].eno[4]=0;
	e[14].eno[5]=0;
	e[14].eno[6]=0;
	e[14].eno[7]=0;
        e[15].name="Phosphorus";
	e[15].symbol="P";
	e[15].eno[1]=2;
	e[15].eno[2]=8;
	e[15].eno[3]=5;
	e[15].eno[4]=0;
	e[15].eno[5]=0;
	e[15].eno[6]=0;
	e[15].eno[7]=0;
        e[16].name="Sulfur";
	e[16].symbol="S";
	e[16].eno[1]=2;
	e[16].eno[2]=8;
	e[16].eno[3]=6;
	e[16].eno[4]=0;
	e[16].eno[5]=0;
	e[16].eno[6]=0;
	e[16].eno[7]=0;
        e[17].name="Chlorine";
	e[17].symbol="Cl";
	e[17].eno[1]=2;
	e[17].eno[2]=8;
	e[17].eno[3]=7;
	e[17].eno[4]=0;
	e[17].eno[5]=0;
	e[17].eno[6]=0;
	e[17].eno[7]=0;
        e[18].name="Argon";
	e[18].symbol="Ar";
	e[18].eno[1]=2;
	e[18].eno[2]=8;
	e[18].eno[3]=8;
	e[18].eno[4]=0;
	e[18].eno[5]=0;
	e[18].eno[6]=0;
	e[18].eno[7]=0;
        e[19].name="Potassium";
	e[19].symbol="K";
	e[19].eno[1]=2;
	e[19].eno[2]=8;
	e[19].eno[3]=8;
	e[19].eno[4]=1;
	e[19].eno[5]=0;
	e[19].eno[6]=0;
	e[19].eno[7]=0;
        e[20].name="Calcium";
	e[20].symbol="Ca";
	e[20].eno[1]=2;
	e[20].eno[2]=8;
	e[20].eno[3]=8;
	e[20].eno[4]=2;
	e[20].eno[5]=0;
	e[20].eno[6]=0;
	e[20].eno[7]=0;
        e[21].name="Scandium";
	e[21].symbol="Sc";
	e[21].eno[1]=2;
	e[21].eno[2]=8;
	e[21].eno[3]=9;
	e[21].eno[4]=2;
	e[21].eno[5]=0;
	e[21].eno[6]=0;
	e[21].eno[7]=0;
        e[22].name="Titanium";
	e[22].symbol="Ti";
	e[22].eno[1]=2;
	e[22].eno[2]=8;
	e[22].eno[3]=10;
	e[22].eno[4]=2;
	e[22].eno[5]=0;
	e[22].eno[6]=0;
	e[22].eno[7]=0;
        e[23].name="Vanadium";
	e[23].symbol="V";
	e[23].eno[1]=2;
	e[23].eno[2]=8;
	e[23].eno[3]=11;
	e[23].eno[4]=2;
	e[23].eno[5]=0;
	e[23].eno[6]=0;
	e[23].eno[7]=0;
        e[24].name="Chromium";
	e[24].symbol="Cr";
	e[24].eno[1]=2;
	e[24].eno[2]=8;
	e[24].eno[3]=13;
	e[24].eno[4]=1;
	e[24].eno[5]=0;
	e[24].eno[6]=0;
	e[24].eno[7]=0;
        e[25].name="Manganese";
	e[25].symbol="Mn";
	e[25].eno[1]=2;
	e[25].eno[2]=8;
	e[25].eno[3]=13;
	e[25].eno[4]=2;
	e[25].eno[5]=0;
	e[25].eno[6]=0;
	e[25].eno[7]=0;
        e[26].name="Iron";
	e[26].symbol="Fe";
	e[26].eno[1]=2;
	e[26].eno[2]=8;
	e[26].eno[3]=14;
	e[26].eno[4]=3;
	e[26].eno[5]=0;
	e[26].eno[6]=0;
	e[26].eno[7]=0;
        e[27].name="Cobalt";
	e[27].symbol="Co";
	e[27].eno[1]=2;
	e[27].eno[2]=8;
	e[27].eno[3]=15;
	e[27].eno[4]=2;
	e[27].eno[5]=0;
	e[27].eno[6]=0;
	e[27].eno[7]=0;
        e[28].name="Nickel";
	e[28].symbol="Ni";
	e[28].eno[1]=2;
	e[28].eno[2]=8;
	e[28].eno[3]=16;
	e[28].eno[4]=2;
	e[28].eno[5]=0;
	e[28].eno[6]=0;
	e[28].eno[7]=0;
        e[29].name="Copper";
	e[29].symbol="Cu";
	e[29].eno[1]=2;
	e[29].eno[2]=8;
	e[29].eno[3]=18;
	e[29].eno[4]=1;
	e[29].eno[5]=0;
	e[29].eno[6]=0;
	e[29].eno[7]=0;
        e[30].name="Zinc";
	e[30].symbol="Zn";
	e[30].eno[1]=2;
	e[30].eno[2]=8;
	e[30].eno[3]=18;
	e[30].eno[4]=2;
	e[30].eno[5]=0;
	e[30].eno[6]=0;
	e[30].eno[7]=0;
        e[31].name="Gallium";
	e[31].symbol="Ga";
	e[31].eno[1]=2;
	e[31].eno[2]=8;
	e[31].eno[3]=18;
	e[31].eno[4]=3;
	e[31].eno[5]=0;
	e[31].eno[6]=0;
	e[31].eno[7]=0;
        e[32].name="Germanium";
	e[32].symbol="Ge";
	e[32].eno[1]=2;
	e[32].eno[2]=8;
	e[32].eno[3]=18;
	e[32].eno[4]=4;
	e[32].eno[5]=0;
	e[32].eno[6]=0;
	e[32].eno[7]=0;
        e[33].name="Arsenic";
	e[33].symbol="As";
	e[33].eno[1]=2;
	e[33].eno[2]=8;
	e[33].eno[3]=18;
	e[33].eno[4]=5;
	e[33].eno[5]=0;
	e[33].eno[6]=0;
	e[33].eno[7]=0;
        e[34].name="Selenium";
	e[34].symbol="Se";
	e[34].eno[1]=2;
	e[34].eno[2]=8;
	e[34].eno[3]=18;
	e[34].eno[4]= 6;
	e[34].eno[5]=0;
	e[34].eno[6]=0;
	e[34].eno[7]=0;
        e[35].name="Bromine";
	e[35].symbol="Br";
	e[35].eno[1]=2;
	e[35].eno[2]=8;
	e[35].eno[3]=18;
	e[35].eno[4]=7;
	e[35].eno[5]=0;
	e[35].eno[6]=0;
	e[35].eno[7]=0;
        e[36].name="Krypton";
	e[36].symbol="Kr";
	e[36].eno[1]=2;
	e[36].eno[2]=8;
	e[36].eno[3]=18;
	e[36].eno[4]=8;
	e[36].eno[5]=0;
	e[36].eno[6]=0;
	e[36].eno[7]=0;
        e[37].name="Rubidium";
	e[37].symbol="Rb";
	e[37].eno[1]=2;
	e[37].eno[2]=8;
	e[37].eno[3]=18;
	e[37].eno[4]=8;
	e[37].eno[5]=1;
	e[37].eno[6]=0;
	e[37].eno[7]=0;
        e[38].name="Strontium";
	e[38].symbol="Sr";
	e[38].eno[1]=2;
	e[38].eno[2]=8;
	e[38].eno[3]=18;
	e[38].eno[4]=8;
	e[38].eno[5]=2;
	e[38].eno[6]=0;
	e[38].eno[7]=0;
        e[39].name="Yttrium";
	e[39].symbol="Y";
	e[39].eno[1]=2;
	e[39].eno[2]=8;
	e[39].eno[3]=18;
	e[39].eno[4]=9;
	e[39].eno[5]=2;
	e[39].eno[6]=0;
	e[39].eno[7]=0;
        e[40].name="Zirconium";
	e[40].symbol="Zr";
	e[40].eno[1]=2;
	e[40].eno[2]=8;
	e[40].eno[3]=18;
	e[40].eno[4]=10;
	e[40].eno[5]=2;
	e[40].eno[6]=0;
	e[40].eno[7]=0;
        e[41].name="Niobium";
	e[41].symbol="Nb";
	e[41].eno[1]=2;
	e[41].eno[2]=8;
	e[41].eno[3]=18;
	e[41].eno[4]=12;
	e[41].eno[5]=1;
	e[41].eno[6]=0;
	e[41].eno[7]=0;
        e[42].name="Molybdenum";
	e[42].symbol="Mo";
	e[42].eno[1]=2;
	e[42].eno[2]=8;
	e[42].eno[3]=18;
	e[42].eno[4]=13;
	e[42].eno[5]=1;
	e[42].eno[6]=0;
	e[42].eno[7]=0;
        e[43].name="Technetium";
	e[43].symbol="Tc";
	e[43].eno[1]=2;
	e[43].eno[2]=8;
	e[43].eno[3]=18;
	e[43].eno[4]=14;
	e[43].eno[5]=1;
	e[43].eno[6]=0;
	e[43].eno[7]=0;
        e[44].name="Ruthenium";
	e[44].symbol="Ru";
	e[44].eno[1]=2;
	e[44].eno[2]=8;
	e[44].eno[3]=18;
	e[44].eno[4]=15;
	e[44].eno[5]=1;
	e[44].eno[6]=0;
	e[44].eno[7]=0;
        e[45].name="Rhodium";
	e[45].symbol="Rh";
	e[45].eno[1]=2;
	e[45].eno[2]=8;
	e[45].eno[3]=18;
	e[45].eno[4]=16;
	e[45].eno[5]=1;
	e[45].eno[6]=0;
	e[45].eno[7]=0;
        e[46].name="Palladium";
	e[46].symbol="Pd";
	e[46].eno[1]=2;
	e[46].eno[2]=8;
	e[46].eno[3]=18;
	e[46].eno[4]=18;
	e[46].eno[5]=0;
	e[46].eno[6]=0;
	e[46].eno[7]=0;
        e[47].name="Silver";
	e[47].symbol="Ag";
	e[47].eno[1]=2;
	e[47].eno[2]=8;
	e[47].eno[3]=18;
	e[47].eno[4]=18;
	e[47].eno[5]=1;
	e[47].eno[6]=0;
	e[47].eno[7]=0;
        e[48].name="Cadmium";
	e[48].symbol="Cd";
	e[48].eno[1]=2;
	e[48].eno[2]=8;
	e[48].eno[3]=18;
	e[48].eno[4]=18;
	e[48].eno[5]=2;
	e[48].eno[6]=0;
	e[48].eno[7]=0;
        e[49].name="Indium";
	e[49].symbol="In";
	e[49].eno[1]=2;
	e[49].eno[2]=8;
	e[49].eno[3]=18;
	e[49].eno[4]=18;
	e[49].eno[5]=3;
	e[49].eno[6]=0;
	e[49].eno[7]=0;
        e[50].name="Tin";
	e[50].symbol="Sn";
	e[50].eno[1]=2;
	e[50].eno[2]=8;
	e[50].eno[3]=18;
	e[50].eno[4]=18;
	e[50].eno[5]=4;
	e[50].eno[6]=0;
	e[50].eno[7]=0;
        e[51].name="Antimony";
	e[51].symbol="Sb";
	e[51].eno[1]=2;
	e[51].eno[2]=8;
	e[51].eno[3]=18;
	e[51].eno[4]=18;
	e[51].eno[5]=5;
	e[51].eno[6]=0;
	e[51].eno[7]=0;
        e[52].name="Tellurium";
	e[52].symbol="Te";
	e[52].eno[1]=2;
	e[52].eno[2]=8;
	e[52].eno[3]=18;
	e[52].eno[4]=18;
	e[52].eno[5]=6;
	e[52].eno[6]=0;
	e[52].eno[7]=0;
        e[53].name="Iodine";
	e[53].symbol="I";
	e[53].eno[1]=2;
	e[53].eno[2]=8;
	e[53].eno[3]=18;
	e[53].eno[4]=18;
	e[53].eno[5]=7;
	e[53].eno[6]=0;
	e[53].eno[7]=0;
        e[54].name="Xenon";
	e[54].symbol="Xe";
	e[54].eno[1]=2;
	e[54].eno[2]=8;
	e[54].eno[3]=18;
	e[54].eno[4]=18;
	e[54].eno[5]=8;
	e[54].eno[6]=0;
	e[54].eno[7]=0;
        e[55].name="Cesium";
	e[55].symbol="Cs";
	e[55].eno[1]=2;
	e[55].eno[2]=8;
	e[55].eno[3]=18;
	e[55].eno[4]=18;
	e[55].eno[5]=8;
	e[55].eno[6]=1;
	e[55].eno[7]=0;
        e[56].name="Barium";
	e[56].symbol="Ba";
	e[56].eno[1]=2;
	e[56].eno[2]=8;
	e[56].eno[3]=18;
	e[56].eno[4]=18;
	e[56].eno[5]=8;
	e[56].eno[6]=2;
	e[56].eno[7]=0;
        e[57].name="Lanthanum";
	e[57].symbol="La";
	e[57].eno[1]=2;
	e[57].eno[2]=8;
	e[57].eno[3]=18;
	e[57].eno[4]=18;
	e[57].eno[5]=9;
	e[57].eno[6]=2;
	e[57].eno[7]=0;
        e[58].name="Cerium";
	e[58].symbol="Ce";
	e[58].eno[1]=2;
	e[58].eno[2]=8;
	e[58].eno[3]=18;
	e[58].eno[4]=20;
	e[58].eno[5]=8;
	e[58].eno[6]=2;
	e[58].eno[7]=0;
        e[59].name="Praseodymium";
	e[59].symbol="Pr";
	e[59].eno[1]=2;
	e[59].eno[2]=8;
	e[59].eno[3]=18;
	e[59].eno[4]=21;
	e[59].eno[5]=8;
	e[59].eno[6]=2;
	e[59].eno[7]=0;
    e[60].name="Neodymium";
	e[60].symbol="Nd";
	e[60].eno[1]=2;
	e[60].eno[2]=8;
	e[60].eno[3]=18;
	e[60].eno[4]=22;
	e[60].eno[5]=8;
	e[60].eno[6]=2;
	e[60].eno[7]=0;

}
