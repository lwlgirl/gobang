#include <graphics.h>
#include<cstdio>
#include<cstring>
#include<iostream>
#include<algorithm>
#include<conio.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include<windows.h>
#include<mmsystem.h>
#include<stack>
#include<queue>
#pragma comment(lib,"WINMM.LIB")

#define SIZE 15//棋盘大小
#define Win 6      //连五
#define flex4 5    //活四
#define block4 4   //冲四
#define flex3 3    //活三
#define block3 2   //眠三
#define flex2 1    //活二
#define Ntype 7    //棋型数
using namespace std;

//1代表黑棋2代表白棋0代表空格

//C电脑颜色，P人颜色
int C,P,step,Depth,Bestval,LEN;//LEN为用户个数
int mp[SIZE][SIZE];//棋盘
int now_user,INDEX,my_color;//当前用户的id

struct node
{
    int x;
    int y;
};

struct user
{
    char name[100];
    int win;
    int total;
    double score;
} user[101];


//d临时变量，Bestmove最好的位置
node d,Bestmove,Chess[100],Score[SIZE][SIZE];

stack<node>st;

int face();
void playmusic();
void stopmusic();
void loading();
int color();
void game_face();
int Double();
int is_win(int x,int y,int color);
int win();
void fail();
int pc();
int play();
int max_min(int,int,int );
int evaluate();
int priority();
void GetType(int x,int y,int role,int *d);
int other();
int regist();
int login();
void position(int x,int y,int *nx,int *ny);
void display(int x,int y,int C);
void init();
int Rank();
int win(int color);


int start_winsock()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD( 1, 1 );
    err = WSAStartup(wVersionRequested, &wsaData );
    if ( err != 0 )
    {
        return -1;
    }
    if (LOBYTE( wsaData.wVersion ) != 1
            || HIBYTE(wsaData.wVersion) != 1 )
    {
        WSACleanup( );
        return -1;
    }
}


//连接
void wait_conn(SOCKET *sockser, SOCKET *sockcli)
{
    SOCKADDR_IN  addrClient;
    int len = sizeof(SOCKADDR);

    printf("正在等待客户连接！！\n");
    *sockcli = accept(*sockser, (SOCKADDR*)&addrClient, &len);
    game_face();
}


void set_socket(SOCKET *sock)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    //设置通信地址
    SOCKADDR_IN addrSrv;

    // 将INADDR_ANY转换为网络字节序，调用 htonl(long型)或htons(整型)
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);
    //把socket与地址绑定在一起
    bind(*sock, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    listen(*sock, 10);
}

//获取当前鼠标位置的坐标
void point(char a[],int *nx,int *ny)
{
    int x,y;

    mouse_msg msg = {0};
    PIMAGE img;

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();
            x=msg.x;
            y=msg.y;
            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            if((int)msg.is_left()&&msg.is_up()&&x>=59&&x<=517&&y>=53&&y<=533)
            {
                x-=70;
                y-=63;
                if(x%32>=16)
                {
                    *nx=x/32+1;
                    x=((x/32)+1)*32;
                }
                else
                {
                    *nx=x/32;
                    x=(x/32)*32;
                }
                if(y%32>=16)
                {
                    *ny=y/32+1;
                    y=((y/32)+1)*32;
                }
                else
                {
                    *ny=y/32;
                    y=(y/32)*32;
                }

                if(mp[*nx][*ny]!=0)
                    continue;
                mp[*nx][*ny]=1;
                a[0]=*nx/10+'0';
                a[1]=*nx%10+'0';
                a[2]=*ny/10+'0';
                a[3]=*ny%10+'0';
                return;
            }
        }

    }
}

//打印棋子
void printmap(int mmp[][SIZE] )
{

    game_face();
    for(int i=0; i<SIZE; i++)
    {
        for(int j=0; j<SIZE; j++)
        {
            if(mmp[i][j]==1)
            {
                PIMAGE img=newimage();
                getimage(img,"img\\black.png");

                putimage_transparent(NULL,img,i*32+50,j*32+55,0);
            }
            else if (mmp[i][j]==2)
            {
                PIMAGE img=newimage();
                getimage(img,"img\\white.png");
                putimage_transparent(NULL,img,i*32+50,j*32+55,0);
            }
        }
    }
}

