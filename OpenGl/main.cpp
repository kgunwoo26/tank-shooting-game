#include <GL/glut.h>
#include <math.h>
#include <vector>
using namespace std;

int MX = 300;
int MY = 300;
float time = 0;  // 물체 좌우 흔들림
float r = 5; // 총구 각도
float x = 0; // 탱크의 좌표(전역변수)
int Life = 10;   //태양 라이프 수
float color1 = 0.6;  
float color2 = 0.4;
float color3 = 0;      //태양 색
float barColor =0.2;     
float barColor2 =0.5;    //라이프 바 색
int stage = 0;           //스테이지 레벨
float timeIncrease= 3;     // 태양 움직임 속도
float posLevelup = 100;    //level up의 위치
float posClear = 100;
float posSun = 0;
float shake = 0;


void drawLevelUp();

class vec3 {
public:
	float m[3];
	vec3(float x = 0, float y = 0, float z = 0) {
		m[0] = x; m[1] = y; m[2] = z;
	}
	vec3 operator+(vec3 x) {
		return vec3(m[0] + x.m[0],
			m[1] + x.m[1],
			m[2] + x.m[2]);
	}
	vec3 operator-(vec3 x) {
		return vec3(m[0] - x.m[0],
			m[1] - x.m[1],
			m[2] - x.m[2]);
	}
	vec3 operator*(float x) {
		return vec3(m[0] * x, m[1] * x, m[2] * x);
	}
	float Length() {
		return sqrtf(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
	}
};
class Missile {
public:
	vec3 v;
	vec3 pos;
};
vector<Missile> Missiles;

void MyMouseMove(GLint X, GLint Y) {
	MX = X;
	MY = Y;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(MX / 300.0, MY / 300.0, 1.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);  //시점변환
	glutPostRedisplay();
}

void MyTimer(int value) {
	color1 = 0.6;
	color2 = 0.4;
	color3 = 0;
	glutPostRedisplay();
}

void ClearTimer(int value) {
	posClear = -3;
}

void MyMoveTimer(int value) {
	posLevelup = 100;
	glutPostRedisplay();
}

void hitColor() {
	color1 = color2 = color3 = 1;
	glutTimerFunc(100, MyTimer, 1);
}

void MySpecial(int key, int X, int Y) { //x는 탱크위치, r은 총구 각도
	if (key == GLUT_KEY_UP) {
		r += 0.5;
	}
	if (key == GLUT_KEY_DOWN) {
		r -= 0.5;
	}
	if (key == GLUT_KEY_LEFT) {
		x -= 0.5f;
	}
	if (key == GLUT_KEY_RIGHT) {
		x += 0.5f;
	}

	glutPostRedisplay();
}



void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
	if (KeyPressed == ' ') { //spacebar 
		Missile sb;
		sb.pos.m[0] = x ; // 탱크의 좌표 x
		sb.pos.m[1] = 2.8;
		sb.pos.m[2] = 12;
		sb.v.m[0] = cos(r* 0.31415926535897932)*2;
		sb.v.m[1] = 0;
		sb.v.m[2] = -sin(r * 0.31415926535897932)*2;
		Missiles.push_back(sb);

		glutPostRedisplay();
	}
}


void MyIdle() {
	time += timeIncrease;
	glutPostRedisplay();
}

void ClearIdle() {
	if(posSun > -6) posSun -= 0.001;
	shake += 10;
	glutPostRedisplay();
}

void MyReshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-20.0, 20.0, -20.0, 20.0, -100.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(MX / 300.0, MY / 300.0, 1,
		0, 0, 0,
		0.0, 1.0, 0.0);  //시점변환
}


