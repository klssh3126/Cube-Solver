//조명 테스트 하기
// 화면에 모자이크 처리 없애기




#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <stdio.h>
#include <wiringPi.h>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <cstdlib>
#include <stdint.h>

using namespace std;
using namespace cv;

#define CELL_SIZE 85 // default 110
#define BOX_X 180
#define BOX_Y 115
#define BOX_SZ (CELL_SIZE*3)
#define COLOR_BIN 9
#define __ 0 //EMPTY
#define RE 1 //RED
#define BL 2 //BLUE
#define YE 3 //YELLOW
#define OR 4 //ORANGE
#define WH 5 //WHITE
#define GR 6 //GREEN

#define DELAY 800
#define IN_OUT_STEP	330 //350 okay
#define STEP_PULSE_WIDTH 600

 //don't use 24, 27 pin because the pin is broken
#define UPDIR 28
#define UPSTEP 25
#define DOWNDIR 10
#define DOWNSTEP 14
#define LED1 23
#define LED2 21

#define UP_EN 30
#define DOWN_EN 31 
#define ABLE 0
#define DISABLE 1
#define BUTTON_A 26
#define BUTTON_B 31


// R ~ Bi : 1~ 12 를 차지하고 있다.
#define UTF 13 
#define FTU 14
#define R2 15
#define L2 16
#define U2 17
#define D2 18
#define F2 19
#define B2 20

int initial_color[200] = { //원하는 색깔로 초기화, @@ 주의!!!! 십자가 방향에 EMPTY가 들어가면 오류난다!
	0,
	/*__,__,__,GR,WH,WH,__,__,__,__,__,__,
	__,__,__,BL,RE,RE,__,__,__,__,__,__,
	__,__,__,YE,RE,BL,__,__,__,__,__,__,
	RE,OR,BL,RE,YE,YE,OR,GR,BL,OR,GR,YE,
	WH,BL,WH,OR,WH,BL,YE,GR,OR,YE,YE,RE,
	WH,GR,RE,GR,YE,GR,YE,BL,WH,GR,BL,RE,
	__,__,__,WH,GR,OR,__,__,__,__,__,__,
	__,__,__,OR,OR,RE,__,__,__,__,__,__,
	__,__,__,BL,WH,OR,__,__,__,__,__,__,*/

};
int CUBE_ARRAY[200] = { 0 };
int ROTATE_ORDER[200] = { 0 };
int UTF_FTU_ORDER[400] = { 0 };
int TEMP_ORDER[200] = { 0 };
int PARTIAL_ORDER[8][200] = { 0 }; //인덱스는 1부터 시작하니 각 LV함수에서 조심 해야한다.


int Level_Checker[8] = { 0 };
int cnt_white, cnt_yellow, cnt_orange, cnt_pink, cnt_green, cnt_blue;

int Lv1_TARGET_array[50] = { 4,15,17,28,37,48,50,61,76,87,89,100,43,54,56,67,46,57,59,70 };//정면십자가 타겟블록범위
int Lv2_TARGET_array[4] = { 46,57,59,70 };// 정면십자가의 팔 부분 /뒷면에서 찾는다.
class Rotate {
public:
	int R(int cube_array[], int order[]);  //return 1
	int Ri(int cube_array[], int order[]); //return 2
	int L(int cube_array[], int order[]);  //return 3
	int Li(int cube_array[], int order[]); //return 4
	int U(int cube_array[], int order[]);  //return 5
	int Ui(int cube_array[], int order[]); //return 6
	int D(int cube_array[], int order[]);  //return 7
	int Di(int cube_array[], int order[]); //return 8
	int F(int cube_array[], int order[]);  //return 9
	int Fi(int cube_array[], int order[]); //return 10
	int B(int cube_array[], int order[]);  //return 11
	int Bi(int cube_array[], int order[]); //return 12
	
};


void color_initializer(int cube_array[], int color[]);
void show_order(int order[]);
void LV_show_order(int order[], int order_index);
int check_order_empty(int order[]);

void LV1(int cube_array[], int color[], int rotate_order[], int lv1_target_array[]);
//LV1은 정면 십자가 완성하는 단계
bool check_front_cross(int cube_array[]);
int Lv1_search_target_block(int target_array[], int cube_array[]);
int Give_target_block_partner_location(int target_location);
int search_side_center(int cube_array[], int partner_location);
void HowToRotate(int target_location, int order[]);
int Check_collision(int target_location, int order[]);
void simplify_order(int rotate_order[]);
bool check_able_simplify(int rotate_order[]);
void shift_order_move2(int rotate_order[], int num);

void LV2(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]);
void recursive(int cube_array[]);
//LV2는 십자가의 옆면과 옆면중앙 색깔과 일치시키는 단계
bool check_SideOfCross_equal_SideOfCenter_COLOR(int cube_array[]);
//optimizing_order(ROTATE_ORDER);//출력하기전에 순서를 간소화 시킨다. F-F-F를 Fi로 고치는 방식
bool check_two_block_same(int partner_color);
bool Check_exist_target(int cube_array[]);
int LV2_search_target_block(int target_array[], int cube_array[]);
void LV3(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]);
bool check_LV3_complete(int cube_array[]);
void side3_assemble(int cube_array[]);
void Is_equal_60_49(int cube_array[]);
void Is_equal_101_88(int cube_array[]);
void Is_equal_44_55(int cube_array[]);
void Is_equal_3_16(int cube_array[]);
void Is_equal_36_49(int cube_array[]);
void Is_equal_99_88(int cube_array[]);
void Is_equal_68_55(int cube_array[]);
void Is_equal_5_16(int cube_array[]);
bool LV3_side3_exist(int cube_array[]);
void side1_move_to_side3(int cube_array[]);
bool LV3_side1_exist(int cube_array[]);
void front_move_to_side3(int cube_array[]);
bool LV3_front_exist(int cube_array[]);
void bottom_move_to_side3(int cube_array[]);
bool LV3_bottom_exist(int cube_array[]);
void LV4(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]);
bool check_LV4_complete(int cube_array[]);
bool is_LV4_side3_target_exist(int cube_array[]);
void LV5(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]);
bool check_LV5_complete(int cube_array[]);
bool is_LV5_L_shape_exist(int cube_array[]);
bool is_LV5_bar_shape_exist(int cube_array[]);
void LV6(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]);
bool check_LV6_complete(int cube_array[]);
bool is_LV6_No_coner(int cube_array[]);
bool is_LV6_One_Conner_exist(int cube_array[]);
bool is_LV6_Two_Conner_exist(int cube_array[]);

void LV7(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]);
bool check_LV7_complete(int cube_array[]);
bool is_LV7_same_direction(int cube_array[]);
bool is_LV7_diagonal_direction(int cube_array[]);

void LV8(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]);
bool check_LV8_complete(int cube_array[]);
bool is_LV8_complete_side_face_exist(int cube_array[]);
bool is_LV8_88_direction(int cube_array[]);
bool is_LV8_55_direction(int cube_array[]);
bool is_LV8_16_direction(int cube_array[]);
bool is_LV8_49_direction(int cube_array[]);
void holding02();
void holding13();

class color_Value {
public:
	int W[3] = { 168,139,83 };
	int Y[3] = { 0,120,118 };	
	int O[3] = { 0,23,153 };
	int pink[3] = { 88,23,153 };
	int light_blue[3] = { 148,91,0 };
	int light_green[3] = { 15,102,20 };

};


void color_teller(int B[], int G[], int R[], int represent_color[], color_Value color);
void Send_picture_data_to_cube(int initial_color[], int represent_color[], int picture_num);
void Dos_opencv();
void camera_rotation();
void horizontal_rotate();
void vertical_rotate();
int x = 0;
void Delay() {
	delay(600);
}

void servo0_90() {
	system("echo 0=213>/dev/servoblaster");
}
void servo0_R90() { //값이 줄어들면 리버스 90이다.
	system("echo 0=74>/dev/servoblaster");
}
void servo0_vertical() {
	system("echo 0=147>/dev/servoblaster");
}

void servo1_90() {
	system("echo 1=216>/dev/servoblaster");
}
void servo1_R90() {
	system("echo 1=79>/dev/servoblaster");
}
void servo1_vertical() {
	system("echo 1=150>/dev/servoblaster");
}

void servo2_90() {
	system("echo 2=206>/dev/servoblaster");
}
void servo2_R90() {
	system("echo 2=73>/dev/servoblaster");
}
void servo2_vertical() {
	system("echo 2=143>/dev/servoblaster");
}

void servo3_90() {
	system("echo 3=217>/dev/servoblaster");
}
void servo3_R90() {
	system("echo 3=79>/dev/servoblaster");
}
void servo3_vertical() {
	system("echo 3=151>/dev/servoblaster");
}


//*************************************//
void Force_servo0_90() {
	system("echo 0=218>/dev/servoblaster");
}
void Force_servo0_R90() { //값이 줄어들면 리버스 90이다.
	system("echo 0=70>/dev/servoblaster");
}
void Force_servo0_vertical() {
	system("echo 0=147>/dev/servoblaster");
}

void Force_servo1_90() {
	system("echo 1=219>/dev/servoblaster");
}
void Force_servo1_R90() {
	system("echo 1=76>/dev/servoblaster");
}
void Force_servo1_vertical() {
	system("echo 1=150>/dev/servoblaster");
}

void Force_servo2_90() {
	system("echo 2=211>/dev/servoblaster");
}
void Force_servo2_R90() {
	system("echo 2=68>/dev/servoblaster");
}
void Force_servo2_vertical() {
	system("echo 2=143>/dev/servoblaster");
}

void Force_servo3_90() {
	system("echo 3=220>/dev/servoblaster");
}
void Force_servo3_R90() {
	system("echo 3=75>/dev/servoblaster");
}
void Force_servo3_vertical() {
	system("echo 3=151>/dev/servoblaster");
}


void bottom_UP() {
	system("echo 4=150>/dev/servoblaster");
}

void bottom_DOWN() {
	system("echo 4=70>/dev/servoblaster");
}
//////////////////////////

void move02_IN() {
	digitalWrite(UPDIR, HIGH); //a4988 , drv8825
	//digitalWrite(UPDIR, LOW); //tmc2100

	for (x = 0; x < IN_OUT_STEP+1; x++) // go centor
	{
		//tmc 1500
	 //a4988 120	
		//if(x == IN_OUT_STEP/4) 	system("echo 0=78>/dev/servoblaster"); //for TEST

		digitalWrite(UPSTEP, 1); //ON
		delayMicroseconds(STEP_PULSE_WIDTH); //a4988 3000us
		digitalWrite(UPSTEP, 0);//OFF
		delayMicroseconds(STEP_PULSE_WIDTH);
	}
}
void move02_OUT() {
	digitalWrite(UPDIR, LOW); //a4988, //drv8825
	//digitalWrite(UPDIR, HIGH); //tmc2100
	for (x = 0; x < IN_OUT_STEP; x++) // go outside
	{//tmc 1200
	 //a4988 95

		//if (x == IN_OUT_STEP/4) 	system("echo 0=218>/dev/servoblaster"); test

		digitalWrite(UPSTEP, 1);//ON
		delayMicroseconds(STEP_PULSE_WIDTH);
		digitalWrite(UPSTEP, 0);//OFF
		delayMicroseconds(STEP_PULSE_WIDTH);
	}
}
void move02_OUT_Vertical() {

	digitalWrite(UPDIR, LOW); //a4988 , drv8825
	//digitalWrite(DOWNDIR, HIGH);//tmc2100
	for (x = 0; x < IN_OUT_STEP; x++) // go outside
	{//tmc950
		int STEP = IN_OUT_STEP;
		if (x == (int)(STEP / 1.5)) {
			servo0_vertical();
			delay(2);
			servo2_vertical();
		}
		digitalWrite(UPSTEP, 1);//ON
		delayMicroseconds(STEP_PULSE_WIDTH);
		digitalWrite(UPSTEP, 0);//OFF
		delayMicroseconds(STEP_PULSE_WIDTH);
	}
}
void move13_IN() {
	
	digitalWrite(DOWNDIR, HIGH);//a4988 , drv8825
	//digitalWrite(DOWNDIR, LOW);//tmc2100
	for (x = 0; x < IN_OUT_STEP+1; x++) // go centor
	{//tmc1200
		digitalWrite(DOWNSTEP, 1); //ON
		delayMicroseconds(STEP_PULSE_WIDTH);
		digitalWrite(DOWNSTEP, 0);//OFF
		delayMicroseconds(STEP_PULSE_WIDTH);
	}
}
void move13_OUT() {

	digitalWrite(DOWNDIR, LOW); //a4988 , drv8825
	//digitalWrite(DOWNDIR, HIGH);//tmc2100
	for (x = 0; x < IN_OUT_STEP; x++) // go outside
	{//tmc950
		digitalWrite(DOWNSTEP, 1);//ON
		delayMicroseconds(STEP_PULSE_WIDTH);
		digitalWrite(DOWNSTEP, 0);//OFF
		delayMicroseconds(STEP_PULSE_WIDTH);
	}
}
void move13_OUT_Vertical() {

	digitalWrite(DOWNDIR, LOW); //a4988 , drv8825
	//digitalWrite(DOWNDIR, HIGH);//tmc2100
	for (x = 0; x < IN_OUT_STEP; x++) // go outside
	{//tmc950
		int STEP = IN_OUT_STEP;
		if (x == (int)( STEP / 1.5)) {
			servo1_vertical();
			delay(2);
			servo3_vertical();
		}

		digitalWrite(DOWNSTEP, 1);//ON
		delayMicroseconds(STEP_PULSE_WIDTH);
		digitalWrite(DOWNSTEP, 0);//OFF
		delayMicroseconds(STEP_PULSE_WIDTH);
	}
}

void vertical_rotate() {
	move02_OUT();

	servo1_R90();
	delay(3);
	servo3_90();
	delay(600);

	bottom_UP();
	delay(300);

	move02_IN();
	bottom_DOWN();


	move13_OUT_Vertical();
	Delay();

	move13_IN();
	delay(500);
	
}

void flip180(){
	
	bottom_DOWN();//추가됨

	servo1_R90();
	delay(3);
	servo3_90();

	delay(800);
	bottom_UP();
	delay(500);

	/*move02_IN();
	bottom_DOWN();
	move13_OUT_Vertical();
	Delay();
	move13_IN();
	delay(300);*/

}