//联机模式的接收和发送
int Kobe()
{
    if(now_user!=-1)
    {
        user[now_user].total++;
    }
    SOCKET sockSrv, sockConn;
    char sendBuf[100], recvBuf[100];
    int ret;
    int P;
    int x,y;

    PIMAGE img;

    if(start_winsock() == -1)
        return 0;
    set_socket(&sockSrv);
    wait_conn(&sockSrv,&sockConn);

    if(now_user!=-1)
    {
        user[now_user].total++;
    }

    while(1)
    {
        point(sendBuf,&x,&y);

        printf("x=%d y=%d\n",x,y);

        printmap(mp);


        send(sockConn, sendBuf, strlen(sendBuf)+1, 0);
        int t=is_win(x,y,1);

        if(t==1)
        {
            if(now_user!=-1)
            {
                user[now_user].win++;
            }
            int k=win(1);
            INDEX=0;
            return 0;
        }

        //printf("%d %d\n",x,y);

        Sleep(1000);
        ret = recv(sockConn, recvBuf, 100, 0);
        if(ret>0)
        {
            x= (recvBuf[0]-'0')*10+recvBuf[1]-'0';
            y= (recvBuf[2]-'0')*10+recvBuf[3]-'0';
            mp[x][y]=2;

            printmap(mp);

            t=is_win(x,y,2);
            if(t==1)
            {
                win(2);
                INDEX=0;
                return 0;
            }

        }
    }
}

//播放音乐
void playmusic()
{
    mciSendString(TEXT("open 1.mp3 alias back"),NULL,0,NULL);//播放的音乐放在该文件的目录下
    mciSendString(TEXT("Play back repeat"),NULL,0,NULL);
}

//关闭音乐
void stopmusic()
{
    mciSendString(TEXT("stop back"),NULL,0,NULL);
    //mciSendString(TEXT("close back"),NULL,0,NULL);
}

//游戏加载界面
void loading()
{

    setcolor(EGERGB(255,255,255)); // 蓝色
    rectangle(90,280,418,310); // 空心矩形
    rectangle(88,278,420,312);

    char s[100]="";
    sprintf(s,"loading   to");
    outtextxy(180,330,s);

    for(int i=3,j=0,k=0; i<32; i++,j++,k++)
    {
        setfillcolor(EGERGB(0,132,255));// 蓝色  填充颜色
        bar(95+j*11,285,105+j*11,305); // 实心无边框矩形
        sprintf(s,"%d%%",3*i+7);
        outtextxy(270,330,s);
        Sleep(40);
    }
    for(int i=0; i<3; i++)
    {
        sprintf(s," . ");
        outtextxy(310+10*i,330,s);
        Sleep(200);
    }
    cleardevice();//清屏函数
}

//选择谁先下子
int priority()
{
    PIMAGE img;

    img=newimage();
    getimage(img,"img\\pri.png");
    putimage(170,200,img);
    //delimg(img);
    mouse_msg msg = {0};

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();

            int x=msg.x;
            int y=msg.y;

            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            if((int)msg.is_left()&&msg.is_up())
            {
                if(x>=234&&x<=322&&y>=239&&y<=263)
                {
                    //我先手
                    return 1;

                }
                else if(x>=235&&x<=324&&y>=289&&y<=314)
                {
                    return 2;
                    //我后手
                }
            }
        }
    }
    return 0;
}

//选择棋子颜色
int color()
{
    PIMAGE img;
    setbkcolor(EGERGB(218,238,243));
    cleardevice();
    img=newimage();
    getimage(img,"img\\color.png");
    putimage(0,0,img);

    mouse_msg msg = {0};

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();

            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            int x,y;

            if((int)msg.is_left()&&msg.is_up())
            {
                x=msg.x;
                y=msg.y;

            }

            //黑色
            if(x>=263&&x<=336&&y>=279&&y<=317)
                return 1;

            //白色
            else if(x>=257&&x<=330&&y>=442&&y<=472)
                return 2;
        }

    }

}

//显示棋盘
void game_face()
{
    PIMAGE img1,img2,img;

    //cleardevice();

    //mouse_msg msg = {0};

    img1=newimage();
    getimage(img1,"img\\qipan.png");
    putimage(30,25,img1);

    img2=newimage();
    getimage(img2,"img\\1.png");
    putimage(100,585,img2);

}

//赢
int win(int color)
{

    PIMAGE img;
    img=newimage();
    if(color==1)
    {
        getimage(img,"img\\black_win.png");
        putimage_transparent(NULL,img,85,150,0);
    }
    else if(color==2)
    {
        getimage(img,"img\\white_win.png");
        putimage_transparent(NULL,img,85,150,0);
    }

    mouse_msg msg = {0};

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();

            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            int x=msg.x;
            int y=msg.y;
            //重玩
            if((int)msg.is_left()&&msg.is_up()&&x>=169&&x<=241&&y>=310&&y<=342)
            {
                init();

                return 1;
            }
            //退出到主菜单
            else if((int)msg.is_left()&&msg.is_up()&&x>=337&&x<=411&&y>=308&&y<=344)
            {
                init();
                // cleardevice();
                return 2;
            }
        }
    }
}

