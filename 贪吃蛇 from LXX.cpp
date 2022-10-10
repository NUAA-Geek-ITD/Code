#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

#define SIZE_max 35  //��ͼ��С(������һ�д�������޸�)
#define FWT 10  //���ʳ������� 
#define PWT 15  //��ҩ����仯����
#define FOOD 8  //���ʳ����
#define POISON 7  //���ҩ��
#define DI 5  //��ҩˢ�¾���ͷ����С���� 
#define RT 100  //ˢ�¼��(ms)

typedef struct{
	bool is_snake;
	bool is_food;
	//is_snake���ڱ���Ƿ�Ϊ�ߡ�is_food���ڱ���Ƿ�Ϊʳ�����ͬʱΪ����Ϊ��ҩ 
	unsigned char footmark;
	//����ߵ��㼣���������βʱָ����ͷ�������һλ�����ڸ�����βλ�� 
}MAP;

typedef struct{
	struct{
		unsigned char x;
		unsigned char y;
	}head;  //����ָʾ��ͷ���� 
	struct{
		unsigned char x;
		unsigned char y;
	}tail;  //����ָʾ��β���� 
	unsigned int length; //���ڼ�¼�߳��� 
}SNAKE; 

MAP maps[SIZE_max][SIZE_max];  //��ͼ��ά���� 
SNAKE snake;  //�ߵĴ���ռ� 
unsigned int foodrest;  //ʣ���ʳ��
unsigned int time_mark;  //��¼ʱ����Ϣ

HANDLE handle;
COORD coord;

void welcome();  //��ӭ���� 
bool finish();  //������� 
inline bool next_step(unsigned int dir);  //״̬ˢ�� 
void add_food();   //ʳ����� 
void refresh_poison();  //��ҩˢ�� 
void refresh();  //ȫ��ˢ��ͼ�� 
inline void refresh(unsigned char x,unsigned char y);  //����ˢ��ͼ�� 

inline int power(int x);  //�õ�2��X�η�(���������η�) 
inline void color(int x);  //������ɫ
inline void gotoxy(int x,int y);  //�ƶ����λ�� 
inline bool wait();  //��ͣʱ�ȴ� 

int main(){
	system("mode con cols=70 lines=36");
	system("title ̰����_SNAKE");
	system("color F0");
	handle=GetStdHandle(STD_OUTPUT_HANDLE);
	//�������ú;����ȡ 
	welcome();  //��ӭ����  
	unsigned int temp_dir,counter_food,counter_poison,counter,counter_poison_refresh;
	  /*�ֱ��ʾ
	    �ߵ��˶�����(0123�ֱ�ָ��������)
		���ϴ����ʳ������
		���ϴ���Ӷ�ҩ����
		�뿪ʼ��Ϸ����
		��Ϸ�Ѷ����Ӵ���(�ӿ춾ҩˢ��) 
	  */ 
	beginning:  //����finish�а��ո��(ʵ����wait)������1��next_step���ٷ���1���������������ת���˴��� 
		temp_dir=3;
		counter_food=0;
		counter_poison=0;
		counter=0;
		counter_poison_refresh=0;
	    foodrest=0;  //��������ʼ��
	
	//��ʼ����Ϸ���� 
	for(int i=0;i<SIZE_max;i++)
	for(int u=0;u<SIZE_max;u++){
		maps[i][u].is_snake=0;
		maps[i][u].is_food=0;
	}
	snake.head.x=SIZE_max/2+1;
	snake.head.y=SIZE_max/2+1;
	snake.tail.x=SIZE_max/2-1;
	snake.tail.y=SIZE_max/2+1;
	snake.length=3;
	maps[SIZE_max/2-1][SIZE_max/2+1].is_snake=1;
	maps[SIZE_max/2][SIZE_max/2+1].is_snake=1;
	maps[SIZE_max/2+1][SIZE_max/2+1].is_snake=1;
	maps[SIZE_max/2-1][SIZE_max/2+1].footmark=3;
	maps[SIZE_max/2][SIZE_max/2+1].footmark=3;
	maps[SIZE_max/2+1][SIZE_max/2+1].footmark=3;
	time_mark=(unsigned)time(NULL);
	srand(time_mark);
	for(int i=0;i<POISON;i++)
	refresh_poison();
	
	refresh();  //��ȫ��ˢ��
	while(1){
		if(_kbhit())
		switch(getch()){
			case 32 :  //������ͣ��Ϸ 
				time_mark=(unsigned)time(NULL)-time_mark;//��¼����ʱ�� 
				do{
					Sleep(200);
				}while(wait());
				time_mark=(unsigned)time(NULL)-time_mark;//�õ���Ч��ʱ���
				break; 
			case 27 : return 0; //�����˳����� 
			case 13 : refresh();break;//ǿ��ˢ����Ļ
			case 224:  //����⵽����� 
			    switch(getch()){
			    	case 72 : temp_dir=0;break;
			    	case 80 : temp_dir=1;break;
			    	case 75 : temp_dir=2;break;
			    	case 77 : temp_dir=3;
				} 
		}
		if(next_step(temp_dir))  //ˢ�µ�ͼ��Ϣ 
		goto beginning;  //��next_step()����1��ʾ���¿�ʼ����ת����ͷ 
		counter_food++;
		counter_poison++;  
		counter++;  //���������� 
		if((counter_food>=FWT*power(foodrest))&&(foodrest<FOOD)){
			counter_food=0;
			add_food();
		}  //���ʳ�ﲢ�����������
		if(counter_poison>=PWT-counter_poison_refresh){
			counter_poison=0;
			refresh_poison();
		}   //ˢ�¶�ҩλ�ò������������ 
		if((counter>(15000/RT)*power(counter_poison_refresh))&&counter_poison_refresh<PWT-2) 
		counter_poison_refresh++;  //�����Ѷ�
		gotoxy(0,SIZE_max); //����Ƶ����½� 
		Sleep(RT); //��ʱ 
	}
}