void InitLight() {
	GLfloat mat_diffuse[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_ambient[] = { 0.5, 0.4, 0.3, 1.0 };
	GLfloat mat_shininess[] = { 15.0 };
	GLfloat light_specular[] = { 1.0, 0.8, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat light_position[] = { -3, 6, 3.0, 0.0 };
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glEnable(GL_NORMALIZE); // 조명 계산할 때, 법선 벡터를 정확히 다시 계산
	glEnable(GL_COLOR_MATERIAL);
}

bool HitCheck() { // 미사일과 태양이 충돌하면 true리턴
	vec3 big_snow(sin(time * 0.0001) * 10, 3, -12);
	int size = Missiles.size();
	for (int i = 0; i < size; i++) {
		vec3 diff = big_snow - Missiles[i].pos;
		if (diff.Length() < 1.5) {
			Missiles.erase(Missiles.begin() + i);
			hitColor();
			return true;
		}
	}
	return false;
}


void UpdateMissile() {
	vec3 acc(0, 0, 0); 
	float snowTime = 0.005;

	int size = Missiles.size();
	for (int i = 0; i < size; i++) {
		Missiles[i].v = Missiles[i].v + acc * snowTime;
		Missiles[i].pos = Missiles[i].pos + Missiles[i].v * snowTime;
	}

	for (int i = 0; i < Missiles.size(); ) {
		if (Missiles[i].pos.m[2] > 13 || Missiles[i].pos.m[2] < -13)
			Missiles.erase(Missiles.begin() + i);
		else
			i++;
	}


}
void DrawMissile() {
	UpdateMissile(); // 미사일 좌표의 update
	int size = Missiles.size();
	for (int i = 0; i < size; i++) {
		glPushMatrix();
		glTranslatef(Missiles[i].pos.m[0], Missiles[i].pos.m[1], Missiles[i].pos.m[2]);
		glRotatef(100, 0, 1, 0);
		glutSolidSphere(0.2, 20, 20);
		glPopMatrix();
	}
}


void DrawTank() {
	glPushMatrix();
	glRotatef(sin(time * 0.001) * 1, 1, 0, 0);
	glRotatef(sin(time * 0.001) * 1, 0, 0, 1);
	glTranslatef(0, 3, 0);
	glutSolidCube(10); // 몸통

	glTranslatef(0, 0, 0);
	
	glPushMatrix(); // 바퀴 시작


	glTranslatef(0, 0, 6);
	glScalef(0.4, 1, 0.3);
	glutSolidSphere(5, 20, 20); //왼 바퀴1
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-4, 0, 6);
	glScalef(0.4, 1, 0.3);
	glutSolidSphere(5, 20, 20); //왼 바퀴2
	glPopMatrix();

	glPushMatrix();
	glTranslatef(4, 0, 6);
	glScalef(0.4, 1, 0.3);
	glutSolidSphere(5, 20, 20); //왼 바퀴3
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -6);
	glScalef(0.4, 1, 0.3);
	glutSolidSphere(5, 20, 20); //오른 바퀴1
	glPopMatrix();


	glPushMatrix();
	glTranslatef(-4, 0, -6);
	glScalef(0.4, 1, 0.3);
	glutSolidSphere(5, 20, 20); //오른 바퀴2
	glPopMatrix();

	glPushMatrix();
	glTranslatef(4, 0, -6);
	glScalef(0.4, 1, 0.3);
	glutSolidSphere(5, 20, 20); //오른 바퀴3
	glPopMatrix();

	glPushMatrix();
	glRotatef(18*r, 0, 1, 0);
	glTranslatef(0, 8, 0);
	glutSolidCube(8);  //윗몸체

	glPushMatrix();
	glTranslatef(0, 4, 0);
	glutSolidCube(4); //머리 장식
	glPopMatrix();

	glPushMatrix();
	glTranslatef(5, 2, 0);
	glScalef(3, 1, 0.5);
	glutSolidCube(3);
	glPopMatrix();

	glPopMatrix();
	glPopMatrix();


}



void drawSun() {
	glRotatef(sin(shake * -0.01) * 3, 0, 0, 1);
	glRotatef(sin(shake * -0.01) * 3, 0, 1, 0);
	glTranslatef(0, 10, 0); // 태양 시작 위치
	glutSolidSphere(5, 20, 20); // 몸통

	glPushMatrix(); //태양 장식 시작
	glTranslatef(0, 4, 0);
	glRotatef(270, 1, 0, 0);
	glutSolidCone(2, 5, 4, 5);
	glPopMatrix();

	for (int i = 0; i < 360; i += 30) { //태양 장식 반복
		glPushMatrix(); 
		glRotatef(sin(time * -0.002) * 1, 0, 0, 1);
		glRotatef(i, 0, 0, 1);
		glTranslatef(0, 4, 0);
		glRotatef(270, 1, 0, 0);
		glutSolidCone(2, 5, 4, 5);
		glPopMatrix();
	}


}


void drawLevelUp() {
	glPushMatrix(); //L
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();  

	glTranslatef(0, 0, -14);
	glPushMatrix(); //L2
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();

	glTranslatef(0, 0, 10.8); //E
	glPushMatrix();
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 0, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix(); //E 세로
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();

		glTranslatef(0, 0, -7); //E2
	glPushMatrix();
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 0, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix(); //E 세로
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();

	glTranslatef(0, 0, 1.8);
	glPushMatrix(); //V
		glRotatef(-10, 1, 0, 0);
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
		glPushMatrix(); //V
		glTranslatef(0, 0, 1.3);
		glRotatef(10, 1, 0, 0);
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();

	glTranslatef(0, 0, -9.8);
	glPushMatrix(); //U
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 0, -2);
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
	
	glTranslatef(0, 0, -3.5); //P
	glPushMatrix();
		glTranslatef(0, 0, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix(); 
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glScalef(0.5, 1, 0.5);
		glTranslatef(0, 0.5, -4);
		glutSolidCube(2);
	glPopMatrix();
}

void drawLifebar() {
	glPushMatrix();
	glScalef(0.8, 0.8, 0.8);
	for (int i = 0; i < Life; i++) {
		glutSolidCube(2);
		glTranslatef(2.0, 0.0, 0.0);
	}
	for (int i = 10 - Life; i > 0; i--) {
		glutWireCube(2);
		glTranslatef(2.0, 0.0, 0.0);
	}

	glPopMatrix();

}

