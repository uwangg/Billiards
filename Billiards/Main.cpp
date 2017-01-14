#include "gl/glaux.h"
#include "gl/glut.h"
#include <cmath>
#include <stdio.h>
#include <tchar.h>

#define BALL_RECT	20.0
#define BALL_NUM	7
#define HOLE_RECT	22.0
#define HOLE_NUM	6
#define WIDTH		1000
#define HEIGHT		600
#define PIE	3.141592653589793238462643383279
#define G	9.8065

GLuint tex[1]; //텍스쳐 맵핑 소스의 식별자

			   //공 클래스
class hole {
public:
	float holeColor[3];	// 홀의 색깔을 위한 배열
	double holeCoord[3];	// 홀의 위치를 위한 배열
public:
	// 홀 생성자
	hole(float *color, double *coord) {
		for (int i = 0; i<3; i++) {
			holeColor[i] = color[i];
			holeCoord[i] = coord[i];
		}
	}
};
hole *holeList[HOLE_NUM];
double hole1[3] = { 110.0f,110.0f,0.0f };
double hole2[3] = { 490.0f,110.0f,0.0f };
double hole3[3] = { 890.0f,110.0f,0.0f };
double hole4[3] = { 110.0f,490.0f,0.0f };
double hole5[3] = { 490.0f,490.0f,0.0f };
double hole6[3] = { 890.0f,490.0f,0.0f };

//구멍
float chole1[3] = { 0.0f, 0.5f, 0.0f };
float chole2[3] = { 0.0f, 0.5f, 0.0f };
float chole3[3] = { 0.0f, 0.5f, 0.0f };
float chole4[3] = { 0.0f, 0.5f, 0.0f };
float chole5[3] = { 0.0f, 0.5f, 0.0f };
float chole6[3] = { 0.0f, 0.5f, 0.0f };

class ball {
public:
	float ballColor[3];		//공의 색깔을 위한 배열
	double ballCoord[3];	//공의 위치를 위한 배열
	double ballRect;		//공이 움직일 각도
	double ballSpd;			//공의 속도 대략 0~5정도 됨
	double ballFric;		//공의 마찰계수

public:
	//공 생성자.
	ball(float *color, double *coord, double rect) {
		for (int i = 0; i<3; i++) {
			ballColor[i] = color[i];
			ballCoord[i] = coord[i];
		}
		ballRect = rect;
		ballSpd = 0;
		ballFric = 0.0001;
	}
	// 구멍과 만나는지 아닌지 체크
	void holechk()
	{
		for (int i = 0; i<HOLE_NUM; i++) {
			double x = holeList[i]->holeCoord[0] - ballCoord[0];
			double y = holeList[i]->holeCoord[1] - ballCoord[1];
			double z = holeList[i]->holeCoord[2] - ballCoord[2];
			if ((x*x + y*y + z*z) < BALL_RECT*BALL_RECT - 2) {
				if (ballColor[0] == 1.0f && ballColor[1] == 1.0f && ballColor[2] == 1.0f)	// 흰공이라면
				{
					// 구멍에 빠질경우 다시 시작위치로 빠져나옴
					ballCoord[0] = holeList[i]->holeCoord[0];
					ballCoord[1] = holeList[i]->holeCoord[1];
					ballCoord[2] = holeList[i]->holeCoord[2];
					ballCoord[0] = 650.0f;
					ballCoord[1] = 300.0f;
					ballCoord[2] = 0.0f;
					ballSpd = 0;
				}
				else {	// 다른공들이 구멍에 들어갈경우
					for (int j = 0; j<3; j++) {
						ballColor[j] = 0.0f;	// 공색을 검은색으로
						ballSpd *= 0.1;	// 자연스럽게 들어가보이게
						ballCoord[j] = 1050.0f;	// 창 바깥 좌표로 뺌
					}
				}
			}
		}
	}
	//공이 어떠한 공과 서로 충돌했는지 체크
	bool check(const ball& b) const {

		//미리 다음 위치로 이동시켜본 다음에 충돌하는지 확인
		double move1 = (ballCoord[0] + cos((ballRect*PIE) / 180.0)*ballSpd)
			- (b.ballCoord[0] + cos((b.ballRect*PIE) / 180.0)*b.ballSpd);
		double move2 = (ballCoord[1] + sin((ballRect*PIE) / 180.0)*ballSpd)
			- (b.ballCoord[1] + sin((b.ballRect*PIE) / 180.0)*b.ballSpd);

		if (pow(move1, 2) + pow(move2, 2) <= pow(BALL_RECT * 2, 2) + 1)
			return true;
		else
			return false;
	}