void vertical_rotate_back() {
	move02_OUT();

	servo1_90();
	delay(3);
	servo3_R90();
	delay(600);

	bottom_UP();
	delay(300);

	move02_IN();
	bottom_DOWN();


	move13_OUT_Vertical();
	Delay();

	move13_IN();
	delay(500);
}

void horizontal_rotate() {

	move13_OUT();//holding 추가 해줄것

	servo0_R90();
	delay(2);
	servo2_90();
	//holding02();
	delay(600);

	bottom_UP();
	delay(300);

	move13_IN();
	bottom_DOWN();

	move02_OUT_Vertical();
	Delay();

	move02_IN();
}

void servoU() {
	Force_servo2_90();
	Delay();
	
	move02_OUT_Vertical();
	Delay();

	move02_IN();
	delay(600);
}

void servoUi() {
	//move02_OUT();
	//Delay();
	//NoCube_servo2_90();
	//move02_IN();
	//servo2_vertical();
	//Delay();


	Force_servo2_R90();
	Delay();

	move02_OUT_Vertical();
	Delay();

	move02_IN();
	delay(600);
}

void servoD() {
	Force_servo0_90();
	Delay();

	move02_OUT_Vertical();
	Delay();

	move02_IN();
	delay(600);
}

void servoDi() {
	Force_servo0_R90();
	Delay();

	move02_OUT_Vertical();
	Delay();

	move02_IN();
	delay(600);
}

void servoR() {
	/*move13_OUT();
	Delay();
	NoCube_servo1_R90(); //엣날 기둥 번호하고 지금 번호하고 1,3번이 바뀌어 있다. 지금 이 줄은 옛날 버전이다.
	move13_IN();

	servo3_vertical();
	Delay();*/

	Force_servo3_90();
	Delay();

	move13_OUT_Vertical();
	Delay();

	move13_IN();
	delay(600);
}

void servoRi() {
	Force_servo3_R90();
	Delay();

	move13_OUT_Vertical();
	Delay();

	move13_IN();
	delay(600);
}

void servoL() {
	Force_servo1_90();
	Delay();

	move13_OUT_Vertical();
	Delay();

	move13_IN();
	delay(600);
}

void servoLi() {
	Force_servo1_R90();
	Delay();

	move13_OUT_Vertical();
	Delay();

	move13_IN();
	delay(600);
}

void servoF() {
	servoU();
	////vertical_rotate();

	////아래는 특별
	//Force_servo2_90();
	//Delay();

	//move02_OUT_Vertical();
	////아래는 공통
	//servo1_90();
	//delay(2);
	//servo3_R90();
	//delay(600);

	//bottom_UP();
	//delay(300);

	//move02_IN();
	//bottom_DOWN();


	//move13_OUT_Vertical();
	//Delay();

	//move13_IN();
	//delay(600);
}

void servoFi() {
	servoUi();
	////vertical_rotate();

	////아래는 특별
	//Force_servo2_R90();
	//Delay();

	//move02_OUT_Vertical();
	////아래는 공통
	//servo1_90();
	//delay(2);
	//servo3_R90();
	//delay(600);

	//bottom_UP();
	//delay(300);

	//move02_IN();
	//bottom_DOWN();


	//move13_OUT_Vertical();
	//Delay();

	//move13_IN();
	//delay(600);
}

void servoB() {

	servoD();
	////vertical_rotate();
	//
	////아래는 특별
	//Force_servo0_90();
	//Delay();

	//move02_OUT_Vertical();
	////아래는 공통
	//servo1_90();
	//delay(2);
	//servo3_R90();
	//delay(600);

	//bottom_UP();
	//delay(300);

	//move02_IN();
	//bottom_DOWN();


	//move13_OUT_Vertical();
	//Delay();

	//move13_IN();
	//delay(600);
}

void servoBi() {
	servoDi();
	////vertical_rotate();

	////아래는 특별
	//Force_servo0_R90();
	//Delay();

	//move02_OUT_Vertical();
	////아래는 공통
	//servo1_90();
	//delay(2);
	//servo3_R90();
	//delay(600);

	//bottom_UP();
	//delay(300);

	//move02_IN();
	//bottom_DOWN();


	//move13_OUT_Vertical();
	//Delay();

	//move13_IN();
	//delay(600);
	
}

void servoR2() {
	move13_OUT();
	servo3_R90();
	delay(400);
	move13_IN();
	Force_servo3_90();
	delay(800);
	move13_OUT_Vertical();
	move13_IN();
	delay(500);
}

void servoL2() {
	move13_OUT();
	servo1_R90();
	delay(400);
	move13_IN();
	Force_servo1_90();
	delay(800);
	move13_OUT_Vertical();
	move13_IN();
	delay(500);
}
void servoU2() {
	move02_OUT();
	servo2_R90();
	delay(400);
	move02_IN();
	Force_servo2_90();
	delay(800);
	move02_OUT_Vertical();
	move02_IN();
	delay(500);
}
void servoD2() {
	move02_OUT();
	servo0_R90();
	delay(400);
	move02_IN();
	Force_servo0_90();
	delay(800);
	move02_OUT_Vertical();
	move02_IN();
	delay(500);

}
void servoF2() { servoU2(); }
void servoB2() { servoD2(); }


void blink() {
	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, HIGH);
	delay(500);
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	delay(500);
	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, HIGH);
	delay(500);
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
	delay(500);
	digitalWrite(LED1, HIGH);
	digitalWrite(LED2, HIGH);
	delay(500);
	digitalWrite(LED1, LOW);
	digitalWrite(LED2, LOW);
}
void Rotate_Motor(int rotateorder[], int length) {
	for (int i = 0; i < length; i++) {
		
		if (rotateorder[i] == 0) {
			return;
		}
		switch (rotateorder[i]) {
		case 1:
			servoR();
			break;
		case 2:
			servoRi();
			break;
		case 3:
			servoL();
			break;
		case 4:
			servoLi();
			break;
		case 5:
			servoU();
			break;
		case 6:
			servoUi();
			break;
		case 7:
			servoD();
			break;
		case 8:
			servoDi();
			break;
		case 9:
			servoF();
			break;
		case 10:
			servoFi();
			break;
		case 11:
			servoB();
			break;
		case 12:
			servoBi();
			break;
		case UTF:
			vertical_rotate();
			break;
		case FTU:
			vertical_rotate_back();
			break;
		case R2:
			servoR2();
			break;
		case L2:
			servoL2();
			break;
		case U2:
			servoU2();
			break;
		case D2:
			servoD2();
			break;
		case F2:
			servoF2();
			break;
		case B2:
			servoB2();
			break;
		}
	}
}

void RotateOrder_Reverse(int length) 
{
	int num = 0;
	for (int i = 0; i < length; i++) {
		if (ROTATE_ORDER[i] == 0) {
			num = i;
			break;
		}
	}

	for (int i = 0; i < num; i++)
	{
		//홀수면 1올려주고 짝수면 1 마이너스 해준다. 
		//저장된 순서 뿐만 아니라 시계 반시계 순서까지 바꾼다.
		if (ROTATE_ORDER[num - 1 - i] % 2 == 0)
		{
			TEMP_ORDER[i] = (ROTATE_ORDER[num - 1 - i] - 1);
		}
		else
		{
			TEMP_ORDER[i] = (ROTATE_ORDER[num - 1 - i] + 1);
		}
	}

	for (int i = 0; i < num; i++) {
		ROTATE_ORDER[i] = TEMP_ORDER[i];
	}
}

void Partial_order_save(void) {
	for (int i = 0, j = 0; i < Level_Checker[0]; i++, j++) { //LV1 단계 순서만 부분저장
		PARTIAL_ORDER[0][j] = UTF_FTU_ORDER[i];
	}

	for (int i = Level_Checker[0], j = 0; i < Level_Checker[1]; i++, j++) {//LV2 단계 순서만 부분저장
		PARTIAL_ORDER[1][j] = UTF_FTU_ORDER[i];
	}

	for (int i = Level_Checker[1], j = 0; i < Level_Checker[2]; i++, j++) {//LV3 단계 순서만 부분저장
		PARTIAL_ORDER[2][j] = UTF_FTU_ORDER[i];
	}

	for (int i = Level_Checker[2], j = 0; i < Level_Checker[3]; i++, j++) {//LV4 단계 순서만 부분저장
		PARTIAL_ORDER[3][j] = UTF_FTU_ORDER[i];
	}

	for (int i = Level_Checker[3], j = 0; i < Level_Checker[4]; i++, j++) {//LV5 단계 순서만 부분저장
		PARTIAL_ORDER[4][j] = UTF_FTU_ORDER[i];
	}

	for (int i = Level_Checker[4], j = 0; i < Level_Checker[5]; i++, j++) {//LV6 단계 순서만 부분저장
		PARTIAL_ORDER[5][j] = UTF_FTU_ORDER[i];
	}

	for (int i = Level_Checker[5], j = 0; i < Level_Checker[6]; i++, j++) {//LV7 단계 순서만 부분저장
		PARTIAL_ORDER[6][j] = UTF_FTU_ORDER[i];
	}
	for (int i = Level_Checker[6], j = 0; i < Level_Checker[7]; i++, j++) {//LV8 단계 순서만 부분저장
		PARTIAL_ORDER[7][j] = UTF_FTU_ORDER[i];
	}
}

void UTF_FTU_Add(void) //로테이트 오더에서 FB, UD가 만날때마다 UTF, FTU 추가해줌
{
	bool U_Ui_D_Di_on = 1;
	int index = 0;
	
	for (int i = 0; i < 199; i++) {

		if (
			(U_Ui_D_Di_on == 1) &&

			((ROTATE_ORDER[i] == 9) ||	//F
			(ROTATE_ORDER[i] == 10) ||	//Fi
				(ROTATE_ORDER[i] == 11) ||	//B
				(ROTATE_ORDER[i] == 12))	//Bi
			)
		{
			UTF_FTU_ORDER[i + index] = UTF;
			UTF_FTU_ORDER[i + 1 + index] = ROTATE_ORDER[i];
			U_Ui_D_Di_on = 0;
			index++;
		}
		else if (
			(U_Ui_D_Di_on == 0) &&

			((ROTATE_ORDER[i] == 5) ||	//U
			(ROTATE_ORDER[i] == 6) ||	//Ui
				(ROTATE_ORDER[i] == 7) ||	//D
				(ROTATE_ORDER[i] == 8))		//Di

			)
		{
			UTF_FTU_ORDER[i + index] = FTU;
			UTF_FTU_ORDER[i + 1 + index] = ROTATE_ORDER[i];
			U_Ui_D_Di_on = 1;
			index++;
		}
		else {
			UTF_FTU_ORDER[i + index] = ROTATE_ORDER[i];
		}
	}
}

int UTF_FTU_Count(void) //로테이트 오더에서 FB, UD가 만날때마다 UTF, FTU 횟수를 계산해줌
{
	bool U_Ui_D_Di_on = 1;
	int index = 0;

	for (int i = 0; i < 199; i++) {

		if (
			(U_Ui_D_Di_on == 1) &&

			((ROTATE_ORDER[i] == 9) ||	//F
			(ROTATE_ORDER[i] == 10) ||	//Fi
				(ROTATE_ORDER[i] == 11) ||	//B
				(ROTATE_ORDER[i] == 12))	//Bi
			)
		{
			U_Ui_D_Di_on = 0;
			index++;
		}
		else if (
			(U_Ui_D_Di_on == 0) &&

			((ROTATE_ORDER[i] == 5) ||	//U
			(ROTATE_ORDER[i] == 6) ||	//Ui
				(ROTATE_ORDER[i] == 7) ||	//D
				(ROTATE_ORDER[i] == 8))		//Di

			)
		{
			U_Ui_D_Di_on = 1;
			index++;
		}
	}

	return index;
}

void shift_order_move2(int rotate_order[], int num) {

	while (1) {
		rotate_order[num] = rotate_order[num + 2];

		if (rotate_order[num] == 0) { break; }
		num++;
	}
}
void shift_order_move1(int rotate_order[], int num) {

	while (1) {
		rotate_order[num] = rotate_order[num + 1];

		if (rotate_order[num] == 0) { break; }
		num++;
	}
}


void Rotate180_Order_make(int rotate_order[]) {


		bool flag = 0;


		while (1) {
			for (int i = 0; i < 190; i++) { //변환이 다 끝났는지 확인하는 구간
				if ((rotate_order[i] == rotate_order[i + 1]) &&
					( rotate_order[i] < 13) && rotate_order[i]!=0) {
					flag = 1; //변환 해줘야할 녀석이 아직 있구나!
					break;
				}
			}

			if (flag == 0)
			{
				break; //quit this function
			}
			else {
			}

			for (int i = 0; i < 190; i++) {
				if (//여기는 RR => R2, RiRi=> R2, 등등 180도 회전으로 바꿔준다.회전2개가 1개로 바뀐다.
					((flag == 1) && (rotate_order[i] == rotate_order[i + 1]) && (rotate_order[i] != 0))
					)
				{
					if (rotate_order[i] == 1 || rotate_order[i] == 2) // R또는 Ri가 두번 연속일때
					{
						rotate_order[i] = R2;
						shift_order_move1(rotate_order, i + 1);
					}

					else if (rotate_order[i] == 3 || rotate_order[i] == 4) // L또는 Li가 두번 연속일때
					{
						rotate_order[i] = L2;
						shift_order_move1(rotate_order, i + 1);
					}

					else if (rotate_order[i] == 5 || rotate_order[i] == 6) // U또는 Ui가 두번 연속일때
					{
						rotate_order[i] = U2;
						shift_order_move1(rotate_order, i + 1);
					}

					else if (rotate_order[i] == 7 || rotate_order[i] == 8) // D또는 Di가 두번 연속일때
					{
						rotate_order[i] = D2;
						shift_order_move1(rotate_order, i + 1);
					}

					else if (rotate_order[i] == 9 || rotate_order[i] == 10) // F또는 Fi가 두번 연속일때
					{
						rotate_order[i] = F2;
						shift_order_move1(rotate_order, i + 1);
					}

					else if (rotate_order[i] == 11 || rotate_order[i] == 12) // B또는 Bi가 두번 연속일때
					{
						rotate_order[i] = B2;
						shift_order_move1(rotate_order, i + 1);
					}

					flag = 0;

				}
			}
		}

};

