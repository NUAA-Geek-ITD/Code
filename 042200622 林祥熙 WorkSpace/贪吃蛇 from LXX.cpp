#include<bits/stdc++.h>
#include<windows.h>
#include<conio.h>
using namespace std;

#define SIZE_max 35  //地图大小(与程序第一行代码关联修改)
#define FWT 10  //添加食物的周期 
#define PWT 15  //毒药坐标变化周期
#define FOOD 8  //最大食物数
#define POISON 7  //最大毒药数
#define DI 5  //毒药刷新距离头的最小距离 
#define RT 100  //刷新间隔(ms)

typedef struct{
	bool is_snake;
	bool is_food;
	//is_snake用于标记是否为蛇、is_food用于标记是否为食物、二者同时为真则为毒药 
	unsigned char footmark;
	//标记蛇的足迹，在清除蛇尾时指向蛇头方向的上一位，用于更新蛇尾位置 
}MAP;

typedef struct{
	struct{
		unsigned char x;
		unsigned char y;
	}head;  //用于指示蛇头坐标 
	struct{
		unsigned char x;
		unsigned char y;
	}tail;  //用于指示蛇尾坐标 
	unsigned int length; //用于记录蛇长度 
}SNAKE; 

MAP maps[SIZE_max][SIZE_max];  //地图二维数组 
SNAKE snake;  //蛇的储存空间 
unsigned int foodrest;  //剩余的食物
unsigned int time_mark;  //记录时间信息

HANDLE handle;
COORD coord;

void welcome();  //欢迎界面 
bool finish();  //结算界面 
inline bool next_step(unsigned int dir);  //状态刷新 
void add_food();   //食物添加 
void refresh_poison();  //毒药刷新 
void refresh();  //全屏刷新图像 
inline void refresh(unsigned char x,unsigned char y);  //定点刷新图像 

inline int power(int x);  //得到2的X次方(对于正数次方) 
inline void color(int x);  //设置颜色
inline void gotoxy(int x,int y);  //移动光标位置 
inline bool wait();  //暂停时等待 

int main(){
	system("mode con cols=70 lines=36");
	system("title 贪吃蛇_SNAKE");
	system("color F0");
	handle=GetStdHandle(STD_OUTPUT_HANDLE);
	//界面设置和句柄获取 
	welcome();  //欢迎界面  
	unsigned int temp_dir,counter_food,counter_poison,counter,counter_poison_refresh;
	  /*分别表示
	    蛇的运动方向(0123分别指上下左右)
		离上次添加食物轮数
		离上次添加毒药轮数
		离开始游戏轮数
		游戏难度增加次数(加快毒药刷新) 
	  */ 
	beginning:  //当在finish中按空格键(实际是wait)，返回1到next_step，再返回1到主函数，最后跳转倒此处。 
		temp_dir=3;
		counter_food=0;
		counter_poison=0;
		counter=0;
		counter_poison_refresh=0;
	    foodrest=0;  //计数器初始化
	
	//初始化游戏数据 
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
	
	refresh();  //对全屏刷新
	while(1){
		if(_kbhit())
		switch(getch()){
			case 32 :  //用于暂停游戏 
				time_mark=(unsigned)time(NULL)-time_mark;//记录已用时间 
				do{
					Sleep(200);
				}while(wait());
				time_mark=(unsigned)time(NULL)-time_mark;//得到等效的时间戳
				break; 
			case 27 : return 0; //用于退出程序 
			case 13 : refresh();break;//强制刷新屏幕
			case 224:  //当检测到方向键 
			    switch(getch()){
			    	case 72 : temp_dir=0;break;
			    	case 80 : temp_dir=1;break;
			    	case 75 : temp_dir=2;break;
			    	case 77 : temp_dir=3;
				} 
		}
		if(next_step(temp_dir))  //刷新地图信息 
		goto beginning;  //从next_step()接收1表示重新开始，跳转到开头 
		counter_food++;
		counter_poison++;  
		counter++;  //计数器自增 
		if((counter_food>=FWT*power(foodrest))&&(foodrest<FOOD)){
			counter_food=0;
			add_food();
		}  //添加食物并且清零计数器
		if(counter_poison>=PWT-counter_poison_refresh){
			counter_poison=0;
			refresh_poison();
		}   //刷新毒药位置并且清零计数器 
		if((counter>(15000/RT)*power(counter_poison_refresh))&&counter_poison_refresh<PWT-2) 
		counter_poison_refresh++;  //更新难度
		gotoxy(0,SIZE_max); //光标移到左下角 
		Sleep(RT); //延时 
	}
}

void welcome(){
	//欢迎界面 
	gotoxy(SIZE_max-5,SIZE_max/2);
	cout<<"贪  吃  蛇";
	gotoxy(SIZE_max-4,SIZE_max/2+2);
	cout<<"S N A K E";
	gotoxy(0,SIZE_max);
	
	if(getch()==27)
	exit(0);
}

bool finish(){
	//结算界面 
    color(16*15+0);	
	system("cls");
	gotoxy(0,0);
	cout<<"贪  吃  蛇";
	gotoxy(1,2);
	cout<<"S N A K E";
	gotoxy(SIZE_max-11,SIZE_max/2);
	cout<<"游    戏    结    束";
	gotoxy(SIZE_max-7,SIZE_max/2+2);
	cout<<"蛇的长度:"<<snake.length<<"节";
	gotoxy(SIZE_max-5,SIZE_max/2+4);
	cout<<"用时:"<<((unsigned)time(NULL)-time_mark)<<"秒"; 
	gotoxy(0,SIZE_max);
	
	while(wait()){
		Sleep(500);
	}
	return 1;
}