	//현재 공의 각도에 따른 다음위치 계산
	void move() {
		if (ballSpd>ballFric*G) {
			ballSpd -= ballFric*G;
			ballCoord[0] += cos((ballRect*PIE) / 180.0)*ballSpd;
			ballCoord[1] += sin((ballRect*PIE) / 180.0)*ballSpd;
		}
		else
			ballSpd = 0;
	}

	//벽에 충돌했는지 체크
	void collisionWall() {
		//미리 다음에 움직일 위치를 계산해본다.
		double move1 = ballCoord[0] + cos((ballRect*PIE) / 180.0)*ballSpd;
		double move2 = ballCoord[1] + sin((ballRect*PIE) / 180.0)*ballSpd;

		if (move1 - 75 <= BALL_RECT + 1 || move1 >= WIDTH - 75 - BALL_RECT - 1) {		//왼쪽 or 오른쪽 충돌
			ballRect = 180 - ballRect;
			if (ballRect<0)
				ballRect += 360;
			ballSpd *= 0.55;			//쿠션의 원리를 생각해서 속도를 20% 줄여봄
		}
		else if (move2 - 75 <= BALL_RECT + 1 || move2 >= HEIGHT - 75 - BALL_RECT - 1) {		//위 or 아래 충돌
			ballRect = 360 - ballRect;
			if (ballRect<0)
				ballRect += 360;
			ballSpd *= 0.55;
		}
	}

	//충돌체크한 후 계산해주는 부분
	void collision(ball& b) {

		//벡터계산을 위해 공과 공사이의 각도를 찾아줌
		double rect = atan2(this->ballCoord[1] - b.ballCoord[1]
			, this->ballCoord[0] - b.ballCoord[0]);
		rect *= 180;
		rect /= PIE;

		//충돌로 인한 두개의 공의 속도를 각각 45%씩 줄여줌
		ballSpd *= 0.55;
		b.ballSpd *= 0.55;

		double bx1, by1, bx2, by2;		//벡터용

		bx1 = cos(((ballRect - rect)*PIE) / 180.0)*ballSpd;
		by1 = sin(((ballRect - rect)*PIE) / 180.0)*ballSpd;
		bx2 = cos(((b.ballRect - rect)*PIE) / 180.0)*b.ballSpd;
		by2 = sin(((b.ballRect - rect)*PIE) / 180.0)*b.ballSpd;

		printf("%lf\n", rect);
		printf("SPD : ball1 %lf, ball2 %lf\n", ballSpd, b.ballSpd);
		printf("Rect : ball1 %lf, ball2 %lf\n", ballRect, b.ballRect);
		printf("ball1 x_vector : %lf, y_vector : %lf\n", bx1, by1);
		printf("ball2 x_vector : %lf, y_vector : %lf\n", bx2, by2);

		//탄성충돌이므로
		//각 구의 x벡터를 서로 교환해준다음 다시 속도를 구해줌
		ballSpd = sqrt(pow(bx2, 2) + pow(by1, 2));
		b.ballSpd = sqrt(pow(bx1, 2) + pow(by2, 2));

		//각도도 다시 구해줌
		printf("ball1 SPD : %lf, ball2 SPD : %lf\n", ballSpd, b.ballSpd);
		ballRect = rect + (atan2(by1, bx2) * 180 / PIE);
		b.ballRect = rect + (atan2(by2, bx1) * 180 / PIE);

		printf("newRect ball1 : %lf, ball2 : %lf\n", ballRect, b.ballRect);
		printf("\n");

	}
};

GLUquadricObj *obj[6];
ball *ballList[BALL_NUM];
bool mouseClick = false;
int lineX1, lineY1, lineX2, lineY2;