void Initialize_All_Global_Array() {
	cnt_blue = 0;
	cnt_green = 0;
	cnt_orange = 0;
	cnt_pink = 0;
	cnt_white = 0;
	cnt_yellow = 0;

	for (int i = 0; i < 200; i++) {
		CUBE_ARRAY[i] = 0;
		ROTATE_ORDER[i] = 0;
		TEMP_ORDER[i] = 0;
		initial_color[i] = 0;
	}

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 100; j++) {
			PARTIAL_ORDER[i][j] = 0;
		}
	}

	for (int i = 0; i < 8; i++) {
		Level_Checker[i] = 0;
	}
	for (int i = 0; i < 400; i++) {
		UTF_FTU_ORDER[i] = 0;
	}
}

int main()
{

	system("v4l2-ctl --set-ctrl white_balance_temperature_auto=0");
	system("v4l2-ctl --set-ctrl white_balance_temperature=4800");
	system("v4l2-ctl --set-ctrl brightness=100");
	system("v4l2-ctl --set-ctrl contrast=33");
	system("v4l2-ctl --set-ctrl saturation=32");
	system("v4l2-ctl --set-ctrl gain=26");
	system("v4l2-ctl --set-ctrl exposure_auto=1");
	system("v4l2-ctl --set-ctrl exposure_absolute=303");
	system("v4l2-ctl --set-ctrl exposure_auto_priority=0");
	// 카메라의 여러 부가 기능을 전부 off 시킴

	if (wiringPiSetup() == -1)
		return 1;
	// 라즈베리파이의 초기화


	printf("Raspberry Pi Led control C++\n");
	pinMode(DOWNSTEP, OUTPUT);
	pinMode(DOWNDIR, OUTPUT);
	pinMode(UPDIR, OUTPUT);
	pinMode(UPSTEP, OUTPUT);
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(BUTTON_A, INPUT);
	pinMode(BUTTON_B, INPUT);
	pullUpDnControl(BUTTON_A, PUD_UP);
	pullUpDnControl(BUTTON_B, PUD_UP);
	//버튼 A,B 풀업모드 설정
	



	

	system("echo 0=147>/dev/servoblaster");
	system("echo 1=150>/dev/servoblaster");
	system("echo 2=143>/dev/servoblaster");
	system("echo 3=151>/dev/servoblaster");
	system("echo 4=70>/dev/servoblaster");
	// 서보모터 4개 모두 기본 위치로 초기화


	while (1) 
	{// 전체 루프
		digitalWrite(LED1, HIGH);
		digitalWrite(LED2, HIGH);

		Initialize_All_Global_Array();
		// 모든 변수를 0으로 초기화

		while (1) { //버튼A 입력 대기 모드
			if (digitalRead(BUTTON_A) == LOW) {// 버튼A가 눌러졌을 때, 무한루프 탈출
				delay(10);
				break;
			}
		}

		Dos_opencv();

		cout << "orange count : " << cnt_orange << endl;
		cout << "pink count : " << cnt_pink << endl;
		cout << "white count : " << cnt_white << endl;
		cout << "yellow count : " << cnt_yellow << endl;
		cout << "blue count : " << cnt_blue << endl;
		cout << "gren count : " << cnt_green << endl << endl << endl;

		if (cnt_orange != 9 || cnt_pink != 9 || cnt_white != 9 || cnt_yellow != 9 || cnt_blue != 9 || cnt_green != 9) {
			cout << "Color recognition failed. Press button A to shoot the cube again." << endl;

		}

		blink(); // 촬영전 LED를 3번 깜빡임

		LV1(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv1_TARGET_array);
		Level_Checker[0] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count();
		//각 단계별로 풀이를 계산한 후, 행렬에 저장함

		LV2(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv2_TARGET_array);
		Level_Checker[1] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count();

		LV3(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv2_TARGET_array);
		Level_Checker[2] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count();

		LV4(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv2_TARGET_array);
		Level_Checker[3] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count();

		LV5(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv2_TARGET_array);
		Level_Checker[4] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count();

		LV6(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv2_TARGET_array);
		Level_Checker[5] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count();

		LV7(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv2_TARGET_array);
		Level_Checker[6] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count();

		LV8(CUBE_ARRAY, initial_color, ROTATE_ORDER, Lv2_TARGET_array);
		Level_Checker[7] = check_order_empty(ROTATE_ORDER) + UTF_FTU_Count(); //나중에 회전순서를 단계별로 나눌때 사용할 인덱스를 계산함

		UTF_FTU_Add(); // 중간중간 마다 UTF, FTU를 추가하여 ROTATE_ORDER의 내용을 복사함

		Partial_order_save(); // 회전순서를 단계별로 나누어서 각 단계별로 저장함



		simplify_order(PARTIAL_ORDER[0]);//1 단계부터
		Rotate180_Order_make(PARTIAL_ORDER[0]);

		simplify_order(PARTIAL_ORDER[1]);
		Rotate180_Order_make(PARTIAL_ORDER[1]);

		simplify_order(PARTIAL_ORDER[2]);
		Rotate180_Order_make(PARTIAL_ORDER[2]);

		simplify_order(PARTIAL_ORDER[3]);
		Rotate180_Order_make(PARTIAL_ORDER[3]);

		simplify_order(PARTIAL_ORDER[4]);
		Rotate180_Order_make(PARTIAL_ORDER[4]);

		simplify_order(PARTIAL_ORDER[5]);
		Rotate180_Order_make(PARTIAL_ORDER[5]);

		simplify_order(PARTIAL_ORDER[6]);
		Rotate180_Order_make(PARTIAL_ORDER[6]);

		simplify_order(PARTIAL_ORDER[7]);// 마지막 8단계까지 순서를 최적화해줌
		Rotate180_Order_make(PARTIAL_ORDER[7]);

		bool flag_cube_complete = false;

		if (ROTATE_ORDER[0] == 0) {
			//큐브가 이미 완성이 되어 있는가? 완성되어 있다면 아래를 실행

			int difficult = 0;
			flag_cube_complete = true; //큐브가 완성되었으면 플래그를 1로 만들어줌

			// 큐브의 난이도를 물어보는 문장
			cout << "When you type a number that indicates how difficult it is to solve a cube,"
			   	<< "then the cube Solver mixes the cube accordingly.\n";
			cout << "Hard       : 1\n";
			cout << "medium     : 2\n";
			cout << "easy       : 3\n";


			cin >> difficult;//난이도를 입력 받음

			srand((unsigned int)time(NULL)); //랜덤함수

			if (difficult == 1)//hard mode
			{
				for (int i = 0; i < 9; i++)
				{
					ROTATE_ORDER[i] = ((rand() % 12) + 1);
				}
			}
			else if (difficult == 2) //medium
			{
				for (int i = 0; i < 6; i++)
				{
					ROTATE_ORDER[i] = ((rand() % 12) + 1);
				}
			}
			else if (difficult == 3) //easy
			{
				for (int i = 0; i < 3; i++)
				{
					ROTATE_ORDER[i] = ((rand() % 12) + 1);
				}
			}

			int Length = sizeof(ROTATE_ORDER) / sizeof(int);
			Rotate_Motor(ROTATE_ORDER, Length); //난이도대로 모터를 회전시킴
			RotateOrder_Reverse(Length); // 회전순서를 반대로 저장함
		}
		else {
			//큐브가 완성이 안되있다면 아래를 실행

			flag_cube_complete = false; // 완성이 안됬음을 표시하는 플래그
			cout << endl << endl << "***Clean Order***" << endl; // 화면에 이 문장을 출력
			simplify_order(ROTATE_ORDER); //회전순서가 엄청 기니까 최적화한다.
		}
		show_order(ROTATE_ORDER); // 회전순서를 보여준다.

		int B_count = 0;
		while (1) {
			if (digitalRead(BUTTON_A) == LOW)//A 버튼이 눌러지면 while문 탈출해서 다시 A입력모드로 넘어간다.
			{
				delay(10);
				break;
			}


			if (digitalRead(BUTTON_B) == LOW) {
				delay(10);	//B 버튼이 눌러지면 단계별로 진행하거나 A스위치 입력모드로 넘어간다.
				if (flag_cube_complete == false) { //큐브가 완성이 안되었다면, 아래를 실행
					if (B_count == 8) break; // 이미 완성되었다면 A스위치 입력 대기 상태로 넘어간다. 

					//완성이 안되었다면 아래의 2문장을 실행한다.
					Rotate_Motor(PARTIAL_ORDER[B_count], 100);//모터를 1단계씩 돌려준다.
					B_count++; //B를 1씩 증가해줌
				}
				else
				{
					// 큐브가 완성되었을 때 반전시켰던 순서를 여기서 실행시켜준다.
					Rotate_Motor(ROTATE_ORDER, 100); 
					Initialize_All_Global_Array();
					// 관련된 변수를 모두 초기화 시켜준다.
				}
			}
		}
	}
	return 0;
}