//判断胜负
int is_win(int x,int y,int color)
{
    //判断竖向
    for(int i=x-4; i<=x; i++)
    {
        if(i>=0&&i+4<SIZE&&mp[i][y]==color&&mp[i+1][y]==color&&mp[i+2][y]==color&&mp[i+3][y]==color&&mp[i+4][y]==color)
        {
            return 1;
        }
    }

    //判断横向
    for(int j=y-4; j<=y; j++)
    {
        if(j>=0&&j+4<SIZE&&mp[x][j]==color&&mp[x][j+1]==color&&mp[x][j+2]==color&&mp[x][j+3]==color&&mp[x][j+4]==color)
        {
            return 1;
        }
    }

    //判断右斜
    for(int i=x-4,j=y-4; i<=x; i++,j++)
    {
        if(i>=0&&j>=0&&i+4<SIZE&&j+4<SIZE&&mp[i][j]==color&&mp[i+1][j+1]==color&&mp[i+2][j+2]==color&&mp[i+3][j+3]==color&&mp[i+4][j+4]==color)
        {
            return 1;
        }

    }

    //判断左斜
    for(int i=x-4,j=y+4; i<=x; i++,j--)
    {
        if(i>=0&&j>=0&&i+4<SIZE&&j+4<SIZE&&mp[i][j]==color&&mp[i+1][j-1]==color&&mp[i+2][j-2]==color&&mp[i+3][j-3]==color&&mp[i+4][j-4]==color)
        {
            return 1;
        }
    }

    return 0;
}

void init()
{
    while(!st.empty())
        st.pop();

    for(int i=0; i<SIZE; i++)
    {
        for(int j=0; j<SIZE; j++)
        {
            mp[i][j]=0;
        }

    }
    mp[0][0]=3;
}


//落棋
int play(int x,int y,int f)
{
    int nx,ny;
    PIMAGE img;

    if(x>=59&&x<=517&&y>=53&&y<=533)
    {
        position(x,y,&nx,&ny);

        printf("\nx=%d y=%d\n",nx,ny);

        if(mp[nx][ny]!=0)
            return 0;

        mp[nx][ny]=f;

        d.x=nx,d.y=ny;

        st.push(d);

        display(nx,ny,f);

        int d=is_win(nx,ny,f);

        printf("win=%d \n",d);
        if(d==1)
        {
            if(now_user!=-1&&my_color==f)
            {
                user[now_user].win++;
            }

            int d=win(f);
            cleardevice();
            return d;
        }
    }
    //悔棋
    if(x>=134&&x<=206&&y>=604&&y<=634&&st.size()>0)
    {
        if(INDEX==1&&st.size()>1)
        {
            d=st.top();
            st.pop();
            mp[d.x][d.y]=0;
        }

        d=st.top();
        st.pop();
        mp[d.x][d.y]=0;

        /*for(int i=0; i<SIZE; i++)
        {
            for(int j=0; j<SIZE; j++)
            {
                printf("%d ",mp[i][j]);
            }
            printf("\n");
        }*/

        cleardevice();
        game_face();

        for(int i=0; i<SIZE; i++)
        {
            for(int j=0; j<SIZE; j++)
            {
                if(mp[i][j]==0)
                    continue;

                if(mp[i][j]==1)
                {
                    int x=i*32;
                    int y=j*32;
                    img=newimage();
                    getimage(img,"img\\black.png");
                    putimage_transparent(NULL,img,y+50,x+55,0);
                }
                else if(mp[i][j]==2)
                {
                    int x=i*32;
                    int y=j*32;
                    img=newimage();
                    getimage(img,"img\\white.png");
                    putimage_transparent(NULL,img,y+50,x+55,0);
                }
            }
        }

        return 0;

    }

    //重玩
    else if(x>=270&&x<=341&&y>=602&&y<=634)
    {
        game_face();
        init();
        return 3;
    }
    //返回
    else if(x>=388&&x<=455&&y>=601&&y<=633)
    {
        init();
        return 4;
    }

    return 5;

}

//求坐标
void position(int x,int y,int *nx,int *ny)
{

    x-=70;
    y-=63;

    if(x%32>=16)
    {
        *nx=x/32+1;
        x=((x/32)+1)*32;

    }
    else if(x%32<16)
    {
        *nx=x/32;
        x=(x/32)*32;
    }
    if(y%32>=16)
    {
        *ny=y/32+1;
        y=((y/32)+1)*32;
    }
    else if(y%32<16)
    {
        *ny=y/32;
        y=(y/32)*32;
    }

    int t=*nx;
    *nx=*ny;
    *ny=t;

}

//单机双人模式
int Double()
{
    init();

    PIMAGE img;
    P=C=0;
    int f=color();//1为黑色，2为白色

    if(now_user!=-1)
    {
        my_color=f;
        user[now_user].total++;
    }

    game_face();

    int p=priority();
    cleardevice();
    game_face();

    if(p==2)
    {
        if(f==1)
        {
            f=2;
        }
        else
            f=1;
    }

    //x y为鼠标位置，nx ny为棋盘坐标
    int x,y;

    mouse_msg msg = {0};

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();

            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            if((int)msg.is_left()&&msg.is_up())
            {

                x=msg.x;
                y=msg.y;

                int k=play(x,y,f);

                if(k==0)
                    continue;

                if(f==1)
                    f=2;
                else if(f==2)
                    f=1;

                //重玩
                if(k==1||k==3)
                {
                    return 0;
                }
                //返回
                else if(k==2||k==4)
                {
                    INDEX=0;
                    return 0;
                }

            }
        }

    }
    return 0;
}