void init() {

	//공의 위치들
	double ball1[3] = { 350.0f,300.0f,0.0f };
	double ball2[3] = { 300.0f,330.0f,0.0f };
	double ball3[3] = { 300.0f,270.0f,0.0f };
	double ball4[3] = { 250.0f,350.0f,0.0f };
	double ball5[3] = { 250.0f,300.0f,0.0f };
	double ball6[3] = { 250.0f,250.0f,0.0f };
	//흰공
	double ball7[3] = { 650.0f,300.0f,0.0f };
	//구멍

	//공의 색깔들...
	float cball1[3] = { 1.0f,0.0f,0.0f };
	float cball2[3] = { 0.0f,1.0f,0.0f };
	float cball3[3] = { 0.0f,0.0f,1.0f };
	float cball4[3] = { 1.0f,0.5f,0.0f };
	float cball5[3] = { 1.0f,1.0f,0.0f };
	float cball6[3] = { 1.0f,0.0f,1.0f };
	//흰공
	float cball7[3] = { 1.0f,1.0f,1.0f };

	//공을 만들어줌
	ballList[0] = new ball(cball1, ball1, 170.0);
	ballList[1] = new ball(cball2, ball2, 10.0);
	ballList[2] = new ball(cball3, ball3, 270.0);
	ballList[3] = new ball(cball4, ball4, 90.0);
	ballList[4] = new ball(cball5, ball5, 180.0);
	ballList[5] = new ball(cball6, ball6, 44.0);
	//흰공 만들기
	ballList[6] = new ball(cball7, ball7, 0.0f);

	//홀 만들기
	holeList[0] = new hole(chole1, hole1);
	holeList[1] = new hole(chole2, hole2);
	holeList[2] = new hole(chole3, hole3);
	holeList[3] = new hole(chole4, hole4);
	holeList[4] = new hole(chole5, hole5);
	holeList[5] = new hole(chole6, hole6);

}

void check() {

	//공이 벽에 충돌했는지 체크한다음 벽에 충돌했으면 각도 바꿔준다
	for (int i = 0; i<BALL_NUM; i++)
		ballList[i]->collisionWall();
	//공이 구멍 근처에 가면 구멍안으로 들어가게 해줌
	for (int i = 0; i<BALL_NUM; i++)
		ballList[i]->holechk();

	//공과 공의 충돌을 체크한다음
	//공이 충돌했으면 충돌작업으로 가서 튕겨나갈 계산을 해준다.
	for (int i = 0; i<BALL_NUM - 1; i++) {
		for (int j = i + 1; j<BALL_NUM; j++) {
			if (ballList[i]->check(*ballList[j]))
				ballList[i]->collision(*ballList[j]);

		}
	}
}

//이미지 파일을 열어 GLuint변수에 저장 
AUX_RGBImageRec* LoadBMPFile(TCHAR* filename)
{
	FILE *hFile = NULL;
    errno_t err = NULL;

	if (!filename) return NULL;

	err = _tfopen_s(&hFile, filename, _T("r")); //이미지 파일 열기

	if (err == 0) {
		fclose(hFile);
		return auxDIBImageLoad(filename); //열은 파일을 AUX_RGBImageRec형태로 변형
	}

	return NULL;
}
//이미지 파일읽고 테스쳐 맵핑 소스로 변환하고 설정
void LoadImage()
{
	AUX_RGBImageRec* texRec = 0; //이미지 파일을 저장하기 위한 변수(텍스쳐 맵핑 소스)     

	glGenTextures(1, &tex[0]); //텍스쳐 맵핑 소스의 식별자를 설정
	if (texRec = LoadBMPFile("BilliardsBackground.bmp")) { //이미지 파일을 읽어 텍스쳐 맵핑 소스에 저장
		glBindTexture(GL_TEXTURE_2D, tex[0]); //사용할 텍스쳐 맵핑 소스의 식별자를 지정
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 텍스쳐 맵핑 소스의 크기 > 맵핑되어질 물체의 크기일때 축소처리
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 텍스쳐 맵핑 소스의 크기 < 맵핑되어질 물체의 크기일때 확대처리  
		glTexImage2D(GL_TEXTURE_2D, 0, 3, texRec->sizeX, texRec->sizeY, 0,
			GL_RGB, GL_UNSIGNED_BYTE, texRec->data); //맵핑 소스에 대한 특성을 설정 
	}
	else return;

	if (texRec) {
		if (texRec->data) free(texRec->data); //필요없는 메모리 해제
		free(texRec); //필요없는 메모리 해제  
	}
	glEnable(GL_TEXTURE_2D); //차원 텍스쳐 맵핑을 사용하겠노라고 OpenGL에게 알림   
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); //화면의 색과 텍스쳐 맵핑 소스의 색을 어떻게 조합하여 표현하는지 설정 
}