int Rotate::R(int cube_array[], int order[])
{
	int temp1 = cube_array[5];
	int temp2 = cube_array[17];
	int temp3 = cube_array[29];
	cube_array[5] = cube_array[41];
	cube_array[17] = cube_array[53];
	cube_array[29] = cube_array[65];
	cube_array[41] = cube_array[77];
	cube_array[53] = cube_array[89];
	cube_array[65] = cube_array[101];
	cube_array[77] = cube_array[69];
	cube_array[89] = cube_array[57];
	cube_array[101] = cube_array[45];
	cube_array[45] = temp3;
	cube_array[57] = temp2;
	cube_array[69] = temp1;
	int temp4 = cube_array[54];
	cube_array[54] = cube_array[67];
	cube_array[67] = cube_array[56];
	cube_array[56] = cube_array[43];
	cube_array[43] = temp4;
	int temp5 = cube_array[42];
	cube_array[42] = cube_array[66];
	cube_array[66] = cube_array[68];
	cube_array[68] = cube_array[44];
	cube_array[44] = temp5;

	int index = check_order_empty(order);
	order[index] = 1;
	return 1;
}
int Rotate::Ri(int cube_array[], int order[])
{
	int temp1 = cube_array[77];
	int temp2 = cube_array[89];
	int temp3 = cube_array[101];
	cube_array[77] = cube_array[41];
	cube_array[89] = cube_array[53];
	cube_array[101] = cube_array[65];
	cube_array[41] = cube_array[5];
	cube_array[53] = cube_array[17];
	cube_array[65] = cube_array[29];
	cube_array[5] = cube_array[69];
	cube_array[17] = cube_array[57];
	cube_array[29] = cube_array[45];
	cube_array[45] = temp3;
	cube_array[57] = temp2;
	cube_array[69] = temp1;
	int temp4 = cube_array[42];
	cube_array[42] = cube_array[44];
	cube_array[44] = cube_array[68];
	cube_array[68] = cube_array[66];
	cube_array[66] = temp4;
	int temp5 = cube_array[54];
	cube_array[54] = cube_array[43];
	cube_array[43] = cube_array[56];
	cube_array[56] = cube_array[67];
	cube_array[67] = temp5;

	int index = check_order_empty(order);
	order[index] = 2;
	return 2;
}
int Rotate::L(int cube_array[], int order[])
{
	int temp1 = cube_array[75];
	int temp2 = cube_array[87];
	int temp3 = cube_array[99];
	cube_array[75] = cube_array[39];
	cube_array[87] = cube_array[51];
	cube_array[99] = cube_array[63];
	cube_array[39] = cube_array[3];
	cube_array[51] = cube_array[15];
	cube_array[63] = cube_array[27];
	cube_array[3] = cube_array[71];
	cube_array[15] = cube_array[59];
	cube_array[27] = cube_array[47];
	cube_array[47] = temp3;
	cube_array[59] = temp2;
	cube_array[71] = temp1;
	int temp4 = cube_array[50];
	cube_array[50] = cube_array[37];
	cube_array[37] = cube_array[48];
	cube_array[48] = cube_array[61];
	cube_array[61] = temp4;
	int temp5 = cube_array[36];
	cube_array[36] = cube_array[60];
	cube_array[60] = cube_array[62];
	cube_array[62] = cube_array[38];
	cube_array[38] = temp5;
	int index = check_order_empty(order);
	order[index] = 3;
	return 3;
}
int Rotate::Li(int cube_array[], int order[])
{
	int temp1 = cube_array[3];
	int temp2 = cube_array[15];
	int temp3 = cube_array[27];
	cube_array[3] = cube_array[39];
	cube_array[15] = cube_array[51];
	cube_array[27] = cube_array[63];
	cube_array[39] = cube_array[75];
	cube_array[51] = cube_array[87];
	cube_array[63] = cube_array[99];
	cube_array[75] = cube_array[71];
	cube_array[87] = cube_array[59];
	cube_array[99] = cube_array[47];
	cube_array[47] = temp3;
	cube_array[59] = temp2;
	cube_array[71] = temp1;
	int temp4 = cube_array[37];
	cube_array[37] = cube_array[50];
	cube_array[50] = cube_array[61];
	cube_array[61] = cube_array[48];
	cube_array[48] = temp4;

	int temp5 = cube_array[36];
	cube_array[36] = cube_array[38];
	cube_array[38] = cube_array[62];
	cube_array[62] = cube_array[60];
	cube_array[60] = temp5;

	int index = check_order_empty(order);
	order[index] = 4;

	return 4;
}
int Rotate::U(int cube_array[], int order[])
{
	int temp1 = cube_array[36];
	int temp2 = cube_array[37];
	int temp3 = cube_array[38];
	cube_array[36] = cube_array[39];
	cube_array[37] = cube_array[40];
	cube_array[38] = cube_array[41];
	cube_array[39] = cube_array[42];
	cube_array[40] = cube_array[43];
	cube_array[41] = cube_array[44];
	cube_array[42] = cube_array[45];
	cube_array[43] = cube_array[46];
	cube_array[44] = cube_array[47];
	cube_array[45] = temp1;
	cube_array[46] = temp2;
	cube_array[47] = temp3;
	int temp4 = cube_array[3];
	cube_array[3] = cube_array[27];
	cube_array[27] = cube_array[29];
	cube_array[29] = cube_array[5];
	cube_array[5] = temp4;
	int temp5 = cube_array[4];
	cube_array[4] = cube_array[15];
	cube_array[15] = cube_array[28];
	cube_array[28] = cube_array[17];
	cube_array[17] = temp5;

	int index = check_order_empty(order);
	order[index] = 5;

	return 5;
}
int Rotate::Ui(int cube_array[], int order[])
{
	int temp1 = cube_array[45];
	int temp2 = cube_array[46];
	int temp3 = cube_array[47];
	cube_array[45] = cube_array[42];
	cube_array[46] = cube_array[43];
	cube_array[47] = cube_array[44];
	cube_array[42] = cube_array[39];
	cube_array[43] = cube_array[40];
	cube_array[44] = cube_array[41];
	cube_array[39] = cube_array[36];
	cube_array[40] = cube_array[37];
	cube_array[41] = cube_array[38];
	cube_array[36] = temp1;
	cube_array[37] = temp2;
	cube_array[38] = temp3;
	int temp4 = cube_array[4];
	cube_array[4] = cube_array[17];
	cube_array[17] = cube_array[28];
	cube_array[28] = cube_array[15];
	cube_array[15] = temp4;
	int temp5 = cube_array[5];
	cube_array[5] = cube_array[29];
	cube_array[29] = cube_array[27];
	cube_array[27] = cube_array[3];
	cube_array[3] = temp5;

	int index = check_order_empty(order);
	order[index] = 6;

	return 6;
}
int Rotate::D(int cube_array[], int order[])
{
	int temp1 = cube_array[69];
	int temp2 = cube_array[70];
	int temp3 = cube_array[71];
	cube_array[69] = cube_array[66];
	cube_array[70] = cube_array[67];
	cube_array[71] = cube_array[68];
	cube_array[66] = cube_array[63];
	cube_array[67] = cube_array[64];
	cube_array[68] = cube_array[65];
	cube_array[63] = cube_array[60];
	cube_array[64] = cube_array[61];
	cube_array[65] = cube_array[62];
	cube_array[60] = temp1;
	cube_array[61] = temp2;
	cube_array[62] = temp3;
	int temp4 = cube_array[76];
	cube_array[76] = cube_array[87];
	cube_array[87] = cube_array[100];
	cube_array[100] = cube_array[89];
	cube_array[89] = temp4;
	int temp5 = cube_array[75];
	cube_array[75] = cube_array[99];
	cube_array[99] = cube_array[101];
	cube_array[101] = cube_array[77];
	cube_array[77] = temp5;

	int index = check_order_empty(order);
	order[index] = 7;
	return 7;
}
int Rotate::Di(int cube_array[], int order[])
{
	int temp1 = cube_array[60];
	int temp2 = cube_array[61];
	int temp3 = cube_array[62];
	cube_array[60] = cube_array[63];
	cube_array[61] = cube_array[64];
	cube_array[62] = cube_array[65];
	cube_array[63] = cube_array[66];
	cube_array[64] = cube_array[67];
	cube_array[65] = cube_array[68];
	cube_array[66] = cube_array[69];
	cube_array[67] = cube_array[70];
	cube_array[68] = cube_array[71];
	cube_array[69] = temp1;
	cube_array[70] = temp2;
	cube_array[71] = temp3;
	int temp4 = cube_array[76];
	cube_array[76] = cube_array[89];
	cube_array[89] = cube_array[100];
	cube_array[100] = cube_array[87];
	cube_array[87] = temp4;
	int temp5 = cube_array[75];
	cube_array[75] = cube_array[77];
	cube_array[77] = cube_array[101];
	cube_array[101] = cube_array[99];
	cube_array[99] = temp5;

	int index = check_order_empty(order);
	order[index] = 8;
	return 8;
}
int Rotate::F(int cube_array[], int order[])
{
	int temp1 = cube_array[27];
	int temp2 = cube_array[28];
	int temp3 = cube_array[29];
	cube_array[27] = cube_array[62];
	cube_array[28] = cube_array[50];
	cube_array[29] = cube_array[38];
	cube_array[62] = cube_array[77];
	cube_array[50] = cube_array[76];
	cube_array[38] = cube_array[75];
	cube_array[77] = cube_array[42];
	cube_array[76] = cube_array[54];
	cube_array[75] = cube_array[66];
	cube_array[42] = temp1;
	cube_array[54] = temp2;
	cube_array[66] = temp3;
	int temp4 = cube_array[40];
	cube_array[40] = cube_array[51];
	cube_array[51] = cube_array[64];
	cube_array[64] = cube_array[53];
	cube_array[53] = temp4;
	int temp5 = cube_array[39];
	cube_array[39] = cube_array[63];
	cube_array[63] = cube_array[65];
	cube_array[65] = cube_array[41];
	cube_array[41] = temp5;

	int index = check_order_empty(order);
	order[index] = 9;
	return 9;
}
int Rotate::Fi(int cube_array[], int order[])
{
	int temp1 = cube_array[42];
	int temp2 = cube_array[54];
	int temp3 = cube_array[66];
	cube_array[42] = cube_array[77];
	cube_array[54] = cube_array[76];
	cube_array[66] = cube_array[75];
	cube_array[77] = cube_array[62];
	cube_array[76] = cube_array[50];
	cube_array[75] = cube_array[38];
	cube_array[62] = cube_array[27];
	cube_array[50] = cube_array[28];
	cube_array[38] = cube_array[29];
	cube_array[27] = temp1;
	cube_array[28] = temp2;
	cube_array[29] = temp3;
	int temp4 = cube_array[40];
	cube_array[40] = cube_array[53];
	cube_array[53] = cube_array[64];
	cube_array[64] = cube_array[51];
	cube_array[51] = temp4;
	int temp5 = cube_array[63];
	cube_array[63] = cube_array[39];
	cube_array[39] = cube_array[41];
	cube_array[41] = cube_array[65];
	cube_array[65] = temp5;

	int index = check_order_empty(order);
	order[index] = 10;
	return 10;
}
int Rotate::B(int cube_array[], int order[])
{
	int temp1 = cube_array[44];
	int temp2 = cube_array[56];
	int temp3 = cube_array[68];
	cube_array[44] = cube_array[101];
	cube_array[56] = cube_array[100];
	cube_array[68] = cube_array[99];
	cube_array[101] = cube_array[60];
	cube_array[100] = cube_array[48];
	cube_array[99] = cube_array[36];
	cube_array[60] = cube_array[3];
	cube_array[48] = cube_array[4];
	cube_array[36] = cube_array[5];
	cube_array[3] = temp1;
	cube_array[4] = temp2;
	cube_array[5] = temp3;
	int temp4 = cube_array[46];
	cube_array[46] = cube_array[57];
	cube_array[57] = cube_array[70];
	cube_array[70] = cube_array[59];
	cube_array[59] = temp4;
	int temp5 = cube_array[45];
	cube_array[45] = cube_array[69];
	cube_array[69] = cube_array[71];
	cube_array[71] = cube_array[47];
	cube_array[47] = temp5;

	int index = check_order_empty(order);
	order[index] = 11;
	return 11;
}
int Rotate::Bi(int cube_array[], int order[])
{
	int temp1 = cube_array[5];
	int temp2 = cube_array[4];
	int temp3 = cube_array[3];
	cube_array[5] = cube_array[36];
	cube_array[4] = cube_array[48];
	cube_array[3] = cube_array[60];
	cube_array[36] = cube_array[99];
	cube_array[48] = cube_array[100];
	cube_array[60] = cube_array[101];
	cube_array[99] = cube_array[68];
	cube_array[100] = cube_array[56];
	cube_array[101] = cube_array[44];
	cube_array[68] = temp1;
	cube_array[56] = temp2;
	cube_array[44] = temp3;
	int temp4 = cube_array[46];
	cube_array[46] = cube_array[59];
	cube_array[59] = cube_array[70];
	cube_array[70] = cube_array[57];
	cube_array[57] = temp4;
	int temp5 = cube_array[47];
	cube_array[47] = cube_array[71];
	cube_array[71] = cube_array[69];
	cube_array[69] = cube_array[45];
	cube_array[45] = temp5;

	int index = check_order_empty(order);
	order[index] = 12;
	return 12;
}