int checkcolor(int step)
{
    if(step%2)
        return 1;
    return 2;
}

//判断对方杀棋，返回分值
int GetKill()
{
    int i,j,x,y;

    //得到四种方向上的棋型
    int d[4];

    int type[Ntype]= {0};

    //遍历玩家所走的那些层
    for(i=P; i<=step; i+=2)
    {
        //Chess保存了每一层下了的棋子
        x=Chess[i].x;
        y=Chess[i].y;
        GetType(x, y, P, d);
        for(j=0; j<4; j++)
        {
            //求出各种棋型的数量
            type[d[j]]++;
        }
    }

    //各种棋型所需要的棋子数量
    int num[7]= {0,2,3,3,4,4,5};

    for(i=1; i<Ntype; i++)
    {
        type[i]/=num[i];
    }

    if(type[flex4]>0)
        return 2000;
    else if(type[block4]>1)
        return 2000;
    else if(type[block4]==1&&type[flex3]>0)
        return 1600;
    else if(type[flex3]>1)
        return 200;
    else
        return 0;

}
//局面评估
int evaluate()
{
    int i,j;
    int computer_score=0;
    int people_score=0;
    for(i=0; i<SIZE; i++)
    {
        for(j=0; j<SIZE; j++)
        {
            if(mp[i][j]==0)
            {
                if(Score[i][j].x>computer_score)
                    computer_score=Score[i][j].x;
                if(Score[i][j].y>people_score)
                    people_score=Score[i][j].y;
            }
        }
    }

    people_score=people_score+GetKill();

    return computer_score-people_score;

}

//判断连五
int Check()
{
    int x=Chess[step].x;
    int y=Chess[step].y;
    int role=mp[x][y];
    if(is_win(x, y, role)==1)
    {
        if(role==C)
            return 10000;
        else
            return -10000;
    }
    else
        return 0;
}

//给空格打分
node space_score(int x, int y)
{
    int d[4];//保存四个方向上的可用棋子数量
    int computer_score=0,people_score=0;
    int Cval[Ntype]= {0,2,2,10,10,50,10000};//电脑各种棋型的分值
    int Hval[Ntype]= {0,2,2,10,10,10,10};//玩家
    int computer_type[7]= {0};//保存电脑方各种棋型的数量
    int people_type[7]= {0};//保存玩家各种棋型的数量
    node r;

    //得到四个方向上的棋型
    GetType(x, y, C, d);

    for(int i=0; i<4; i++)
    {
        computer_type[d[i]]++;
    }

    for(int i=0; i<Ntype; i++)
    {
        computer_score+=computer_type[i]*Cval[i];
    }

    GetType(x, y, P, d);

    for(int i=0; i<4; i++)
    {
        people_type[d[i]]++;
    }
    for(int i=0; i<Ntype; i++)
    {
        people_score+=people_type[i]*Hval[i];
    }

    if(computer_type[flex4]>0)
        computer_score+=1000;
    else if(computer_type[block4]>1)
        computer_score+=1000;
    else if(computer_type[block4]==1&&computer_type[flex3]>0)
        computer_score+=800;

    if(people_type[Win]>0)//冲四
        people_score+=50;
    else if(people_type[flex4]>0)//有活三
        people_score+=50;
    else if(people_type[block4]>1)
        people_score+=50;
    else if(people_type[block4]==1&&people_type[flex3]>0)//冲四活三
        people_score+=40;
    else if(people_type[flex3]>1)//双活三
        people_score+=20;

    r.x=computer_score;
    r.y=people_score;
    return r;
}

//判断有多少颗棋子
//len总长度、len2棋子加空格长度、block障碍物个数、count可用棋子数
//返回是哪种棋型
int check_type(int len, int len2, int block, int count)
{
    if(len>=5&&count>1)
    {
        if(count==5)
            return Win;//长连

        //两边无障碍物
        if(len>5&&len2<5&&block==0)
        {
            switch(count)
            {
            case 4:
                return flex4;//活四
            case 3:
                return flex3;//活三
            case 2:
                return flex2;//活二
            }
        }
        else
        {
            switch(count)
            {
            case 4:
                return block4;//眠四
            case 3:
                return block3;//冲三
            }
        }
    }
    return 0;
}