void welcome(){
	//��ӭ���� 
	gotoxy(SIZE_max-5,SIZE_max/2);
	cout<<"̰  ��  ��";
	gotoxy(SIZE_max-4,SIZE_max/2+2);
	cout<<"S N A K E";
	gotoxy(0,SIZE_max);
	
	if(getch()==27)
	exit(0);
}

bool finish(){
	//������� 
    color(16*15+0);	
	system("cls");
	gotoxy(0,0);
	cout<<"̰  ��  ��";
	gotoxy(1,2);
	cout<<"S N A K E";
	gotoxy(SIZE_max-11,SIZE_max/2);
	cout<<"��    Ϸ    ��    ��";
	gotoxy(SIZE_max-7,SIZE_max/2+2);
	cout<<"�ߵĳ���:"<<snake.length<<"��";
	gotoxy(SIZE_max-5,SIZE_max/2+4);
	cout<<"��ʱ:"<<((unsigned)time(NULL)-time_mark)<<"��"; 
	gotoxy(0,SIZE_max);
	
	while(wait()){
		Sleep(500);
	}
	return 1;
}

bool next_step(unsigned int dir){
	/*������֤*/ 
	if(maps[snake.head.x][snake.head.y].footmark+dir==1||maps[snake.head.x][snake.head.y].footmark+dir==5) 
	dir=maps[snake.head.x][snake.head.y].footmark;
	  //���ҽ�������֮��Ϊ1��5ʱ������ָʾ�෴����dir��Ч����footmark��ֵ��dir 
	else
	maps[snake.head.x][snake.head.y].footmark=dir;
	  //Ϊ��ͼ���·������� 
	  
	/*��ͷ�ƶ�*/ 
	switch(dir){
		case 0 : snake.head.y=(snake.head.y-1+SIZE_max)%SIZE_max;break;
		case 1 : snake.head.y=(snake.head.y+1)%SIZE_max;break;
		case 2 : snake.head.x=(snake.head.x-1+SIZE_max)%SIZE_max;break;
		case 3 : snake.head.x=(snake.head.x+1)%SIZE_max;
	}  //���ݷ���dir�ƶ���ͷ 
	maps[snake.head.x][snake.head.y].footmark=dir;
	  //Ϊ��ͷλ�õ�ͼд�뷽��(Ϊ�´��ƶ���d������֤׼��)
	  
	/*������֤*/ 
	if(maps[snake.head.x][snake.head.y].is_snake)
	if(finish()) //�����߻�ҩ��������� 
	return 1;  //Ϊ�������¿�ʼ��Ϸ(�ص�����������ת��������ʼ��ǰ)
	maps[snake.head.x][snake.head.y].is_snake=1;  //δ����������λ������Ϊ��(�����ͷλ�õ�ͼָʾ) 
	if(maps[snake.head.x][snake.head.y].is_food){ 
		maps[snake.head.x][snake.head.y].is_food=0;
		snake.length++;
		foodrest--;
		gotoxy(0,SIZE_max);
		color(16*15+0);
		cout<<"�ߵĳ���:"<<snake.length;  //�����½�ˢ���߳���ָʾ 
		refresh(snake.head.x,snake.head.y);  //ˢ����ͷ��ʹ������Ļ����ʾ
		return 0;  //����ʳ���������β�ƶ���ֱ���˳����� 
	}
	
	/*��β�ƶ�*/ 
	else
	refresh(snake.head.x,snake.head.y);  //ˢ����ͷ��ʹ������Ļ����ʾ 
	maps[snake.tail.x][snake.tail.y].is_snake=0;  //ɾ����βλ�õ�ͼָʾ 
	refresh(snake.tail.x,snake.tail.y);  //ˢ����β 
	switch(maps[snake.tail.x][snake.tail.y].footmark){
		case 0 : snake.tail.y=(snake.tail.y-1+SIZE_max)%SIZE_max;break;
		case 1 : snake.tail.y=(snake.tail.y+1)%SIZE_max;break;
		case 2 : snake.tail.x=(snake.tail.x-1+SIZE_max)%SIZE_max;break;
		case 3 : snake.tail.x=(snake.tail.x+1)%SIZE_max;
	}  //���ݷ����ƶ���β 
	return 0;
}
void add_food(){
	unsigned int x,y,temp=0;
	while(1){
		if(temp>999)
		return;  //��ֹ��ʱ���Ҳ�����λ���»��濨�� 
		temp++;
		x=rand()%SIZE_max;
		y=rand()%SIZE_max;
		if(maps[x][y].is_snake||maps[x][y].is_food)
		continue;
		break;
	}  //Ѱ�ҿ�λ�� 
	maps[x][y].is_food=1;
	foodrest++;
	refresh(x,y);
} 
void refresh_poison(){
	static unsigned int counter=0;
	static unsigned int temp_pos_x[POISON]={0},temp_pos_y[POISON]={0};
	  //���ڴ��涾ҩλ����Ϣ��ˢ��˳�� 
	unsigned int x,y,temp=0;
	while(1){
		if(temp>999)
		return;  //��ֹ��ʱ���Ҳ�����λ���»��濨�� 
		temp++;
		x=rand()%SIZE_max;
		y=rand()%SIZE_max;
		if(maps[x][y].is_snake||maps[x][y].is_food)
		continue;//�����¸�ѭ������Ѱ�� 
		if(x-snake.head.x>DI||snake.head.x-x>DI||y-snake.head.y>DI||snake.head.y-y>DI)//��������ͷ 
		break;
	}  //Ѱ�ҿտ�λ��(����ͷ��Զ) 
	if(maps[temp_pos_x[counter]][temp_pos_y[counter]].is_food&&maps[temp_pos_x[counter]][temp_pos_y[counter]].is_snake){
		maps[temp_pos_x[counter]][temp_pos_y[counter]].is_food=0;
		maps[temp_pos_x[counter]][temp_pos_y[counter]].is_snake=0;
		refresh(temp_pos_x[counter],temp_pos_y[counter]);
	}  //����ɶ�ҩ 
	maps[x][y].is_food=1;
	maps[x][y].is_snake=1;
	  //�����¶�ҩ 
	temp_pos_x[counter]=x;
	temp_pos_y[counter]=y;
	counter=(counter+1)%POISON;
	refresh(x,y);
}