void color_initializer(int cube_array[], int color[]) {
	for (int i = 0; i < 200; i++)
	{
		cube_array[i] = color[i];
	}

}
void show_order(int order[]) {
	for (int i = 0; i < 199; i++)
	{
		//if (i % 5 == 0) cout << endl;
		if (order[i] == 0)
		{
			cout << "Done!\n";
			break;
		}
		switch (order[i])
		{

		case 1:
			cout << "R ->";
			break;
		case 2:
			cout << "Ri ->";
			break;
		case 3:
			cout << "L ->";
			break;
		case 4:
			cout << "Li ->";
			break;
		case 5:
			cout << "U ->";
			break;
		case 6:
			cout << "Ui ->";
			break;
		case 7:
			cout << "D ->";
			break;
		case 8:
			cout << "Di ->";
			break;
		case 9:
			cout << "F ->";
			break;
		case 10:
			cout << "Fi ->";
			break;
		case 11:
			cout << "B ->";
			break;
		case 12:
			cout << "Bi ->";
			break;
		case R2:
			cout << "R2 ->";
			break;
		case L2:
			cout << "L2 ->";
			break;
		case U2:
			cout << "U2 ->";
			break;
		case D2:
			cout << "D2 ->";
			break;
		case F2:
			cout << "F2 ->";
			break;
		case B2:
			cout << "B2 ->";
			break;

		}
	}

}
int check_order_empty(int order[]) {// 배열 요소가 0일 때 인덱스넘버를 리턴한다.
	int num = 0;
	for (int i = 0; i < 200; i++)
	{
		if (order[i] == 0)
		{
			num = i;
			break;
		}
	}
	return num;
}
bool check_front_cross(int cube_array[])//십자가가 완성되었을때 true를 리턴
{
	if (
		(cube_array[40] == cube_array[52])
		&& (cube_array[51] == cube_array[52])
		&& (cube_array[53] == cube_array[52])
		&& (cube_array[64] == cube_array[52])
		)
		return true;
	else
		return false;
}
int Lv1_search_target_block(int target_array[], int cube_array[]) {
	//target_array의 값은 타겟의 범위다.
	//타겟을 발견하는 순간 target_array의 값을 읽어 타겟의 위치, 큐브의 인덱스를 리턴한다.
	int num = 0;
	for (int i = 0; i < 50; i++)
	{
		if ((cube_array[target_array[i]] == cube_array[52]))
		{
			num = target_array[i];
			break;
		}
	}
	return num;
}
int Give_target_block_partner_location(int target_location) {//타겟을 찾으면 그 옆면의 요소를 찾는다.
	int num = 0;
	if (target_location == 4)
		num = 46;
	if (target_location == 15)
		num = 37;
	if (target_location == 28)
		num = 40;
	if (target_location == 17)
		num = 43;
	if (target_location == 50)
		num = 51;
	if (target_location == 54)
		num = 53;
	if (target_location == 56)
		num = 57;
	if (target_location == 59)
		num = 48;
	if (target_location == 76)
		num = 64;
	if (target_location == 89)
		num = 67;
	if (target_location == 61)
		num = 87;
	if (target_location == 70)
		num = 100;
	if (target_location == 46)
		num = 4;
	if (target_location == 43)
		num = 17;
	if (target_location == 37)
		num = 15;
	if (target_location == 48)
		num = 59;
	if (target_location == 57)
		num = 56;
	if (target_location == 87)
		num = 61;
	if (target_location == 67)
		num = 89;
	if (target_location == 100)
		num = 70;

	return num;


}
int search_side_center(int cube_array[], int partner_location) {
	//타겟블록 옆면의 요소와 같은 색깔을 가진 옆면의 중앙을 찾음
	int num = 0;
	if (cube_array[16] == cube_array[partner_location])
		num = 16;
	if (cube_array[49] == cube_array[partner_location])
		num = 49;
	if (cube_array[88] == cube_array[partner_location])
		num = 88;
	if (cube_array[55] == cube_array[partner_location])
		num = 55;
	if (cube_array[58] == cube_array[partner_location])
		num = 58;
	return num;
}
void HowToRotate(int target_location, int order[]) {
	// 회전과 충돌검사를 계속 반복하면서 ROTATE_ORDER에 회전할때마다 기록한다.
	if (target_location == 4) {
		Rotate R;
		R.U(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Ri(CUBE_ARRAY, order);
	}
	if (target_location == 15) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.L(CUBE_ARRAY, order);
	}

	if (target_location == 17) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Ri(CUBE_ARRAY, order);
	}

	if (target_location == 28) {
		Rotate R;
		R.U(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.L(CUBE_ARRAY, order);
	}

	if (target_location == 37) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Ui(CUBE_ARRAY, order);
	}

	if (target_location == 48) {
		Rotate R;
		R.L(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Ui(CUBE_ARRAY, order);
	}

	if (target_location == 50) {
		Rotate R;
		R.L(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.D(CUBE_ARRAY, order);
	}

	if (target_location == 61) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.D(CUBE_ARRAY, order);
	}

	if (target_location == 76) {
		Rotate R;
		R.D(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.R(CUBE_ARRAY, order);
	}

	if (target_location == 87) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Li(CUBE_ARRAY, order);
	}

	if (target_location == 89) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.R(CUBE_ARRAY, order);
	}

	if (target_location == 100) {
		Rotate R;
		R.D(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Li(CUBE_ARRAY, order);
	}

	if (target_location == 43) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.U(CUBE_ARRAY, order);
	}

	if (target_location == 54) {
		Rotate R;
		R.R(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.U(CUBE_ARRAY, order);
	}
	if (target_location == 56) {
		Rotate R;
		R.R(CUBE_ARRAY, order);
		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Di(CUBE_ARRAY, order);
	}
	if (target_location == 67) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.Di(CUBE_ARRAY, order);
	}
	if (target_location == 46) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.U(CUBE_ARRAY, order);
		R.U(CUBE_ARRAY, order);
	}
	if (target_location == 57) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.R(CUBE_ARRAY, order);
		R.R(CUBE_ARRAY, order);
	}
	if (target_location == 59) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.L(CUBE_ARRAY, order);
		R.L(CUBE_ARRAY, order);
	}
	if (target_location == 70) {
		Rotate R;

		while (1)
		{
			if (Check_collision(target_location, order) == false)
				break;
		}
		R.D(CUBE_ARRAY, order);
		R.D(CUBE_ARRAY, order);
	}



}
int Check_collision(int target_location, int order[])
{
	int num = 0;
	Rotate R;
	if (target_location == 15)
	{
		if (CUBE_ARRAY[51] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 17)
	{
		if (CUBE_ARRAY[53] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 37)
	{
		if (CUBE_ARRAY[40] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 61)
	{
		if (CUBE_ARRAY[64] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 87)
	{
		if (CUBE_ARRAY[51] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 89)
	{
		if (CUBE_ARRAY[53] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 43)
	{
		if (CUBE_ARRAY[40] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 67)
	{
		if (CUBE_ARRAY[64] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 46)
	{
		if (CUBE_ARRAY[40] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 57)
	{
		if (CUBE_ARRAY[53] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 59)
	{
		if (CUBE_ARRAY[51] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	if (target_location == 70)
	{
		if (CUBE_ARRAY[64] == CUBE_ARRAY[52])

		{
			R.F(CUBE_ARRAY, order);
			num = true;
		}
		else {
			num = false;
		}
	}
	return num;

}
void LV1(int cube_array[], int color[], int rotate_order[], int lv1_target_array[])
{
	color_initializer(CUBE_ARRAY, initial_color);
	while (1) {

		if (check_front_cross(CUBE_ARRAY) == true)
		{

			show_order(ROTATE_ORDER);//결과를 보고싶다면 주석해제하면 된다.
			cout << "LV1 is Complete" << endl;
			break;
		}

		int target_location = Lv1_search_target_block(Lv1_TARGET_array, CUBE_ARRAY);
		//만약에 타겟을 발견할경우 타겟의 위치를 넘겨준다. 못 찾는다면??
		//(여기서 타겟이란 십자가의 수평수직성분이 옆면에 흩어져 있는 블록을 말한다.)


		HowToRotate(target_location, ROTATE_ORDER);
		// 회전과 충돌검사를 계속 반복하면서 ROTATE_ORDER에 회전할때마다 기록한다.

	}

}
void LV2(int cube_array[], int color[], int rotate_order[], int lv2_target_array[])
{
	Rotate rotate;
	int order_index = check_order_empty(ROTATE_ORDER);
	while (1) {
		if (check_SideOfCross_equal_SideOfCenter_COLOR(CUBE_ARRAY) == true)
		{//LV2가 완성되었는지 물어본다.
			LV_show_order(ROTATE_ORDER, order_index);//결과를 보고싶다면 주석해제하면 된다.
			cout << "LV2 is Complete" << endl;
			break;
		}


		if (CUBE_ARRAY[76] != CUBE_ARRAY[88]) {
			rotate.D(CUBE_ARRAY, ROTATE_ORDER);
			rotate.D(CUBE_ARRAY, ROTATE_ORDER);
		}
		if (CUBE_ARRAY[54] != CUBE_ARRAY[55]) {
			rotate.R(CUBE_ARRAY, ROTATE_ORDER);
			rotate.R(CUBE_ARRAY, ROTATE_ORDER);
		}
		if (CUBE_ARRAY[28] != CUBE_ARRAY[16]) {
			rotate.U(CUBE_ARRAY, ROTATE_ORDER);
			rotate.U(CUBE_ARRAY, ROTATE_ORDER);
		}
		if (CUBE_ARRAY[50] != CUBE_ARRAY[49]) {
			rotate.L(CUBE_ARRAY, ROTATE_ORDER);
			rotate.L(CUBE_ARRAY, ROTATE_ORDER);
		}
		if (CUBE_ARRAY[64] != CUBE_ARRAY[52]) {
			while (1) {

				if (CUBE_ARRAY[88] == CUBE_ARRAY[100])
				{
					if (CUBE_ARRAY[70] == CUBE_ARRAY[52])

						break;

				}
				rotate.B(CUBE_ARRAY, ROTATE_ORDER);
			}
			rotate.D(CUBE_ARRAY, ROTATE_ORDER);
			rotate.D(CUBE_ARRAY, ROTATE_ORDER);

		}

		if (CUBE_ARRAY[53] != CUBE_ARRAY[52]) {
			while (1) {
				if (CUBE_ARRAY[55] == CUBE_ARRAY[56])
				{
					if (CUBE_ARRAY[57] == CUBE_ARRAY[52])
					{
						break;
					}

				}

				rotate.B(CUBE_ARRAY, ROTATE_ORDER);
			}
			rotate.R(CUBE_ARRAY, ROTATE_ORDER);
			rotate.R(CUBE_ARRAY, ROTATE_ORDER);

		}



		if (CUBE_ARRAY[40] != CUBE_ARRAY[52]) {
			while (1) {
				if (CUBE_ARRAY[16] == CUBE_ARRAY[4])
				{
					if (CUBE_ARRAY[46] == CUBE_ARRAY[52])
						break;
				}
				rotate.B(CUBE_ARRAY, ROTATE_ORDER);
			}

			rotate.U(CUBE_ARRAY, ROTATE_ORDER);
			rotate.U(CUBE_ARRAY, ROTATE_ORDER);
		}

		if (CUBE_ARRAY[51] != CUBE_ARRAY[52]) {
			while (1) {
				if (CUBE_ARRAY[48] == CUBE_ARRAY[49])
				{
					if (CUBE_ARRAY[59] == CUBE_ARRAY[52])
						break;
				}
				rotate.B(CUBE_ARRAY, ROTATE_ORDER);
			}
			rotate.L(CUBE_ARRAY, ROTATE_ORDER);
			rotate.L(CUBE_ARRAY, ROTATE_ORDER);
		}
	}
}


bool check_SideOfCross_equal_SideOfCenter_COLOR(int cube_array[]) {
	int num = 0;
	if (
		(cube_array[28] == cube_array[16])
		&& (cube_array[50] == cube_array[49])
		&& (cube_array[76] == cube_array[88])
		&& (cube_array[54] == cube_array[55])
		)
	{
		num = true;
	}
	else {
		num = false;
	}
	return num;

}
bool check_two_block_same(int partner_color) {
	int num = 0;
	if (
		(CUBE_ARRAY[16] == partner_color) ||
		(CUBE_ARRAY[49] == partner_color) ||
		(CUBE_ARRAY[88] == partner_color) ||
		(CUBE_ARRAY[55] == partner_color))
		num = true;
	else
		num = false;
	return num;
}
bool Check_exist_target(int cube_array[]) {
	int num = 0;
	if (
		(cube_array[46] == cube_array[52]) ||
		(cube_array[59] == cube_array[52]) ||
		(cube_array[70] == cube_array[52]) ||
		(cube_array[57] == cube_array[52]))
		num = true;
	else
		num = false;
	return num;
}
int LV2_search_target_block(int target_array[], int cube_array[])
{
	int num = 0;
	for (int i = 0; i < 4; i++)
	{
		if ((cube_array[target_array[i]] == cube_array[52]))
		{
			num = target_array[i];

		}
	}
	return num;
}
void recursive(int cube_array[])
{
	Rotate rotate;
	if (Check_exist_target(CUBE_ARRAY) == false)
	{
		return;
	}
	if (CUBE_ARRAY[70] == CUBE_ARRAY[52])
	{
		if (CUBE_ARRAY[88] == CUBE_ARRAY[100])
		{
			rotate.D(CUBE_ARRAY, ROTATE_ORDER);
			rotate.D(CUBE_ARRAY, ROTATE_ORDER);
			return;
		}
		if (CUBE_ARRAY[88] != CUBE_ARRAY[100])
		{
			rotate.B(CUBE_ARRAY, ROTATE_ORDER);
			if (CUBE_ARRAY[55] == CUBE_ARRAY[56])
			{
				rotate.R(CUBE_ARRAY, ROTATE_ORDER);
				rotate.R(CUBE_ARRAY, ROTATE_ORDER);
				return;
			}
			if (CUBE_ARRAY[55] != CUBE_ARRAY[56])
			{
				rotate.B(CUBE_ARRAY, ROTATE_ORDER);
				if (CUBE_ARRAY[4] == CUBE_ARRAY[16])
				{
					rotate.U(CUBE_ARRAY, ROTATE_ORDER);
					rotate.U(CUBE_ARRAY, ROTATE_ORDER);
					return;
				}
				if (CUBE_ARRAY[4] != CUBE_ARRAY[16])
				{
					rotate.B(CUBE_ARRAY, ROTATE_ORDER);
					if (CUBE_ARRAY[49] == CUBE_ARRAY[48])
					{
						rotate.L(CUBE_ARRAY, ROTATE_ORDER);
						rotate.L(CUBE_ARRAY, ROTATE_ORDER);
						return;
					}
				}


			}
		}

	}
	if (CUBE_ARRAY[70] != CUBE_ARRAY[52])
	{
		while (1) {
			if (CUBE_ARRAY[70] == CUBE_ARRAY[52])
			{
				break;
			}
			rotate.B(CUBE_ARRAY, ROTATE_ORDER);
		}
	}
}
void LV_show_order(int order[], int order_index) {
	{

		for (int i = order_index; i < 199; i++)
		{
			if (order[i] == 0)
			{
				cout << "Done!\n";
				break;
			}
			switch (order[i])
			{

			case 1:
				cout << "R ->";
				break;
			case 2:
				cout << "Ri ->";
				break;
			case 3:
				cout << "L ->";
				break;
			case 4:
				cout << "Li ->";
				break;
			case 5:
				cout << "U ->";
				break;
			case 6:
				cout << "Ui ->";
				break;
			case 7:
				cout << "D ->";
				break;
			case 8:
				cout << "Di ->";
				break;
			case 9:
				cout << "F ->";
				break;
			case 10:
				cout << "Fi ->";
				break;
			case 11:
				cout << "B ->";
				break;
			case 12:
				cout << "Bi ->";
				break;

			}
		}

	}
}
void LV3(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]) {

	int order_index = check_order_empty(ROTATE_ORDER);
	while (1) {
		if (check_LV3_complete(CUBE_ARRAY) == true)
		{//LV3가 완성되었는지 물어본다.

			LV_show_order(ROTATE_ORDER, order_index);//결과를 보고싶다면 주석해제하면 된다.
			cout << "LV3 is Complete!" << endl;
			break;
		}

		// LV3가 완성 되었는지 검사한다. 완성되면 무한루프에서 탈출한다.
		side3_assemble(cube_array);
		//side3 어셈블은 조금 불필요한 뒷면회전이 있지만, 어쨌거나 무사히 임무를 완수한다.
		side1_move_to_side3(cube_array);
		side3_assemble(cube_array);
		front_move_to_side3(cube_array);
		side3_assemble(cube_array);
		bottom_move_to_side3(cube_array);
		side3_assemble(cube_array);

	}
}
bool check_LV3_complete(int cube_array[]) {
	int num = 0;
	if (
		(cube_array[39] == cube_array[52]) &&
		(cube_array[40] == cube_array[52]) &&
		(cube_array[41] == cube_array[52]) &&
		(cube_array[51] == cube_array[52]) &&
		(cube_array[53] == cube_array[52]) &&
		(cube_array[63] == cube_array[52]) &&
		(cube_array[64] == cube_array[52]) &&
		(cube_array[65] == cube_array[52])
		)
	{
		if (
			(cube_array[75] == cube_array[88]) &&
			(cube_array[77] == cube_array[88]) &&
			(cube_array[66] == cube_array[55]) &&
			(cube_array[42] == cube_array[55]) &&
			(cube_array[27] == cube_array[16]) &&
			(cube_array[29] == cube_array[16]) &&
			(cube_array[38] == cube_array[49]) &&
			(cube_array[62] == cube_array[49])
			)
		{
			num = true;// LV3가 완성되면 true를 리턴한다.
		}

	}
	else {
		num = false;
	}
	return num;

}
void side3_assemble(int cube_array[]) {
	while (1)
	{
		if (LV3_side3_exist(cube_array) == false)
		{

			break;
		}
		if (cube_array[99] == cube_array[52]) {
			Is_equal_60_49(cube_array);
			Is_equal_101_88(cube_array);
			Is_equal_44_55(cube_array);
			Is_equal_3_16(cube_array);
			//cout << "설마";
		}
		if (cube_array[101] == cube_array[52]) {
			Is_equal_68_55(cube_array);
			Is_equal_5_16(cube_array);
			Is_equal_36_49(cube_array);
			Is_equal_99_88(cube_array);
		}
		if (cube_array[68] == cube_array[52]) {
			Is_equal_101_88(cube_array);
			Is_equal_44_55(cube_array);
			Is_equal_3_16(cube_array);
			Is_equal_60_49(cube_array);
		}
		if (cube_array[44] == cube_array[52]) {
			Is_equal_5_16(cube_array);
			Is_equal_36_49(cube_array);
			Is_equal_99_88(cube_array);
			Is_equal_68_55(cube_array);
		}
		if (cube_array[5] == cube_array[52]) {
			Is_equal_44_55(cube_array);
			Is_equal_3_16(cube_array);
			Is_equal_60_49(cube_array);
			Is_equal_101_88(cube_array);
		}
		if (cube_array[3] == cube_array[52]) {
			Is_equal_36_49(cube_array);
			Is_equal_99_88(cube_array);
			Is_equal_68_55(cube_array);
			Is_equal_5_16(cube_array);
		}
		if (cube_array[36] == cube_array[52]) {
			Is_equal_3_16(cube_array);
			Is_equal_60_49(cube_array);
			Is_equal_101_88(cube_array);
			Is_equal_44_55(cube_array);
		}
		if (cube_array[60] == cube_array[52]) {
			Is_equal_99_88(cube_array);
			Is_equal_68_55(cube_array);
			Is_equal_5_16(cube_array);
			Is_equal_36_49(cube_array);
		}
	}
}
void Is_equal_60_49(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[60] == cube_array[49]) && (cube_array[99] == cube_array[52]))
	{
		rotate.Di(cube_array, ROTATE_ORDER);
		rotate.Bi(cube_array, ROTATE_ORDER);
		rotate.D(cube_array, ROTATE_ORDER);
	}
	else
		rotate.B(cube_array, ROTATE_ORDER);
}//잘작동
void Is_equal_101_88(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[101] == cube_array[88]) && (cube_array[68] == cube_array[52]))
	{
		rotate.Ri(cube_array, ROTATE_ORDER);
		rotate.Bi(cube_array, ROTATE_ORDER);
		rotate.R(cube_array, ROTATE_ORDER);
	}
	else
		rotate.B(cube_array, ROTATE_ORDER);
}//잘 작동
void Is_equal_44_55(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[44] == cube_array[55]) && (cube_array[5] == cube_array[52]))
	{
		rotate.Ui(cube_array, ROTATE_ORDER);
		rotate.Bi(cube_array, ROTATE_ORDER);
		rotate.U(cube_array, ROTATE_ORDER);

	}//맞다
	else
		rotate.B(cube_array, ROTATE_ORDER);
}
void Is_equal_3_16(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[3] == cube_array[16]) && (cube_array[36] == cube_array[52]))
	{
		rotate.Li(cube_array, ROTATE_ORDER);
		rotate.Bi(cube_array, ROTATE_ORDER);
		rotate.L(cube_array, ROTATE_ORDER);
	}
	else
		rotate.B(cube_array, ROTATE_ORDER);
}
void Is_equal_36_49(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[36] == cube_array[49]) && (cube_array[3] == cube_array[52]))
	{
		rotate.U(cube_array, ROTATE_ORDER);
		rotate.B(cube_array, ROTATE_ORDER);
		rotate.Ui(cube_array, ROTATE_ORDER);
	}
	else
		rotate.B(cube_array, ROTATE_ORDER);
}
void Is_equal_99_88(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[99] == cube_array[88]) && (cube_array[60] == cube_array[52]))
	{
		rotate.L(cube_array, ROTATE_ORDER);
		rotate.B(cube_array, ROTATE_ORDER);
		rotate.Li(cube_array, ROTATE_ORDER);
	}
	else
		rotate.B(cube_array, ROTATE_ORDER);
}
void Is_equal_68_55(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[68] == cube_array[55]) && (cube_array[101] == cube_array[52]))
	{
		rotate.D(cube_array, ROTATE_ORDER);
		rotate.B(cube_array, ROTATE_ORDER);
		rotate.Di(cube_array, ROTATE_ORDER);
	}
	else
		rotate.B(cube_array, ROTATE_ORDER);
}
void Is_equal_5_16(int cube_array[]) {
	Rotate rotate;
	if ((cube_array[5] == cube_array[16]) && (cube_array[44] == cube_array[52]))
	{
		rotate.R(cube_array, ROTATE_ORDER);
		rotate.B(cube_array, ROTATE_ORDER);
		rotate.Ri(cube_array, ROTATE_ORDER);
	}
	else
		rotate.B(cube_array, ROTATE_ORDER);
}
bool LV3_side3_exist(int cube_array[]) {
	int num = 0;
	if (
		(cube_array[99] == cube_array[52]) ||
		(cube_array[101] == cube_array[52]) ||
		(cube_array[68] == cube_array[52]) ||
		(cube_array[44] == cube_array[52]) ||
		(cube_array[5] == cube_array[52]) ||
		(cube_array[3] == cube_array[52]) ||
		(cube_array[36] == cube_array[52]) ||
		(cube_array[60] == cube_array[52])
		)
	{
		num = true;
	}
	else
	{
		num = false;
	}
	return num;
}
void side1_move_to_side3(int cube_array[]) {
	Rotate rotate;
	while (1)
	{
		if (LV3_side1_exist(cube_array) == false)
		{
			break;
		}

		if (cube_array[66] == cube_array[52]) {
			rotate.Ri(cube_array, ROTATE_ORDER);
			rotate.Bi(cube_array, ROTATE_ORDER);
			rotate.R(cube_array, ROTATE_ORDER);
		}
		if (cube_array[29] == cube_array[52]) {
			rotate.Ui(cube_array, ROTATE_ORDER);
			rotate.Bi(cube_array, ROTATE_ORDER);
			rotate.U(cube_array, ROTATE_ORDER);
		}
		if (cube_array[38] == cube_array[52]) {
			rotate.Li(cube_array, ROTATE_ORDER);
			rotate.Bi(cube_array, ROTATE_ORDER);
			rotate.L(cube_array, ROTATE_ORDER);
		}
		if (cube_array[75] == cube_array[52]) {
			rotate.Di(cube_array, ROTATE_ORDER);
			rotate.Bi(cube_array, ROTATE_ORDER);
			rotate.D(cube_array, ROTATE_ORDER);
		}
		if (cube_array[42] == cube_array[52]) {
			rotate.R(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Ri(cube_array, ROTATE_ORDER);
		}
		if (cube_array[27] == cube_array[52]) {
			rotate.U(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Ui(cube_array, ROTATE_ORDER);
		}
		if (cube_array[62] == cube_array[52]) {
			rotate.L(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Li(cube_array, ROTATE_ORDER);
		}
		if (cube_array[77] == cube_array[52]) {
			rotate.D(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Di(cube_array, ROTATE_ORDER);
		}
	}
}
bool LV3_side1_exist(int cube_array[]) {
	int num = 0;
	if (
		(cube_array[75] == cube_array[52]) ||
		(cube_array[77] == cube_array[52]) ||
		(cube_array[66] == cube_array[52]) ||
		(cube_array[42] == cube_array[52]) ||
		(cube_array[29] == cube_array[52]) ||
		(cube_array[27] == cube_array[52]) ||
		(cube_array[38] == cube_array[52]) ||
		(cube_array[62] == cube_array[52])
		)
	{
		num = true;
	}
	else
	{
		num = false;
	}
	return num;
}
void front_move_to_side3(int cube_array[]) {
	Rotate rotate;
	while (1)
	{
		if (LV3_front_exist(cube_array) == false)
		{
			break;
		}

		if ((cube_array[65] == cube_array[52]) && (cube_array[77] != cube_array[76]))
		{
			rotate.D(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Di(cube_array, ROTATE_ORDER);
		}
		if ((cube_array[41] == cube_array[52]) && (cube_array[42] != cube_array[54]))
		{
			rotate.R(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Ri(cube_array, ROTATE_ORDER);
		}
		if ((cube_array[39] == cube_array[52]) && (cube_array[27] != cube_array[28])) {
			rotate.U(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Ui(cube_array, ROTATE_ORDER);
		}
		if ((cube_array[63] == cube_array[52]) && (cube_array[62] != cube_array[50])) {
			rotate.L(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Li(cube_array, ROTATE_ORDER);
		}

	}
}
bool LV3_front_exist(int cube_array[]) {
	int num = 0;
	if (
		((cube_array[65] == cube_array[52]) && (cube_array[77] != cube_array[76])) ||
		((cube_array[41] == cube_array[52]) && (cube_array[42] != cube_array[54])) ||
		((cube_array[39] == cube_array[52]) && (cube_array[27] != cube_array[28])) ||
		((cube_array[63] == cube_array[52]) && (cube_array[62] != cube_array[50]))
		)
	{
		num = true;
	}

	else {
		num = false;
	}
	return num;
}
void bottom_move_to_side3(int cube_array[]) {
	Rotate rotate;
	while (1)
	{
		if (LV3_bottom_exist(cube_array) == false)
		{
			break;
		}

		if (cube_array[69] == cube_array[52]) {
			rotate.Ri(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.R(cube_array, ROTATE_ORDER);
		}
		if (cube_array[45] == cube_array[52]) {
			rotate.Ui(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.U(cube_array, ROTATE_ORDER);
		}
		if (cube_array[47] == cube_array[52]) {
			rotate.Li(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.L(cube_array, ROTATE_ORDER);
		}
		if (cube_array[71] == cube_array[52]) {
			rotate.Di(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.D(cube_array, ROTATE_ORDER);
		}

	}
}
bool LV3_bottom_exist(int cube_array[]) {
	int num = 0;
	if (
		(cube_array[45] == cube_array[52]) ||
		(cube_array[47] == cube_array[52]) ||
		(cube_array[69] == cube_array[52]) ||
		(cube_array[71] == cube_array[52])
		)
	{
		num = true;
	}

	else {
		num = false;
	}
	return num;
}

void simplify_order(int rotate_order[]) {
	while (1) {
		if (check_able_simplify(rotate_order) == false) {

			break;
		}

		for (int i = 0; i < 196; i++)
		{
			if (
				(rotate_order[i] == rotate_order[i + 1]) && (rotate_order[i + 1] == rotate_order[i + 2])
				
				)//연달아서 RRR,FFF나오는지 검사한다. 그리고 RRR이면 Ri로 치환하고 배열을 앞으로 당겨준다.
			{
				if (rotate_order[i] == 1)
				{
					rotate_order[i] = 2;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 2)
				{
					rotate_order[i] = 1;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 3)
				{
					rotate_order[i] = 4;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 4)
				{
					rotate_order[i] = 3;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 5)
				{
					rotate_order[i] = 6;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 6)
				{
					rotate_order[i] = 5;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 7)
				{
					rotate_order[i] = 8;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 8)
				{
					rotate_order[i] = 7;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 9)
				{
					rotate_order[i] = 10;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 10)
				{
					rotate_order[i] = 9;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
				if (rotate_order[i] == 11)
				{

					rotate_order[i] = 12;
					shift_order_move2(rotate_order, i + 1);
					break;
				}if (rotate_order[i] == 12)
				{
					rotate_order[i] = 11;
					shift_order_move2(rotate_order, i + 1);
					break;
				}
			}
			if (//여기는 R과 Ri가 서로 만나는지 검사를 한다. 서로 만나면 배열을 그냥 뒤에서 앞으로 덮어 씌운다.
				((rotate_order[i] == 1) && (rotate_order[i + 1] == 2)) ||
				((rotate_order[i] == 2) && (rotate_order[i + 1] == 1)) ||
				((rotate_order[i] == 3) && (rotate_order[i + 1] == 4)) ||
				((rotate_order[i] == 4) && (rotate_order[i + 1] == 3)) ||
				((rotate_order[i] == 5) && (rotate_order[i + 1] == 6)) ||
				((rotate_order[i] == 6) && (rotate_order[i + 1] == 5)) ||
				((rotate_order[i] == 7) && (rotate_order[i + 1] == 8)) ||
				((rotate_order[i] == 8) && (rotate_order[i + 1] == 7)) ||
				((rotate_order[i] == 9) && (rotate_order[i + 1] == 10)) ||
				((rotate_order[i] == 10) && (rotate_order[i + 1] == 9)) ||
				((rotate_order[i] == 11) && (rotate_order[i + 1] == 12)) ||
				((rotate_order[i] == 12) && (rotate_order[i + 1] == 11))
				)
			{
				shift_order_move2(rotate_order, i);
			}
			
		}

	}
}
bool check_able_simplify(int rotate_order[]) {
	int num = 0;
	for (int i = 0; i < 196; i++) {
		if (
			(((rotate_order[i] == rotate_order[i + 1]) &&  (rotate_order[i + 1] == rotate_order[i + 2])) &&
				rotate_order[i] != 0) ||
			((rotate_order[i] == 1) && (rotate_order[i + 1] == 2)) ||
			((rotate_order[i] == 2) && (rotate_order[i + 1] == 1)) ||
			((rotate_order[i] == 3) && (rotate_order[i + 1] == 4)) ||
			((rotate_order[i] == 4) && (rotate_order[i + 1] == 3)) ||
			((rotate_order[i] == 5) && (rotate_order[i + 1] == 6)) ||
			((rotate_order[i] == 6) && (rotate_order[i + 1] == 5)) ||
			((rotate_order[i] == 7) && (rotate_order[i + 1] == 8)) ||
			((rotate_order[i] == 8) && (rotate_order[i + 1] == 7)) ||
			((rotate_order[i] == 9) && (rotate_order[i + 1] == 10)) ||
			((rotate_order[i] == 10) && (rotate_order[i + 1] == 9)) ||
			((rotate_order[i] == 11) && (rotate_order[i + 1] == 12)) ||
			((rotate_order[i] == 12) && (rotate_order[i + 1] == 11))
			)
		{
			num = true;
			break;
		}
		else
			num = false;
	}
	return num;
}

void LV4(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]) {

	Rotate rotate;
	int order_index = check_order_empty(ROTATE_ORDER);
	while (1) {
		if (check_LV4_complete(CUBE_ARRAY) == true)
		{//LV4가 완성되었는지 물어본다.

			LV_show_order(ROTATE_ORDER, order_index);//결과를 보고싶다면 주석해제하면 된다.
			cout << "LV4 is Complete!" << endl;
			break;
		}
		if (is_LV4_side3_target_exist(cube_array) == false)
		{
			if (
				((cube_array[88] == cube_array[89]) && (cube_array[55] == cube_array[67])) == false
				)
			{

				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Ri(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Di(cube_array, ROTATE_ORDER);

				continue;
			}
			if (
				((cube_array[55] == cube_array[43]) && (cube_array[17] == cube_array[16])) == false
				)
			{

				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Ui(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Ri(cube_array, ROTATE_ORDER);

				continue;
			}
			if (
				((cube_array[15] == cube_array[16]) && (cube_array[37] == cube_array[49])) == false
				)
			{
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Ui(cube_array, ROTATE_ORDER);
				continue;
			}
			if (
				((cube_array[49] == cube_array[61]) && (cube_array[87] == cube_array[88])) == false
				)
			{
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Di(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				continue;
			}
		}
		if (is_LV4_side3_target_exist(cube_array) == true)
		{
			if (cube_array[100] == cube_array[88])
			{
				if (cube_array[70] == cube_array[49])
				{
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);

					continue;
				}
				if (cube_array[70] == cube_array[55])
				{
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);

					continue;
				}
			}
			if (cube_array[56] == cube_array[55])
			{
				if (cube_array[57] == cube_array[88])
				{
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					continue;
				}
				if (cube_array[57] == cube_array[16])
				{
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);

					continue;
				}
			}
			if (cube_array[4] == cube_array[16])
			{

				if (cube_array[46] == cube_array[49])
				{
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					continue;
				}
				if (cube_array[46] == cube_array[55])
				{
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					continue;
				}
			}
			if (cube_array[48] == cube_array[49])
			{
				if (cube_array[59] == cube_array[16])
				{
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					continue;
				}
				if (cube_array[59] == cube_array[88])
				{
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					continue;
				}
			}
			rotate.B(cube_array, ROTATE_ORDER);
		}
	}
}
bool check_LV4_complete(int cube_array[])
{
	int num = 0;
	if
		(
		(cube_array[88] == cube_array[87]) && (cube_array[88] == cube_array[89]) &&
			(cube_array[55] == cube_array[67]) && (cube_array[55] == cube_array[43]) &&
			(cube_array[16] == cube_array[15]) && (cube_array[16] == cube_array[17]) &&
			(cube_array[49] == cube_array[37]) && (cube_array[49] == cube_array[61])
			)

	{
		num = true;// LV4가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;

}
bool is_LV4_side3_target_exist(int cube_array[]) {
	int num = 0;
	if (
		(
		((cube_array[100] == cube_array[88]) && (cube_array[70] != cube_array[58])) ||
			((cube_array[100] == cube_array[55]) && (cube_array[70] != cube_array[58])) ||
			((cube_array[100] == cube_array[16]) && (cube_array[70] != cube_array[58])) ||
			((cube_array[100] == cube_array[49]) && (cube_array[70] != cube_array[58]))
			) ||

			(
		((cube_array[56] == cube_array[88]) && (cube_array[57] != cube_array[58])) ||
				((cube_array[56] == cube_array[55]) && (cube_array[57] != cube_array[58])) ||
				((cube_array[56] == cube_array[16]) && (cube_array[57] != cube_array[58])) ||
				((cube_array[56] == cube_array[49]) && (cube_array[57] != cube_array[58]))
				) ||
				(
		((cube_array[4] == cube_array[88]) && (cube_array[46] != cube_array[58])) ||
					((cube_array[4] == cube_array[55]) && (cube_array[46] != cube_array[58])) ||
					((cube_array[4] == cube_array[16]) && (cube_array[46] != cube_array[58])) ||
					((cube_array[4] == cube_array[49]) && (cube_array[46] != cube_array[58]))
					) ||
					(
		((cube_array[48] == cube_array[88]) && (cube_array[59] != cube_array[58])) ||
						((cube_array[48] == cube_array[55]) && (cube_array[59] != cube_array[58])) ||
						((cube_array[48] == cube_array[16]) && (cube_array[59] != cube_array[58])) ||
						((cube_array[48] == cube_array[49]) && (cube_array[59] != cube_array[58]))
						)
		)
	{
		num = true;
	}
	else
		num = false;
	return num;
}
void LV5(int cube_array[], int color[], int rotate_order[], int lv2_target_array[])
{
	Rotate rotate;
	int order_index = check_order_empty(ROTATE_ORDER);
	while (1) {
		if (check_LV5_complete(CUBE_ARRAY) == true)
		{//LV5가 완성되었는지 물어본다.

			LV_show_order(ROTATE_ORDER, order_index);
			cout << "LV5 is Complete!" << endl;
			break;
		}
		if (is_LV5_L_shape_exist(cube_array) == true) {
			if ((cube_array[57] == cube_array[58]) && (cube_array[46] == cube_array[58]))
			{
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.Di(cube_array, ROTATE_ORDER);
				continue;
			}
			rotate.B(cube_array, ROTATE_ORDER);

			continue;
		}
		if (is_LV5_bar_shape_exist(cube_array) == true)
		{
			if (
				(cube_array[58] == cube_array[57]) && (cube_array[58] == cube_array[59])
				)
			{
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				rotate.Di(cube_array, ROTATE_ORDER);
				continue;
			}
			rotate.B(cube_array, ROTATE_ORDER);

			continue;
		}

		//나머지 한 점일 때 실행되는 것들
		rotate.D(cube_array, ROTATE_ORDER);
		rotate.B(cube_array, ROTATE_ORDER);
		rotate.L(cube_array, ROTATE_ORDER);
		rotate.Bi(cube_array, ROTATE_ORDER);
		rotate.Li(cube_array, ROTATE_ORDER);
		rotate.Di(cube_array, ROTATE_ORDER);
	}
}
bool check_LV5_complete(int cube_array[])
{
	int num = 0;
	if
		(
		(cube_array[57] == cube_array[58]) &&
			(cube_array[46] == cube_array[58]) &&
			(cube_array[59] == cube_array[58]) &&
			(cube_array[70] == cube_array[58])
			)

	{
		num = true;// LV5가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;

}
bool is_LV5_L_shape_exist(int cube_array[])
{
	int num = 0;
	if
		(
		((cube_array[57] == cube_array[58]) && (cube_array[46] == cube_array[58])) ||
			((cube_array[46] == cube_array[58]) && (cube_array[59] == cube_array[58])) ||
			((cube_array[59] == cube_array[58]) && (cube_array[70] == cube_array[58])) ||
			((cube_array[70] == cube_array[58]) && (cube_array[57] == cube_array[58]))
			)

	{
		num = true;// LV4가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;

}
bool is_LV5_bar_shape_exist(int cube_array[])
{
	int num = 0;
	if
		(
		((cube_array[57] == cube_array[58]) && (cube_array[59] == cube_array[58])) ||
			((cube_array[46] == cube_array[58]) && (cube_array[70] == cube_array[58]))
			)

	{
		num = true;// LV4가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;

}

void LV6(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]) {
	Rotate rotate;
	int order_index = check_order_empty(ROTATE_ORDER);
	while (1) {
		if (check_LV6_complete(CUBE_ARRAY) == true)
		{//LV6 바닥면 전체가 완성되었는지 물어본다.

			LV_show_order(ROTATE_ORDER, order_index);
			cout << "LV6 is Complete!" << endl;
			break;
		}
		if (is_LV6_No_coner(cube_array) == true) {
			rotate.L(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Li(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.L(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.B(cube_array, ROTATE_ORDER);
			rotate.Li(cube_array, ROTATE_ORDER);
			continue;
		}
		if (is_LV6_One_Conner_exist(cube_array) == true) {
			if (
				(cube_array[45] != cube_array[58]) &&
				(cube_array[47] != cube_array[58]) &&
				(cube_array[69] == cube_array[58]) &&
				(cube_array[71] != cube_array[58])
				)
			{
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				continue;
			}
			rotate.B(cube_array, ROTATE_ORDER);
			continue;
		}
		if (is_LV6_Two_Conner_exist(cube_array) == true) {
			if (cube_array[101] == cube_array[58])
			{
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.B(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				continue;
			}
			rotate.B(cube_array, ROTATE_ORDER);
			continue;
		}
	}
}
bool check_LV6_complete(int cube_array[]) {
	int num = 0;
	if
		(
		(cube_array[45] == cube_array[58]) &&
			(cube_array[46] == cube_array[58]) &&
			(cube_array[47] == cube_array[58]) &&
			(cube_array[57] == cube_array[58]) &&
			(cube_array[59] == cube_array[58]) &&
			(cube_array[69] == cube_array[58]) &&
			(cube_array[70] == cube_array[58]) &&
			(cube_array[71] == cube_array[58])

			)

	{
		num = true;// LV6가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;

}
bool is_LV6_No_coner(int cube_array[]) {
	int num = 0;
	if
		(

		(cube_array[45] != cube_array[58]) &&
			(cube_array[47] != cube_array[58]) &&
			(cube_array[69] != cube_array[58]) &&
			(cube_array[71] != cube_array[58])

			)

	{
		num = true;// LV4가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;

}

bool is_LV6_One_Conner_exist(int cube_array[]) {
	int num = 0;
	if
		(
		(
			(cube_array[45] == cube_array[58]) &&
			(cube_array[47] != cube_array[58]) &&
			(cube_array[69] != cube_array[58]) &&
			(cube_array[71] != cube_array[58])
			) ||
			(
			(cube_array[45] != cube_array[58]) &&
				(cube_array[47] == cube_array[58]) &&
				(cube_array[69] != cube_array[58]) &&
				(cube_array[71] != cube_array[58])
				) ||
				(
			(cube_array[45] != cube_array[58]) &&
					(cube_array[47] != cube_array[58]) &&
					(cube_array[69] == cube_array[58]) &&
					(cube_array[71] != cube_array[58])
					) ||
					(
			(cube_array[45] != cube_array[58]) &&
						(cube_array[47] != cube_array[58]) &&
						(cube_array[69] != cube_array[58]) &&
						(cube_array[71] == cube_array[58])
						)

			)

	{
		num = true;
	}


	else {
		num = false;
	}
	return num;
}
bool is_LV6_Two_Conner_exist(int cube_array[]) {
	int num = 0;
	if
		(
		(
			(cube_array[45] == cube_array[58]) &&
			(cube_array[47] == cube_array[58]) &&
			(cube_array[69] != cube_array[58]) &&
			(cube_array[71] != cube_array[58])
			) ||
			(
			(cube_array[45] == cube_array[58]) &&
				(cube_array[47] != cube_array[58]) &&
				(cube_array[69] == cube_array[58]) &&
				(cube_array[71] != cube_array[58])
				) ||
				(
			(cube_array[45] == cube_array[58]) &&
					(cube_array[47] != cube_array[58]) &&
					(cube_array[69] != cube_array[58]) &&
					(cube_array[71] == cube_array[58])
					) ||
					(
			(cube_array[45] != cube_array[58]) &&
						(cube_array[47] == cube_array[58]) &&
						(cube_array[69] == cube_array[58]) &&
						(cube_array[71] != cube_array[58])
						) ||
						(
			(cube_array[45] != cube_array[58]) &&
							(cube_array[47] == cube_array[58]) &&
							(cube_array[69] != cube_array[58]) &&
							(cube_array[71] == cube_array[58])
							) ||
							(
			(cube_array[45] != cube_array[58]) &&
								(cube_array[47] != cube_array[58]) &&
								(cube_array[69] == cube_array[58]) &&
								(cube_array[71] == cube_array[58])
								)
			)

	{
		num = true;
	}


	else {
		num = false;
	}
	return num;
}

void LV7(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]) {
	Rotate rotate;
	int order_index = check_order_empty(ROTATE_ORDER);
	while (1) {
		if (check_LV7_complete(CUBE_ARRAY) == true)
		{//LV7가 완성되었는지 물어본다.

			LV_show_order(ROTATE_ORDER, order_index);
			cout << "LV7 is Complete!" << endl;
			break;
		}
		if (is_LV7_same_direction(cube_array) == true) {
			if ((cube_array[99] == cube_array[88]) && (cube_array[101] == cube_array[88]))
			{
				//		cout << "88방향으로 두개 맞음" << endl;
				rotate.Ri(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.Ri(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.Ui(cube_array, ROTATE_ORDER);
				rotate.Ri(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				continue;
			}
			if ((cube_array[68] == cube_array[55]) && (cube_array[44] == cube_array[55]))
			{
				//cout << "55방향으로 두개 맞음" << endl;

				rotate.Ui(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.Ui(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.Ui(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				continue;
			}
			if ((cube_array[3] == cube_array[16]) && (cube_array[5] == cube_array[16]))
			{
				//	cout << "16방향으로 두개 맞음" << endl;


				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.Di(cube_array, ROTATE_ORDER);
				rotate.Li(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.U(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				continue;
			}
			if ((cube_array[36] == cube_array[49]) && (cube_array[60] == cube_array[49]))
			{
				//cout << "49방향으로 두개 맞음" << endl;


				rotate.Di(cube_array, ROTATE_ORDER);
				rotate.R(cube_array, ROTATE_ORDER);
				rotate.Di(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.Ri(cube_array, ROTATE_ORDER);
				rotate.Di(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.L(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.D(cube_array, ROTATE_ORDER);
				rotate.Bi(cube_array, ROTATE_ORDER);
				continue;
			}
		}
		if (is_LV7_diagonal_direction(cube_array) == true) {
			//				cout << "대각 방향으로 맞음" << endl;
			rotate.Li(cube_array, ROTATE_ORDER);
			rotate.D(cube_array, ROTATE_ORDER);
			rotate.Li(cube_array, ROTATE_ORDER);
			rotate.U(cube_array, ROTATE_ORDER);
			rotate.U(cube_array, ROTATE_ORDER);
			rotate.L(cube_array, ROTATE_ORDER);
			rotate.Di(cube_array, ROTATE_ORDER);
			rotate.Li(cube_array, ROTATE_ORDER);
			rotate.U(cube_array, ROTATE_ORDER);
			rotate.U(cube_array, ROTATE_ORDER);
			rotate.L(cube_array, ROTATE_ORDER);
			rotate.L(cube_array, ROTATE_ORDER);
			rotate.Bi(cube_array, ROTATE_ORDER);
			continue;
		}
		//					cout << "B" << endl;
		rotate.B(cube_array, ROTATE_ORDER);
	}
}
bool check_LV7_complete(int cube_array[]) {
	int num = 0;
	if
		(
		((cube_array[99] == cube_array[88]) && (cube_array[88] == cube_array[101])) &&
			((cube_array[68] == cube_array[55]) && (cube_array[44] == cube_array[55]))
			)

	{
		num = true;// LV7가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;

}
bool is_LV7_same_direction(int cube_array[]) {
	int num = 0;
	if
		(
		((cube_array[99] == cube_array[88]) && (cube_array[101] == cube_array[88])) ||
			((cube_array[68] == cube_array[55]) && (cube_array[44] == cube_array[55])) ||
			((cube_array[3] == cube_array[16]) && (cube_array[5] == cube_array[16])) ||
			((cube_array[36] == cube_array[49]) && (cube_array[60] == cube_array[49]))
			)

	{
		num = true;// LV7가 완성되면 true를 리턴한다.
	}

	else {
		num = false;
	}
	return num;
}
bool is_LV7_diagonal_direction(int cube_array[]) {
	int num = 0;
	if
		(
		(
			((cube_array[101] == cube_array[88]) && (cube_array[68] == cube_array[55])) &&
			((cube_array[36] == cube_array[49]) && (cube_array[3] == cube_array[16]))
			)
			||
			(
			((cube_array[99] == cube_array[88]) && (cube_array[60] == cube_array[49])) &&
				((cube_array[16] == cube_array[5]) && (cube_array[55] == cube_array[44]))
				)

			)

	{
		num = true;// LV7가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;
}

void LV8(int cube_array[], int color[], int rotate_order[], int lv2_target_array[]) {
	Rotate rotate;
	int order_index = check_order_empty(ROTATE_ORDER);
	while (1) {
		if (check_LV8_complete(CUBE_ARRAY) == true)
		{//LV8가 완성되었는지 물어본다.

			LV_show_order(ROTATE_ORDER, order_index);
			cout << "LV8 is Complete!, and all work is done!" << endl;
			break;
		}
		if (is_LV8_complete_side_face_exist(cube_array) == true) {
			if (is_LV8_88_direction(cube_array) == true)
			{
				if (
					(cube_array[48] == cube_array[55]) &&
					(cube_array[56] == cube_array[16]) &&
					(cube_array[4] == cube_array[49])
					)
				{
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);

					continue;
				}
				if (
					(cube_array[59] == cube_array[49]) &&
					(cube_array[48] == cube_array[16]) &&
					(cube_array[4] == cube_array[55])
					)
				{
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					continue;
				}
			}
			if (is_LV8_55_direction(cube_array) == true)
			{
				if (
					(cube_array[100] == cube_array[16]) &&
					(cube_array[4] == cube_array[49]) &&
					(cube_array[48] == cube_array[88])
					)
				{
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					continue;
				}
				if (
					(cube_array[100] == cube_array[49]) &&
					(cube_array[48] == cube_array[16]) &&
					(cube_array[4] == cube_array[88])
					)
				{
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					continue;
				}
			}
			if (is_LV8_16_direction(cube_array) == true)
			{
				if (
					(cube_array[56] == cube_array[49]) &&
					(cube_array[48] == cube_array[88]) &&
					(cube_array[100] == cube_array[55])
					)
				{
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					continue;
				}
				if (
					(cube_array[56] == cube_array[88]) &&
					(cube_array[100] == cube_array[49]) &&
					(cube_array[48] == cube_array[55])
					)
				{
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Li(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.Ri(cube_array, ROTATE_ORDER);
					rotate.L(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					continue;
				}
			}
			if (is_LV8_49_direction(cube_array) == true)
			{
				if (
					(cube_array[4] == cube_array[88]) &&
					(cube_array[55] == cube_array[100]) &&
					(cube_array[56] == cube_array[16])
					)
				{
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.B(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					continue;
				}
				if (
					(cube_array[4] == cube_array[55]) &&
					(cube_array[56] == cube_array[88]) &&
					(cube_array[100] == cube_array[16])
					)
				{
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.U(cube_array, ROTATE_ORDER);
					rotate.Di(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.Ui(cube_array, ROTATE_ORDER);
					rotate.D(cube_array, ROTATE_ORDER);
					rotate.Bi(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					rotate.R(cube_array, ROTATE_ORDER);
					continue;
				}
			}
		}
		rotate.U(cube_array, ROTATE_ORDER);
		rotate.U(cube_array, ROTATE_ORDER);
		rotate.B(cube_array, ROTATE_ORDER);
		rotate.L(cube_array, ROTATE_ORDER);
		rotate.Ri(cube_array, ROTATE_ORDER);
		rotate.U(cube_array, ROTATE_ORDER);
		rotate.U(cube_array, ROTATE_ORDER);
		rotate.Li(cube_array, ROTATE_ORDER);
		rotate.R(cube_array, ROTATE_ORDER);
		rotate.B(cube_array, ROTATE_ORDER);
		rotate.U(cube_array, ROTATE_ORDER);
		rotate.U(cube_array, ROTATE_ORDER);
	}
}
bool check_LV8_complete(int cube_array[]) {
	int num = 0;
	if
		(
		(cube_array[100] == cube_array[88]) &&
			(cube_array[56] == cube_array[55]) &&
			(cube_array[4] == cube_array[16]) &&
			(cube_array[48] == cube_array[49])
			)

	{
		num = true;// LV8가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;
}
bool is_LV8_complete_side_face_exist(int cube_array[]) {
	int num = 0;
	if
		(
		(cube_array[100] == cube_array[88]) ||
			(cube_array[56] == cube_array[55]) ||
			(cube_array[4] == cube_array[16]) ||
			(cube_array[48] == cube_array[49])
			)

	{
		num = true;// LV8가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;
}
bool is_LV8_88_direction(int cube_array[]) {
	int num = 0;
	if
		(
		(cube_array[100] == cube_array[88])
			)

	{
		num = true;// LV8가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;
}
bool is_LV8_55_direction(int cube_array[]) {
	int num = 0;
	if
		(
		(cube_array[56] == cube_array[55])
			)

	{
		num = true;// LV8가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;
}
bool is_LV8_16_direction(int cube_array[]) {
	int num = 0;
	if
		(
			cube_array[4] == cube_array[16]
			)

	{
		num = true;// LV8가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;
}
bool is_LV8_49_direction(int cube_array[]) {
	int num = 0;
	if
		(
			cube_array[48] == cube_array[49]
			)

	{
		num = true;// LV8가 완성되면 true를 리턴한다.
	}


	else {
		num = false;
	}
	return num;
}
void Dos_opencv() {
	int represent_color_3x3[10];
	color_Value color_represent;
	int bit_B[10];
	int bit_G[10];
	int bit_R[10];

	Mat image;			 
	int picture_num = 0;
	for (;;) {


		VideoCapture cap(0);
		cap >> image;
		//waitKey(1000);
		cap.release();

			if (!image.empty()) {
			std::vector<cv::Vec3b> colors;
			for (int r = 0; r < 3; r++) {
				for (int c = 0; c < 3; c++) {
					Rect cell = {
						BOX_X + c * CELL_SIZE,// x 
						BOX_Y + r * CELL_SIZE,//y
						CELL_SIZE,//width 
						CELL_SIZE //height
					};
					
					Mat cell_img = image(cell);

					cv::Vec3b center_pixel =
						cell_img.at<Vec3b>(r * CELL_SIZE / 2, c * CELL_SIZE / 2);


					Point pt(BOX_X + r*CELL_SIZE * 3 / 2.0, BOX_Y + c * CELL_SIZE * 3 / 2.0);
					circle(image, pt, 8, { 0,0,255 }, 2);

					colors.push_back(center_pixel);

					//rectangle(image, cell, center_pixel, -1);
					//rectangle(image, cell, { 255,0,0 }, 2);
				}
			}
			imshow("Image", image);
			waitKey(20);
				char key = ' ';
				if ((key == 'q') || (picture_num == 6)) {
				break;
			}

			//register the color
			if (key == ' ') {
				picture_num++;
				int i = 0;
				int x = 0;
				for (auto& d : colors) {
					//std::cout << d << ", ";
					i++;

					bit_B[i - 1] = d.val[0];
					bit_G[i - 1] = d.val[1];
					bit_R[i - 1] = d.val[2];

				}


				cout << colors[0] << "\t" << colors[1] << "\t" << colors[2] << endl;
				cout << colors[3] << "\t" << colors[4] << "\t" << colors[5] << endl;
				cout << colors[6] << "\t" << colors[7] << "\t" << colors[8] << endl;

				color_teller(bit_B, bit_G, bit_R, represent_color_3x3, color_represent);

				for (int i = 0; i < 9; i++) {
					switch (represent_color_3x3[i]) {
					case 1:
						cnt_pink++;
						cout << dec << "Pink\t";
						break;
					case 2:
						cnt_blue++;
						cout << dec << "Blue\t";
						break;
					case 3:
						cnt_yellow++;
						cout << dec << "Yello\t";
						break;
					case 4:
						cnt_orange++;
						cout << dec << "Orang\t";
						break;
					case 5:
						cnt_white++;
						cout << dec << "White\t";
						break;
					case 6:
						cnt_green++;
						cout << dec << "Green\t";
						break;
					default:
						break;
					}
					if (i % 3 == 2) cout << endl;
				}
				cout << " picture#" << picture_num<<" was taken" << endl << endl;
				Send_picture_data_to_cube(initial_color, represent_color_3x3, picture_num);
			}
		}
	}
}
void color_teller(int B[], int G[], int R[], int represent_color[], color_Value color) {
	for (int i = 0; i < 9; i++) {
		int white = (color.W[0] - B[i])*(color.W[0] - B[i]) + (color.W[1] - G[i])*(color.W[1] - G[i])
			+ (color.W[2] - R[i])*(color.W[2] - R[i]);

		int yellow = (color.Y[0] - B[i])*(color.Y[0] - B[i]) + (color.Y[1] - G[i])*(color.Y[1] - G[i])
			+ (color.Y[2] - R[i])*(color.Y[2] - R[i]);

		int orange = (color.O[0] - B[i])*(color.O[0] - B[i]) + (color.O[1] - G[i])*(color.O[1] - G[i])
			+ (color.O[2] - R[i])*(color.O[2] - R[i]);

		int pink = (color.pink[0] - B[i])*(color.pink[0] - B[i]) + (color.pink[1] - G[i])*(color.pink[1] - G[i])
			+ (color.pink[2] - R[i])*(color.pink[2] - R[i]);

		int light_blue = (color.light_blue[0] - B[i])*(color.light_blue[0] - B[i]) + (color.light_blue[1] - G[i])*(color.light_blue[1] - G[i])
			+ (color.light_blue[2] - R[i])*(color.light_blue[2] - R[i]);

		int light_green = (color.light_green[0] - B[i])*(color.light_green[0] - B[i]) + (color.light_green[1] - G[i])*(color.light_green[1] - G[i])
			+ (color.light_green[2] - R[i])*(color.light_green[2] - R[i]);

		int compute[6] = { white, yellow, orange, pink, light_blue, light_green };
		int min = 9999999;

		for (int i = 0; i < 6; i++) {
			if (compute[i] < min) {
				min = compute[i];
			}
		}
		if (min == white) {
			represent_color[i] = 5;
		}
		if (min == orange) {
			represent_color[i] = 4;
		}
		if (min == pink) {
			represent_color[i] = 1;
		}
		if (min == light_blue) {
			represent_color[i] = 2;
		}
		if (min == yellow) {
			represent_color[i] = 3;
		}
		if (min == light_green) {
			represent_color[i] = 6;
		}

	}
}
void Send_picture_data_to_cube(int initial_color_cv[], int represent_color[], int picture_num) {
	if (picture_num == 1)
	{
		initial_color_cv[39] = represent_color[0];
		initial_color_cv[40] = represent_color[1];
		initial_color_cv[41] = represent_color[2];
		initial_color_cv[51] = represent_color[3];
		initial_color_cv[52] = represent_color[4];
		initial_color_cv[53] = represent_color[5];
		initial_color_cv[63] = represent_color[6];
		initial_color_cv[64] = represent_color[7];
		initial_color_cv[65] = represent_color[8];
		horizontal_rotate();
	}
	if (picture_num == 2)
	{
		initial_color_cv[42] = represent_color[0];
		initial_color_cv[43] = represent_color[1];
		initial_color_cv[44] = represent_color[2];
		initial_color_cv[54] = represent_color[3];
		initial_color_cv[55] = represent_color[4];
		initial_color_cv[56] = represent_color[5];
		initial_color_cv[66] = represent_color[6];
		initial_color_cv[67] = represent_color[7];
		initial_color_cv[68] = represent_color[8];
		horizontal_rotate();
	}
	if (picture_num == 3)
	{
		initial_color_cv[45] = represent_color[0];
		initial_color_cv[46] = represent_color[1];
		initial_color_cv[47] = represent_color[2];
		initial_color_cv[57] = represent_color[3];
		initial_color_cv[58] = represent_color[4];
		initial_color_cv[59] = represent_color[5];
		initial_color_cv[69] = represent_color[6];
		initial_color_cv[70] = represent_color[7];
		initial_color_cv[71] = represent_color[8];
		horizontal_rotate();
	}
	if (picture_num == 4)
	{
		initial_color_cv[36] = represent_color[0];
		initial_color_cv[37] = represent_color[1];
		initial_color_cv[38] = represent_color[2];
		initial_color_cv[48] = represent_color[3];
		initial_color_cv[49] = represent_color[4];
		initial_color_cv[50] = represent_color[5];
		initial_color_cv[60] = represent_color[6];
		initial_color_cv[61] = represent_color[7];
		initial_color_cv[62] = represent_color[8];
		
		// horizontal rotate와 vertical back rotate를 합쳐놓은건데 약간 수정을 해줌

		move13_OUT();//holding 추가 해줄것

		servo0_R90();
		delay(2);
		servo2_90();
		//holding02();
		delay(600);

		bottom_UP();
		delay(300);

		move13_IN();
		bottom_DOWN();

		move02_OUT_Vertical();
		Delay();

		servo1_90();
		delay(2);
		servo3_R90();
		delay(500);

		bottom_UP();
		delay(300);

		/*move02_IN();
		bottom_DOWN();

		move13_OUT_Vertical();
		Delay();

		move13_IN();
		delay(300);*/

	}
	if (picture_num == 5)
	{
		initial_color_cv[3] = represent_color[0];
		initial_color_cv[4] = represent_color[1];
		initial_color_cv[5] = represent_color[2];
		initial_color_cv[15] = represent_color[3];
		initial_color_cv[16] = represent_color[4];
		initial_color_cv[17] = represent_color[5];
		initial_color_cv[27] = represent_color[6];
		initial_color_cv[28] = represent_color[7];
		initial_color_cv[29] = represent_color[8];
		
		flip180();

	}
	if (picture_num == 6)
	{
		initial_color_cv[75] = represent_color[0];
		initial_color_cv[76] = represent_color[1];
		initial_color_cv[77] = represent_color[2];
		initial_color_cv[87] = represent_color[3];
		initial_color_cv[88] = represent_color[4];
		initial_color_cv[89] = represent_color[5];
		initial_color_cv[99] = represent_color[6];
		initial_color_cv[100] = represent_color[7];
		initial_color_cv[101] = represent_color[8];


		//vertical_rotate_back이다.


		servo1_vertical();
		bottom_DOWN();
		servo3_vertical();
		delay(500);

		move02_IN();
	}

}