//查询该角色在四个方向棋型
void GetType(int x,int y,int role,int *d)
{
    int a,b;
    int space=0;//空格
    int block=0;//障碍
    int len=1;//总长度数
    int len2=1;//count+space
    int count=1;//可用棋子数

    //搜索下面四个
    for(a=x+1; a<=x+4; a++)
    {
        //特判
        if(a>14)
        {
            if(len2==space+count)
                block++;//边缘也算障碍
            break;
        }
        if(mp[a][y]==role)
        {
            //空格大于2说明没戏，space+count>4说明搜索完毕
            if(space>2||space+count>4)
                break;

            count++;
            len++;
            len2=space+count;
        }
        else if(mp[a][y]==0)
        {
            len++;
            space++;
        }
        //遇到对方棋子就结束搜索
        else
        {
            //障碍物
            if(len2==space+count)
                block++;
            break;
        }
    }

    space=len2-count;

    //搜索上4个
    for(a=x-1; a>=x-4; a--)
    {
        if(a<0)
        {
            if(len2==space+count)
                block++;
            break;
        }
        if(mp[a][y]==role)
        {
            if(space>2||space+count>4)
                break;
            count++;
            len++;
            len2=space+count;
        }
        else if(mp[a][y]==0)
        {
            len++;
            space++;
        }
        else
        {
            if(len2==space+count)
                block++;
            break;
        }
    }

    d[0]=check_type(len,len2,block,count);

    len=1;
    len2=1;
    count=1;
    block=0;
    space=0;

    //右边4个
    for(b=y+1; b<=y+4; b++)
    {
        if(b>14)
        {
            if(len2==space+count)
                block++;
            break;
        }
        if(mp[x][b]==role)
        {
            if(space>2||space+count>4)
                break;
            count++;
            len++;
            len2=space+count;
        }
        else if(mp[x][b]==0)
        {
            len++;
            space++;
        }
        else
        {
            if(len2==space+count)
                block++;
            break;
        }
    }
    space=len2-count;
    //左边
    for(b=y-1; b>=y-4; b--)
    {
        if(b<0)
        {
            if(len2==space+count)
                block++;
            break;
        }
        if(mp[x][b]==role)
        {
            if(space>2||space+count>4)
                break;
            count++;
            len++;
            len2=space+count;
        }
        else if(mp[x][b]==0)
        {
            len++;
            space++;
        }
        else
        {
            if(len2==space+count)
                block++;
            break;
        }
    }

    d[1]=check_type(len,len2,block,count);

    len=1;
    len2=1;
    count=1;
    block=0;
    space=0;

    //右下
    for(a=x+1,b=y+1; a<=x+4; a++,b++)
    {
        if(a>14||b>14)
        {
            if(len2==space+count)
                block++;
            break;
        }
        if(mp[a][b]==role)
        {
            if(space>2||space+count>4)
                break;
            count++;
            len++;
            len2=space+count;
        }
        else if(mp[a][b]==0)
        {
            len++;
            space++;
        }
        else
        {
            if(len2==space+count)
                block++;
            break;
        }
    }
    space=len2-count;

    //左上
    for(a=x-1,b=y-1; a>=x-4; a--,b--)
    {
        if(a<0||b<0)
        {
            if(len2==space+count)
                block++;
            break;
        }
        if(mp[a][b]==role)
        {
            if(space>2||space+count>4)
                break;
            count++;
            len++;
            len2=space+count;
        }
        else if(mp[a][b]==0)
        {
            len++;
            space++;
        }
        else
        {
            if(len2==space+count)
                block++;
            break;
        }
    }
    d[2]=check_type(len,len2,block,count);

    len=1;
    len2=1;
    count=1;
    block=0;
    space=0;

    //右上
    for(a=x+1,b=y-1; a<=x+4; a++,b--)
    {
        if(a>14||b<0)
        {
            if(len2==space+count)
                block++;
            break;
        }
        if(mp[a][b]==role)
        {
            if(space>2||space+count>4)
                break;
            count++;
            len++;
            len2=space+count;
        }
        else if(mp[a][b]==0)
        {
            len++;
            space++;
        }
        else
        {
            if(len2==space+count)
                block++;
            break;
        }
    }
    space=len2-count;

    //左下
    for(a=x-1,b=y+1; a>=x-4; a--,b++)
    {
        if(a<0||b>14)
        {
            if(len2==space+count)
                block++;
            break;
        }
        if(mp[a][b]==role)
        {
            if(space>2||space+count>4)
                break;
            count++;
            len++;
            len2=space+count;
        }
        else if(mp[a][b]==0)
        {
            len++;
            space++;
        }
        else
        {
            if(len2==space+count)
                block++;
            break;
        }
    }
    d[3]=check_type(len,len2,block,count);
}

//更新棋子周围空格分值
void get_space_score(node p)
{
    int x=p.x;
    int y=p.y;
    int a,b;
    //竖
    for(a=x-4; a<=x+4; a++)
    {
        if(a<0)
            continue;
        if(a>14)
            break;
        if(mp[a][y]==0)
            Score[a][y]=space_score(a, y);
    }
    //横
    for(b=y-4; b<=y+4; b++)
    {
        if(b<0)
            continue;
        if(b>14)
            break;
        if(mp[x][b]==0)
            Score[x][b]=space_score(x, b);
    }
    //右斜
    for(a=x-4,b=y-4; a<=x+4; a++,b++)
    {
        if(a<0||b<0)
            continue;
        if(a>14||b>14)
            break;
        if(mp[a][b]==0)
            Score[a][b]=space_score(a, b);
    }

    //左斜
    for(a=x-4,b=y+4; a<=x+4; a++,b--)
    {
        if(b>14||a<0)
            continue;
        if(a>14||b<0)
            break;
        if(mp[a][b]==0)
            Score[a][b]=space_score(a, b);
    }
}