void display() {
	glClearColor(0.50, 0.3, 0.0, 0.12);	// 바닥색
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//흰공을 움직이도록 마우스 클릭하고 움직였을때 그 선을 그려줌
	if (mouseClick && lineX2 != 0) {
		glDisable(GL_LIGHTING);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex2f(lineX1, lineY1);
		glVertex2f(lineX2, lineY2);
		glEnd();
	}

	glDisable(GL_LIGHTING);
	glColor3f(0.1f, 0.2f, 0.1f);
	glBegin(GL_LINE_STRIP);
	glVertex3f(50.0f, 50.0f, 0.0f);
	glVertex3f(950.0f, 50.0f, 0.0f);
	glVertex3f(950.0f, 550.0f, 0.0f);
	glVertex3f(50.0f, 550.0f, 0.0f);
	glVertex3f(50.0f, 50.0f, 0.0f);
	glVertex3f(75.0f, 75.0f, 0.0f);
	glVertex3f(925.0f, 75.0f, 0.0f);
	glVertex3f(950.0f, 50.0f, 0.0f);
	glVertex3f(925.0f, 75.0f, 0.0f);
	glVertex3f(925.0f, 525.0f, 0.0f);
	glVertex3f(950.0f, 550.0f, 0.0f);
	glVertex3f(925.0f, 525.0f, 0.0f);
	glVertex3f(75.0f, 525.0f, 0.0f);
	glVertex3f(50.0f, 550.0f, 0.0f);
	glVertex3f(75.0f, 525.0f, 0.0f);
	glVertex3f(75.0f, 75.0f, 0.0f);
	glEnd();

	//원을 그리는 방정식 배열을 등록한다음 Pop을 해줌
	for (int i = 0; i<BALL_NUM; i++) {
		GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //조명색
		GLfloat mat_shininess[] = { 127.0 }; // 반사되어 비치는 정도 정반사조절 예를들면 유리, 코팅된플라스틱등
		GLfloat light_position[] = { -0.5f, 0.5f, 0.5f, 0.0f }; // 조명값지정 0.0은 동차좌표계(2이면 1.0 1.0 1.0이 각각 0.5가된다)
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //물질의재질
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);//물질의재질
		glMaterialfv(GL_FRONT, GL_DIFFUSE, ballList[i]->ballColor);//물질의재질
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);//조명의 특성 GL_LIGHT0는 0부터 7까지 사용가능
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glEnable(GL_LIGHTING); //조명을 켜는것
		glPushMatrix();
		glTranslatef(ballList[i]->ballCoord[0],
			ballList[i]->ballCoord[1], ballList[i]->ballCoord[2]);
		glutSolidSphere(BALL_RECT, 24, 24);
		glPopMatrix();
	}
	for (int i = 0; i<HOLE_NUM; i++) {
		GLfloat mat_specular[] = { 0.0f, 0.5f, 0.0f, 1.0f }; //조명색
		GLfloat mat_shininess[] = { 2.0 }; // 반사되어 비치는 정도 정반사조절 예를들면 유리, 코팅된플라스틱등
		GLfloat light_position[] = { -0.5f, 0.6f, -0.8f, 0.0f }; // 조명값지정 0.0은 동차좌표계(2이면 1.0 1.0 1.0이 각각 0.5가된다)
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); //물질의재질
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);//물질의재질
		glMaterialfv(GL_FRONT, GL_DIFFUSE, holeList[i]->holeColor);//물질의재질
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);//조명의 특성 GL_LIGHT0는 0부터 7까지 사용가능
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glEnable(GL_LIGHTING); //조명을 켜는것
		glPushMatrix();
		glTranslatef(holeList[i]->holeCoord[0], holeList[i]->holeCoord[1], holeList[i]->holeCoord[2]);
		glutSolidSphere(HOLE_RECT, 24, 24);
		glPopMatrix();
	}
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex[0]); //어떤 텍스쳐 맵핑 소스를 사용할지 설정
	glColor3f(0.0f, 0.5f, 0.0f); //그릴 색을 정함
								 /*맨위의 네모를 그림*/
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS); // 네모를 나타내며 내부색 칠함
	glTexCoord2f(0.0f, 0.0f); /*정점에 텍스쳐 맵핑 소스의 정점을 맞춤*/ glVertex3f(75.0f, 75.0f, 0.0f); //정점설정
	glTexCoord2f(0.0f, 1.0f); glVertex3f(75.0f, 525.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(925.0f, 525.0f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(925.0f, 75.0f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D); //텍스쳐 맵핑 소스를 사용 안하겠다고 OpenGL에게 알림
	glPopMatrix();

	glColor3f(0.45f, 0.2f, 0.1f); //그릴 색을 정함
	glRectf(50.0f, 50.0f, 950.0f, 550.0f); //사각형
										   //출력
	glutSwapBuffers();
}

void MyTimer(int value) {

	//1ms마다 공들이 각도랑 속도에 따라 적당히 움직임
	for (int i = 0; i<BALL_NUM; i++)
		ballList[i]->move();

	check();
	glutPostRedisplay();			//화면을 갱신해줌
	glutTimerFunc(1, MyTimer, 1);		//자기 자신을 다시 부름으로써 1ms마다 계속 갱신됨
}

//마우스 입력을 받았을시 콜백되는 함수
//button은 왼쪽클릭,오른쪽클릭인지
//state는 누름상태인지 띈상태인지
//x,y는 클릭된 좌표
void mouse(int button, int state, int x, int y) {

	//마우스의 좌표는 왼쪽 위가 0,0이므로
	//왼쪽 아래가 0,0인 그리기좌표로 변환해준다.
	y = HEIGHT - y;

	//마우스 왼쪽버튼 눌렀을때
	//만약 우리가 움직일 수 있는 흰공(제일 마지막공)의 원 안에 x,y좌표가 들어있다면
	//누름체크를 true로 해줘서 mouseomove에서 알수 있도록 한다.
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && ballList[BALL_NUM - 1]->ballSpd == 0) {
		if (pow(x - ballList[BALL_NUM - 1]->ballCoord[0], 2) + pow(y - ballList[BALL_NUM - 1]->ballCoord[1], 2) < pow(BALL_RECT, 2)) {
			mouseClick = true;
			//그리기를 이쁘게 하기 위해서 원의 중심으로부터 선이 그려지도록 한다.
			lineX1 = ballList[BALL_NUM - 1]->ballCoord[0];
			lineY1 = ballList[BALL_NUM - 1]->ballCoord[1];
		}
	}
	//마우스 왼쪽버튼 띌때
	//흰공의 속도가 0이면 공을 칠 수 있는 상태이므로..
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && ballList[BALL_NUM - 1]->ballSpd == 0) {
		mouseClick = false;
		printf("%d %d\n", lineX1 - lineX2, lineY1 - lineY2);
		printf("%lf\n", atan2(double(lineY1 - lineY2), double(lineX1 - lineX2)) * 180 / PIE);
		ballList[BALL_NUM - 1]->ballRect = atan2(double(lineY1 - lineY2), double(lineX1 - lineX2)) * 180 / PIE;
		//atan2를 이용하여 공이 날아갈 각도를 계산해준다.

		ballList[BALL_NUM - 1]->ballSpd = sqrt(pow(lineX1 - lineX2, 2.0) + pow(lineY1 - lineY2, 2.0)) / 20;
		//흰공의 속도는 나누기 20을 해줌으로써 적당히 할당해준다.

		lineX1 = lineY1 = lineX2 = lineY2 = 0;
		//선그리기 끝내기위해 모두 0으로
	}
}

//마우스 이동 함수(왼쪽버튼이 눌린 상태에서만 작동함)
void mousemove(int x, int y) {

	//마우스 왼쪽이 눌려져있다면
	if (mouseClick) {
		y = HEIGHT - y;
		lineX2 = x;
		lineY2 = y;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("당구");		//윈도우 이름
	glMatrixMode(GL_PROJECTION);	// 투영 행렬 변환
	glLoadIdentity();	// 단위행렬 초기화
	glViewport(0, 0, WIDTH, HEIGHT);		//뷰포트 사이즈
	glOrtho(0.0, WIDTH, 0.0, HEIGHT, -100.0, 100.0);		//뷰포트에 대한 좌표적용 1000,600으로 해줌
	init();
	LoadImage(); //이미지 불러옴
	glutMouseFunc(mouse);			//마우스 입력함수 등록
	glutMotionFunc(mousemove);		//마우스 이동함수 등록
	glutTimerFunc(1, MyTimer, 1);		//타이머설치
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}