void refresh(){
	for(unsigned char x=0;x<SIZE_max;x++)
	for(unsigned char y=0;y<SIZE_max;y++)
	refresh(x,y);
	gotoxy(0,SIZE_max);
	color(16*15+0);
	cout<<"�ߵĳ���:"<<snake.length;
}

void refresh(unsigned char x,unsigned char y){
	gotoxy(2*x,y);
	if(maps[x][y].is_snake&&maps[x][y].is_food)
	color(16*2+0);//��ɫ 
	else if(maps[x][y].is_snake)
	color(16*0+0);//��ɫ 
	else if(maps[x][y].is_food)
	color(16*4+0);//��ɫ 
	else
	color(16*15+0);//��ɫ 
	cout<<"  ";
}

int power(int x){
	if(x<1)
	return 1;
	int temp=1;
	do{
		temp=temp*2;
		x--;
	}while(x);
	return temp;
}

void color(int x){
	SetConsoleTextAttribute(handle,x);
}

void gotoxy(int x,int y){
	coord.X=x;
	coord.Y=y;
	SetConsoleCursorPosition(handle,coord);
}

bool wait(){
	unsigned char temp;
	temp=getch();
	if(temp==32)
	return 0;
	else if(temp==27)
	exit(0);
	else
	return 1;
} 