//下子，不打印
void Make(node next)
{
    int x=next.x;
    int y=next.y;
    step++;
    mp[x][y]=checkcolor(step);
    Chess[step].x=x;
    Chess[step].y=y;

    //更新空格的分值
    get_space_score(next);
}

//删子
void Del()
{
    int x=Chess[step].x;
    int y=Chess[step].y;
    mp[x][y]=0;
    get_space_score(Chess[step]);
    step--;
}

//检查周围n格内有无棋子
int Round(int x, int y)
{
    for(int i=(x-1>=0)?x-1:0; i<=x+1&&i<SIZE; i++)
    {
        for(int j=(y-1>=0)?y-1:0; j<=y+1&&j<SIZE; j++)
        {
            if(mp[i][j]!=0)
                return 1;
        }
    }
    return 0;
}

//检查周围四个方向有无该颜色棋子
int CheckRound(int x, int y, int role)
{
    for(int i=-1; i<2; i++)
    {
        for(int j=-1; j<2; j++)
        {
            if(role==mp[x+i][y+j])
                return 1;
        }
    }
    return 0;
}

//检查空格是否可下,优先判断对手是否有活3以上的棋型
int Checkspace(node p, int role)
{
    int x=p.x;
    int y=p.y;
    int d[4];//保存四种方向的棋型

    //得到玩家棋型
    GetType(x, y, role, d);

    for(int i=0; i<4; i++)
    {
        if(d[i]>=flex2)//活2
            return 1;
    }

    //电脑方四种方向的棋型
    GetType(x, y, 3-role, d);

    for(int i=0; i<4; i++)
    {
        if(d[i]>=flex3)//活3
            return 1;
    }
    return 0;
}

//得到所有可以走的坐标
int Getspace(int depth,node s[10][100])
{
    int t=0;
    int i,j;
    node p;

    int role=checkcolor(step+1);//对方的颜色

    for(i=0; i<SIZE; i++)
    {
        for(j=0; j<SIZE; j++)
        {
            p.x=i;
            p.y=j;
            if(mp[i][j]==0&&Round(i, j))
            {
                if(Checkspace(p, role)||step==1)
                {
                    t++;
                    s[depth][t].x=i;
                    s[depth][t].y=j;
                }
            }
        }
    }

    return t;
}

//极大极小搜索算法
int max_min(int depth,int alpha,int beta)
{
    int len;
    int is_win=Check();

    node s[10][100];//保存所有可下的坐标

    if(is_win)
    {
        if(depth%2==0)
            return is_win;
        else
            return -is_win;
    }

    if(depth==0)
        return evaluate();

    else
        len=Getspace(depth,s);

    while(len)
    {
        Make(s[depth][len]);//下棋
        int val=-max_min(depth-1,-beta,-alpha);//递归
        Del();//回溯

        //剪枝
        //max层
        if(val>=beta)
        {
            if(depth==Depth)
                Bestmove=s[depth][len];
            return val;
        }
        //min层
        if(val>alpha)
        {
            alpha=val;
            if(depth==Depth)
                Bestmove=s[depth][len];
        }
        len--;
    }
    return alpha;
}

//显示棋子
void display(int x,int y,int C)
{
    PIMAGE img=newimage();

    if(C==1)
    {
        getimage(img,"img\\black.png");
        putimage_transparent(NULL,img,y*32+50,x*32+55,0);

        getimage(img,"img\\w.png");
        putimage_transparent(NULL,img,400,27,0);

    }
    else if(C==2)
    {
        getimage(img,"img\\white.png");
        putimage_transparent(NULL,img,y*32+50,x*32+55,0);

        getimage(img,"img\\b.png");
        putimage_transparent(NULL,img,400,27,0);

    }
}