bool next_step(unsigned int dir){
	/*方向验证*/ 
	if(maps[snake.head.x][snake.head.y].footmark+dir==1||maps[snake.head.x][snake.head.y].footmark+dir==5) 
	dir=maps[snake.head.x][snake.head.y].footmark;
	  //当且仅当二者之和为1或5时，二者指示相反方向，dir无效，将footmark赋值给dir 
	else
	maps[snake.head.x][snake.head.y].footmark=dir;
	  //为地图更新方向数据 
	  
	/*蛇头移动*/ 
	switch(dir){
		case 0 : snake.head.y=(snake.head.y-1+SIZE_max)%SIZE_max;break;
		case 1 : snake.head.y=(snake.head.y+1)%SIZE_max;break;
		case 2 : snake.head.x=(snake.head.x-1+SIZE_max)%SIZE_max;break;
		case 3 : snake.head.x=(snake.head.x+1)%SIZE_max;
	}  //根据方向dir移动蛇头 
	maps[snake.head.x][snake.head.y].footmark=dir;
	  //为蛇头位置地图写入方向(为下次移动的d方向验证准备)
	  
	/*触碰验证*/ 
	if(maps[snake.head.x][snake.head.y].is_snake)
	if(finish()) //碰到蛇或毒药，进入结算 
	return 1;  //为真则重新开始游戏(回到主函数后将跳转到变量初始化前)
	maps[snake.head.x][snake.head.y].is_snake=1;  //未触碰，将该位置设置为蛇(添加蛇头位置地图指示) 
	if(maps[snake.head.x][snake.head.y].is_food){ 
		maps[snake.head.x][snake.head.y].is_food=0;
		snake.length++;
		foodrest--;
		gotoxy(0,SIZE_max);
		color(16*15+0);
		cout<<"蛇的长度:"<<snake.length;  //在左下角刷新蛇长度指示 
		refresh(snake.head.x,snake.head.y);  //刷新蛇头，使其在屏幕上显示
		return 0;  //碰到食物，不进行蛇尾移动，直接退出函数 
	}
	
	/*蛇尾移动*/ 
	else
	refresh(snake.head.x,snake.head.y);  //刷新蛇头，使其在屏幕上显示 
	maps[snake.tail.x][snake.tail.y].is_snake=0;  //删除蛇尾位置地图指示 
	refresh(snake.tail.x,snake.tail.y);  //刷新蛇尾 
	switch(maps[snake.tail.x][snake.tail.y].footmark){
		case 0 : snake.tail.y=(snake.tail.y-1+SIZE_max)%SIZE_max;break;
		case 1 : snake.tail.y=(snake.tail.y+1)%SIZE_max;break;
		case 2 : snake.tail.x=(snake.tail.x-1+SIZE_max)%SIZE_max;break;
		case 3 : snake.tail.x=(snake.tail.x+1)%SIZE_max;
	}  //根据方向移动蛇尾 
	return 0;
}
void add_food(){
	unsigned int x,y,temp=0;
	while(1){
		if(temp>999)
		return;  //防止长时间找不到空位导致画面卡顿 
		temp++;
		x=rand()%SIZE_max;
		y=rand()%SIZE_max;
		if(maps[x][y].is_snake||maps[x][y].is_food)
		continue;
		break;
	}  //寻找空位置 
	maps[x][y].is_food=1;
	foodrest++;
	refresh(x,y);
} 
void refresh_poison(){
	static unsigned int counter=0;
	static unsigned int temp_pos_x[POISON]={0},temp_pos_y[POISON]={0};
	  //用于储存毒药位置信息及刷新顺序 
	unsigned int x,y,temp=0;
	while(1){
		if(temp>999)
		return;  //防止长时间找不到空位导致画面卡顿 
		temp++;
		x=rand()%SIZE_max;
		y=rand()%SIZE_max;
		if(maps[x][y].is_snake||maps[x][y].is_food)
		continue;//进入下个循环重新寻找 
		if(x-snake.head.x>DI||snake.head.x-x>DI||y-snake.head.y>DI||snake.head.y-y>DI)//附近无蛇头 
		break;
	}  //寻找空旷位置(离蛇头较远) 
	if(maps[temp_pos_x[counter]][temp_pos_y[counter]].is_food&&maps[temp_pos_x[counter]][temp_pos_y[counter]].is_snake){
		maps[temp_pos_x[counter]][temp_pos_y[counter]].is_food=0;
		maps[temp_pos_x[counter]][temp_pos_y[counter]].is_snake=0;
		refresh(temp_pos_x[counter],temp_pos_y[counter]);
	}  //清除旧毒药 
	maps[x][y].is_food=1;
	maps[x][y].is_snake=1;
	  //创建新毒药 
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
	cout<<"蛇的长度:"<<snake.length;
}

void refresh(unsigned char x,unsigned char y){
	gotoxy(2*x,y);
	if(maps[x][y].is_snake&&maps[x][y].is_food)
	color(16*2+0);//绿色 
	else if(maps[x][y].is_snake)
	color(16*0+0);//黑色 
	else if(maps[x][y].is_food)
	color(16*4+0);//红色 
	else
	color(16*15+0);//白色 
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