void drawClear() {
	glTranslatef(-20, posClear, 17); //C
	glPushMatrix();
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
		glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix(); //C세로
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
	
	glTranslatef(0, 0, -3.5);
	glPushMatrix(); //L
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
		glPopMatrix();
	glPushMatrix(); //E 세로
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();

	glTranslatef(0, 0, -3.5); //E
	glPushMatrix();
		glTranslatef(0, -2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix(); //E 세로
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();

	glTranslatef(0, 0, -5);//A
	glPushMatrix(); 
	glRotatef(10, 1, 0, 0);
	glScalef(0.5, 2.5, 0.5);
	glutSolidCube(2);
	glPopMatrix();
	glPushMatrix(); 
	glTranslatef(0, 0, 1.3);
	glRotatef(-10, 1, 0, 0);
	glScalef(0.5, 2.5, 0.5);
	glutSolidCube(2);
	glPopMatrix();

	glTranslatef(0, 0, -2); //R
	glPushMatrix();
		glTranslatef(0, 0, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 2, -1);
		glScalef(0.5, 0.5, 1.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glScalef(0.5, 2.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glScalef(0.5, 1, 0.5);
		glTranslatef(0, 0.5, -4);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, -1, -1.7);
		glRotatef(20, 0.5, 0,0 );
		glScalef(0.5, 1.5, 0.5);
		glutSolidCube(2);
	glPopMatrix();

	glTranslatef(0, 0, -5);
	glPushMatrix(); 
		glTranslatef(0, 0.8, 0);
		glScalef(0.5, 2, 0.7);
		glutSolidCube(2);
	glPopMatrix();
	glPushMatrix(); 
		glTranslatef(0, -2, 0);
		glScalef(0.5, 0.6, 0.7);
		glutSolidCube(2);
	glPopMatrix();
}


void MyDisplay() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0.5, 0.1, 0.2); // 보라
	glPushMatrix();
	glScalef(30, 0.5, 30); // x-z방향 넓은 판
	glutSolidCube(1.0);
	glPopMatrix();

	//  탱크
	glPushMatrix();
	glTranslatef(0, 1, 12); // 기본 위치
	glTranslatef(x, 0, 0); // 키보드 조작
	glScalef(0.4, 0.15, 0.3);
	glColor3f(0.2, 0.5, 0.3);
	DrawTank();
	glPopMatrix();

	


	//LEVER UP
	glColor3f(1, 0.9, 0.25);
	glPushMatrix();
	glRotatef(sin(time * -0.002) * 0.5, 0, 0, 1);
	glTranslatef(-16, posLevelup, 10);
	glScalef(0.75, 0.75, 0.75);
	drawLevelUp();
	glPopMatrix();
	
	//CLEAR
	
	glPushMatrix();
	//glRotatef(sin(time * -0.002) * 0.5, 0, 0, 1);
	glRotatef(270, 0, 1, 0);
	glTranslatef(3, 5, 0);
	glScalef(0.75, 0.75, 0.75);
	drawClear();
	glPopMatrix();


	// 라이프바 
	glPushMatrix();
	glTranslatef(-14, 1, -20);
	glColor3f(barColor, barColor2, 0.3);
	drawLifebar(); 
	glPopMatrix();
	
	//  태양
	glPushMatrix();

	glTranslatef(0, 0, -12); // 기본 위치
	glTranslatef(sin(time * 0.0001) * 10, posSun, 0); // 좌우 이동
	
	glScalef(0.3, 0.3, 0.3);
	glColor3f(color1, color2, color3);
	
	drawSun();
	glPopMatrix();

	// 미사일

	DrawMissile();
	glFlush();




	bool hit = HitCheck();
	if (hit) Life--;
	if (Life == 0 && stage ==0) {
		Life = 10;
		barColor = 1;
		barColor2 = 0.1;
		stage++;
		timeIncrease = 6;
		posLevelup = 2.5;
		glutTimerFunc(3000, MyMoveTimer, 1);
		glutPostRedisplay();
	}
	if (Life == 0 && stage == 1) {
		color1 = color2 = color3 = 1;
		glutTimerFunc(100, MyTimer, 1);
		glutTimerFunc(7000, ClearTimer, 1);
		glutIdleFunc(ClearIdle);
		glutPostRedisplay();
	}

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);              
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Drawing Example");
	glutDisplayFunc(MyDisplay);
	glutReshapeFunc(MyReshape);
	glutIdleFunc(MyIdle);
	glutSpecialFunc(MySpecial);
	glutMotionFunc(MyMouseMove);
	glutKeyboardFunc(MyKeyboard);
	glClearColor(0.3, 0.3, 0.5, 1);
	InitLight();
	glutMainLoop();
	return 0;
}