//选择人机的简单困难模式
void easy(int *n)
{
    cleardevice();
    //setbkcolor(BLUE);
    PIMAGE img=newimage();
    getimage(img,"img\\easy.png");
    putimage_transparent(NULL,img,150,240,0);
    mouse_msg msg = {0};

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();

            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            if((int)msg.is_left()&&msg.is_up())
            {
                int x=msg.x;
                int y=msg.y;

                //简单模式
                if(x>=217&&x<=384&&y>=268&&y<=306)
                {
                    *n=2;
                    Depth=2;
                    return ;
                }
                //困难模式
                else if(x>=219&&x<=380&&y>=393&&y<=426)
                {
                    *n=2;
                    Depth=4;
                    return ;
                }

            }
        }
    }
}
//人机对战
int pc()
{
    init();

    int n;
    easy(&n);


    PIMAGE img;
    game_face();
    P=2;
    C=1;
    int step=0,nx,ny;

    cleardevice();
    game_face();


    if(now_user!=-1)
    {
        user[now_user].total++;
    }
    while(1)
    {
        //人下棋
        if(n!=checkcolor(step))
        {
            mouse_msg msg = {0};

            int flag=0;

            for ( ; is_run(); delay_fps(60))
            {
                while (mousemsg())
                {
                    msg = getmouse();

                    //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

                    if((int)msg.is_left()&&msg.is_up())
                    {
                        int x=msg.x;
                        int y=msg.y;

                        int t=play(x,y,P);

                        if(t!=0)
                        {
                            //重玩
                            if(t==1||t==3)
                            {
                                INDEX=1;
                                return 0;
                            }
                            //返回
                            else if(t==2||t==4)
                            {
                                INDEX=0;
                                return 0;
                            }
                        }

                        //已经有子或者悔棋了
                        else if(t==0)
                            continue;

                        flag=1;
                    }

                }

                if(flag==1)
                    break;
            }
        }

        //电脑下棋
        else
        {
            if(step==0)
            {
                mp[7][7]=C;
                d.x=7,d.y=7;
                st.push(d);
                display(7,7,C);

            }
            else
            {
                int x,y;

                //极大极小搜索算法，获取最佳坐标
                Bestval=max_min(Depth,-10001,10000);

                x=Bestmove.x;
                y=Bestmove.y;

                printf("x= %d %d",x,y);

                mp[x][y]= C;
                d.x=x,d.y=y;
                st.push(d);

                display(x,y,C);

                int t=is_win(x,y,C);

                if(t==1)
                {
                    int d=win(C);
                    //重玩
                    if(d==1)
                    {
                        return 0;
                    }
                    //返回
                    else if(d==2)
                    {
                        INDEX=0;
                        return 0;
                    }
                }
            }

        }
        printf("step=%d \n",step);
        step++;
    }
}

bool cmp(struct user a,struct user b)
{
    return a.score>b.score;
}
//排行榜
int Rank()
{
    cleardevice();
    for(int i=0; i<LEN; i++)
    {
        if(user[i].total>0)
            user[i].score=(user[i].win*1.0)/(user[i].total*1.0);
        else
            user[i].score=0;
    }

    //按分数排序
    sort(user,user+LEN,cmp);

    //printf("姓名  获胜  总局数 分数\n");

    setfont(35,0,"华文新魏");

    char s[100];

    setcolor(BLUE);

    outtextxy(100,100,"姓名");

    outtextxy(200,100,"获胜");

    outtextxy(300,100,"总局数");

    outtextxy(450,100,"分数");

    for(int i=0; i<LEN; i++)
    {
        //printf("%s  %d  %d  %lf\n",user[i].name,user[i].win,user[i].total,user[i].score);

        sprintf(s,"%s",user[i].name);
        outtextxy(100,i*100+200,s);
        sprintf(s,"%d",user[i].win);
        outtextxy(200,i*100+200,s);
        sprintf(s,"%d",user[i].total);
        outtextxy(300,i*100+200,s);
        sprintf(s,"%.2lf",user[i].score);
        outtextxy(450,i*100+200,s);
    }

    getch();
    INDEX=0;
    return 0;
}

//登录
int login()
{
    PIMAGE img;

    char nam[100];

    inputbox_getline("请输入", "账号（回车确认）", nam, 100);
    inputbox_getline("请输入", "账号（回车确认）", nam, 100);

    int j;
    for(j=0; j<LEN; j++)
    {
        if(strcmp(nam,user[j].name)==0)
        {
            cleardevice();
            img=newimage();
            getimage(img,"img\\success.png");
            putimage_transparent(NULL,img,170,220,0);
            getch();
            cleardevice();
            INDEX=0;
            now_user=j;
            return 0;

        }
    }
    if(j==LEN)
    {
        cleardevice();
        img=newimage();
        getimage(img,"img\\register.png");
        putimage_transparent(NULL,img,150,210,0);
        mouse_msg msg = {0};
        for ( ; is_run(); delay_fps(60))
        {
            while (mousemsg())
            {
                msg = getmouse();

                //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

                if((int)msg.is_left()&&msg.is_up())
                {

                    int x=msg.x;
                    int y=msg.y;

                    if(x>=247&&x<=270&&y>=301&&y<=329)
                    {
                        INDEX=6;//注册
                        return 0;
                    }
                    else if(x>=346&&x<=368&&y>=301&&y<=327)
                    {
                        INDEX=4;//不注册
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}
//注册
int regist()
{
    PIMAGE img;

    char nam[100];

    inputbox_getline("请输入", "账号（回车确认）", nam, 100);
    inputbox_getline("请输入", "账号（回车确认）", nam, 100);

    int i;
    for( i=0; i<LEN; i++)
    {
        if(strcmp(user[i].name,nam)==0)
        {
            cleardevice();
            img=newimage();
            getimage(img,"img\\exist.png");
            putimage_transparent(NULL,img,150,220,0);
            getch();
            INDEX=4;
            cleardevice();
            return 0;
        }
    }

    if(i==LEN)
    {
        cleardevice();
        img=newimage();
        getimage(img,"img\\success1.png");
        putimage_transparent(NULL,img,170,220,0);
        getch();
        cleardevice();

        //回到主菜单
        INDEX=0;

        strcpy(user[LEN].name,nam);
        LEN++;
        return 0;
    }
}

//更多
int other()
{
    PIMAGE img;

    cleardevice();
    setbkcolor(EGERGB(255,255,200));

    img=newimage();
    getimage(img,"img\\login.png");
    putimage_transparent(NULL,img,170,220,0);

    mouse_msg msg = {0};

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();

            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            if((int)msg.is_left()&&msg.is_up())
            {

                int x=msg.x;
                int y=msg.y;

                if(x>=256&&x<=316&&y>=259&&y<=283)
                {
                    INDEX=5;
                    return 0;
                }
                else if(x>=256&&x<=314&&y>=317&&y<=342)
                {
                    INDEX=6;
                    return 0;
                }
                else if(x>=236&&x<=333&&y>=375&&y<=402)
                {
                    INDEX=7;
                    return 0;
                }
            }
        }
    }
}
//主界面
int face()
{
    PIMAGE img;
    int x,y;

    setbkcolor(EGERGB(255,255,255));
    img=newimage();
    getimage(img,"img\\begin01.png");
    putimage(0,0,img);

    mouse_msg msg = {0};

    for ( ; is_run(); delay_fps(60))
    {
        while (mousemsg())
        {
            msg = getmouse();
            if((int)msg.is_left()&&msg.is_up())
            {
                x=msg.x;
                y=msg.y;
                break;
            }
            //xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

            //人机对战
            if(x>=233&&x<=383&&y>=161&&y<=201)
            {
                INDEX=1;
                return 0;
            }

            //好友约战
            else if(x>=228&&x<=388&&y>=244&&y<=279)
            {
                mouse_msg msg = {0};

                for ( ; is_run(); delay_fps(60))
                {
                    while (mousemsg())
                    {
                        msg = getmouse();
                        if((int)msg.is_left()&&msg.is_up())
                        {
                            x=msg.x;
                            y=msg.y;

                        }
                        xyprintf(0, 0, "x = %10d  y = %10d",msg.x, msg.y, msg.wheel);

                        cleardevice();
                        setbkcolor(EGERGB(255,255,255));
                        img=newimage();
                        getimage(img,"img\\begin.png");
                        putimage(170,550,img);

                        if(x>=213&&x<=376&&y>=581&&y<=615)
                        {
                            INDEX=2;
                            return 0;
                        }
                    }
                }
            }

            //单机模式
            else if(x>=230&&x<=388&&y>=327&&y<=358)
            {
                INDEX=3;
                return 0;
            }

            //退出游戏
            else if(x>=226&&x<=387&&y>=405&&y<=444)
            {
                INDEX=-1;
                return 0;
            }
            //更多
            else if(x>=263&&x<=334&&y>=485&&y<=516)
            {
                INDEX=4;
                return 0;
            }
        }
    }
    return 1;
}
void openfile()
{
    FILE *fp;

    fp=fopen("\\MyGame\\user.txt","r");
    if(fp==NULL)
    {
        printf("Error\n");
        return ;
    }
    int i=0;
    while(!feof(fp))
    {
        fscanf(fp,"%s %d %d %lf\n", user[i].name,&user[i].win,&user[i].total,&user[i].score);
        i++;
    }
    fclose(fp);
    LEN=i;

}
int main()
{
    playmusic();
    PIMAGE img;
    openfile();
    printf("%d\n",LEN);
    initgraph(600, 650);
    now_user=-1;
    loading();
    while(1)
    {
        if(INDEX==-1)
        {
            printf("%d\n",LEN);
            FILE *fp;
            fp=fopen("\\MyGame\\user.txt","w");
            if(fp==NULL)
            {
                printf("Error\n");
            }
            for(int i=0; i<LEN; i++)
            {
                fprintf(fp,"%s %d %d %lf\n", user[i].name,user[i].win,user[i].total,user[i].score);
            }
            fclose(fp);
            stopmusic();
            break;
        }
        if(INDEX==0)
            face();
        if(INDEX==1)
            pc();
        if(INDEX==2)
            Kobe();
        if(INDEX==3)
            Double();
        if(INDEX==4)
            other();
        if(INDEX==5)
            login();
        if(INDEX==6)
            regist();
        if(INDEX==7)
            Rank();
    }

    //printf("%d ",LEN);

    stopmusic();
    closegraph();
    system("pause");
    return 0;
}
