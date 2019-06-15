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

#define SIZE 15//���̴�С
#define Win 6      //����
#define flex4 5    //����
#define block4 4   //����
#define flex3 3    //����
#define block3 2   //����
#define flex2 1    //���
#define Ntype 7    //������
using namespace std;

//1�������2�������0����ո�

//C������ɫ��P����ɫ
int C,P,step,Depth,Bestval,LEN;//LENΪ�û�����
int mp[SIZE][SIZE];//����
int now_user,INDEX,my_color;//��ǰ�û���id

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


//d��ʱ������Bestmove��õ�λ��
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


//����
void wait_conn(SOCKET *sockser, SOCKET *sockcli)
{
    SOCKADDR_IN  addrClient;
    int len = sizeof(SOCKADDR);

    printf("���ڵȴ��ͻ����ӣ���\n");
    *sockcli = accept(*sockser, (SOCKADDR*)&addrClient, &len);
    game_face();
}


void set_socket(SOCKET *sock)
{
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    //����ͨ�ŵ�ַ
    SOCKADDR_IN addrSrv;

    // ��INADDR_ANYת��Ϊ�����ֽ��򣬵��� htonl(long��)��htons(����)
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);
    //��socket���ַ����һ��
    bind(*sock, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    listen(*sock, 10);
}

//��ȡ��ǰ���λ�õ�����
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

//��ӡ����
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

//����ģʽ�Ľ��պͷ���
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

//��������
void playmusic()
{
    mciSendString(TEXT("open 1.mp3 alias back"),NULL,0,NULL);//���ŵ����ַ��ڸ��ļ���Ŀ¼��
    mciSendString(TEXT("Play back repeat"),NULL,0,NULL);
}

//�ر�����
void stopmusic()
{
    mciSendString(TEXT("stop back"),NULL,0,NULL);
    //mciSendString(TEXT("close back"),NULL,0,NULL);
}

//��Ϸ���ؽ���
void loading()
{

    setcolor(EGERGB(255,255,255)); // ��ɫ
    rectangle(90,280,418,310); // ���ľ���
    rectangle(88,278,420,312);

    char s[100]="";
    sprintf(s,"loading   to");
    outtextxy(180,330,s);

    for(int i=3,j=0,k=0; i<32; i++,j++,k++)
    {
        setfillcolor(EGERGB(0,132,255));// ��ɫ  �����ɫ
        bar(95+j*11,285,105+j*11,305); // ʵ���ޱ߿����
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
    cleardevice();//��������
}

//ѡ��˭������
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
                    //������
                    return 1;

                }
                else if(x>=235&&x<=324&&y>=289&&y<=314)
                {
                    return 2;
                    //�Һ���
                }
            }
        }
    }
    return 0;
}

//ѡ��������ɫ
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

            //��ɫ
            if(x>=263&&x<=336&&y>=279&&y<=317)
                return 1;

            //��ɫ
            else if(x>=257&&x<=330&&y>=442&&y<=472)
                return 2;
        }

    }

}

//��ʾ����
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

//Ӯ
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
            //����
            if((int)msg.is_left()&&msg.is_up()&&x>=169&&x<=241&&y>=310&&y<=342)
            {
                init();

                return 1;
            }
            //�˳������˵�
            else if((int)msg.is_left()&&msg.is_up()&&x>=337&&x<=411&&y>=308&&y<=344)
            {
                init();
                // cleardevice();
                return 2;
            }
        }
    }
}

//�ж�ʤ��
int is_win(int x,int y,int color)
{
    //�ж�����
    for(int i=x-4; i<=x; i++)
    {
        if(i>=0&&i+4<SIZE&&mp[i][y]==color&&mp[i+1][y]==color&&mp[i+2][y]==color&&mp[i+3][y]==color&&mp[i+4][y]==color)
        {
            return 1;
        }
    }

    //�жϺ���
    for(int j=y-4; j<=y; j++)
    {
        if(j>=0&&j+4<SIZE&&mp[x][j]==color&&mp[x][j+1]==color&&mp[x][j+2]==color&&mp[x][j+3]==color&&mp[x][j+4]==color)
        {
            return 1;
        }
    }

    //�ж���б
    for(int i=x-4,j=y-4; i<=x; i++,j++)
    {
        if(i>=0&&j>=0&&i+4<SIZE&&j+4<SIZE&&mp[i][j]==color&&mp[i+1][j+1]==color&&mp[i+2][j+2]==color&&mp[i+3][j+3]==color&&mp[i+4][j+4]==color)
        {
            return 1;
        }

    }

    //�ж���б
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


//����
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
    //����
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

    //����
    else if(x>=270&&x<=341&&y>=602&&y<=634)
    {
        game_face();
        init();
        return 3;
    }
    //����
    else if(x>=388&&x<=455&&y>=601&&y<=633)
    {
        init();
        return 4;
    }

    return 5;

}

//������
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

//����˫��ģʽ
int Double()
{
    init();

    PIMAGE img;
    P=C=0;
    int f=color();//1Ϊ��ɫ��2Ϊ��ɫ

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

    //x yΪ���λ�ã�nx nyΪ��������
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

                //����
                if(k==1||k==3)
                {
                    return 0;
                }
                //����
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

//�ж϶Է�ɱ�壬���ط�ֵ
int GetKill()
{
    int i,j,x,y;

    //�õ����ַ����ϵ�����
    int d[4];

    int type[Ntype]= {0};

    //����������ߵ���Щ��
    for(i=P; i<=step; i+=2)
    {
        //Chess������ÿһ�����˵�����
        x=Chess[i].x;
        y=Chess[i].y;
        GetType(x, y, P, d);
        for(j=0; j<4; j++)
        {
            //����������͵�����
            type[d[j]]++;
        }
    }

    //������������Ҫ����������
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
//��������
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

//�ж�����
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

//���ո���
node space_score(int x, int y)
{
    int d[4];//�����ĸ������ϵĿ�����������
    int computer_score=0,people_score=0;
    int Cval[Ntype]= {0,2,2,10,10,50,10000};//���Ը������͵ķ�ֵ
    int Hval[Ntype]= {0,2,2,10,10,10,10};//���
    int computer_type[7]= {0};//������Է��������͵�����
    int people_type[7]= {0};//������Ҹ������͵�����
    node r;

    //�õ��ĸ������ϵ�����
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

    if(people_type[Win]>0)//����
        people_score+=50;
    else if(people_type[flex4]>0)//�л���
        people_score+=50;
    else if(people_type[block4]>1)
        people_score+=50;
    else if(people_type[block4]==1&&people_type[flex3]>0)//���Ļ���
        people_score+=40;
    else if(people_type[flex3]>1)//˫����
        people_score+=20;

    r.x=computer_score;
    r.y=people_score;
    return r;
}

//�ж��ж��ٿ�����
//len�ܳ��ȡ�len2���Ӽӿո񳤶ȡ�block�ϰ��������count����������
//��������������
int check_type(int len, int len2, int block, int count)
{
    if(len>=5&&count>1)
    {
        if(count==5)
            return Win;//����

        //�������ϰ���
        if(len>5&&len2<5&&block==0)
        {
            switch(count)
            {
            case 4:
                return flex4;//����
            case 3:
                return flex3;//����
            case 2:
                return flex2;//���
            }
        }
        else
        {
            switch(count)
            {
            case 4:
                return block4;//����
            case 3:
                return block3;//����
            }
        }
    }
    return 0;
}

//��ѯ�ý�ɫ���ĸ���������
void GetType(int x,int y,int role,int *d)
{
    int a,b;
    int space=0;//�ո�
    int block=0;//�ϰ�
    int len=1;//�ܳ�����
    int len2=1;//count+space
    int count=1;//����������

    //���������ĸ�
    for(a=x+1; a<=x+4; a++)
    {
        //����
        if(a>14)
        {
            if(len2==space+count)
                block++;//��ԵҲ���ϰ�
            break;
        }
        if(mp[a][y]==role)
        {
            //�ո����2˵��ûϷ��space+count>4˵���������
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
        //�����Է����Ӿͽ�������
        else
        {
            //�ϰ���
            if(len2==space+count)
                block++;
            break;
        }
    }

    space=len2-count;

    //������4��
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

    //�ұ�4��
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
    //���
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

    //����
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

    //����
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

    //����
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

    //����
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

//����������Χ�ո��ֵ
void get_space_score(node p)
{
    int x=p.x;
    int y=p.y;
    int a,b;
    //��
    for(a=x-4; a<=x+4; a++)
    {
        if(a<0)
            continue;
        if(a>14)
            break;
        if(mp[a][y]==0)
            Score[a][y]=space_score(a, y);
    }
    //��
    for(b=y-4; b<=y+4; b++)
    {
        if(b<0)
            continue;
        if(b>14)
            break;
        if(mp[x][b]==0)
            Score[x][b]=space_score(x, b);
    }
    //��б
    for(a=x-4,b=y-4; a<=x+4; a++,b++)
    {
        if(a<0||b<0)
            continue;
        if(a>14||b>14)
            break;
        if(mp[a][b]==0)
            Score[a][b]=space_score(a, b);
    }

    //��б
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

//���ӣ�����ӡ
void Make(node next)
{
    int x=next.x;
    int y=next.y;
    step++;
    mp[x][y]=checkcolor(step);
    Chess[step].x=x;
    Chess[step].y=y;

    //���¿ո�ķ�ֵ
    get_space_score(next);
}

//ɾ��
void Del()
{
    int x=Chess[step].x;
    int y=Chess[step].y;
    mp[x][y]=0;
    get_space_score(Chess[step]);
    step--;
}

//�����Χn������������
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

//�����Χ�ĸ��������޸���ɫ����
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

//���ո��Ƿ����,�����ж϶����Ƿ��л�3���ϵ�����
int Checkspace(node p, int role)
{
    int x=p.x;
    int y=p.y;
    int d[4];//�������ַ��������

    //�õ��������
    GetType(x, y, role, d);

    for(int i=0; i<4; i++)
    {
        if(d[i]>=flex2)//��2
            return 1;
    }

    //���Է����ַ��������
    GetType(x, y, 3-role, d);

    for(int i=0; i<4; i++)
    {
        if(d[i]>=flex3)//��3
            return 1;
    }
    return 0;
}

//�õ����п����ߵ�����
int Getspace(int depth,node s[10][100])
{
    int t=0;
    int i,j;
    node p;

    int role=checkcolor(step+1);//�Է�����ɫ

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

//����С�����㷨
int max_min(int depth,int alpha,int beta)
{
    int len;
    int is_win=Check();

    node s[10][100];//�������п��µ�����

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
        Make(s[depth][len]);//����
        int val=-max_min(depth-1,-beta,-alpha);//�ݹ�
        Del();//����

        //��֦
        //max��
        if(val>=beta)
        {
            if(depth==Depth)
                Bestmove=s[depth][len];
            return val;
        }
        //min��
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

//��ʾ����
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


//ѡ���˻��ļ�����ģʽ
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

                //��ģʽ
                if(x>=217&&x<=384&&y>=268&&y<=306)
                {
                    *n=2;
                    Depth=2;
                    return ;
                }
                //����ģʽ
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
//�˻���ս
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
        //������
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
                            //����
                            if(t==1||t==3)
                            {
                                INDEX=1;
                                return 0;
                            }
                            //����
                            else if(t==2||t==4)
                            {
                                INDEX=0;
                                return 0;
                            }
                        }

                        //�Ѿ����ӻ��߻�����
                        else if(t==0)
                            continue;

                        flag=1;
                    }

                }

                if(flag==1)
                    break;
            }
        }

        //��������
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

                //����С�����㷨����ȡ�������
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
                    //����
                    if(d==1)
                    {
                        return 0;
                    }
                    //����
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
//���а�
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

    //����������
    sort(user,user+LEN,cmp);

    //printf("����  ��ʤ  �ܾ��� ����\n");

    setfont(35,0,"������κ");

    char s[100];

    setcolor(BLUE);

    outtextxy(100,100,"����");

    outtextxy(200,100,"��ʤ");

    outtextxy(300,100,"�ܾ���");

    outtextxy(450,100,"����");

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

//��¼
int login()
{
    PIMAGE img;

    char nam[100];

    inputbox_getline("������", "�˺ţ��س�ȷ�ϣ�", nam, 100);
    inputbox_getline("������", "�˺ţ��س�ȷ�ϣ�", nam, 100);

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
                        INDEX=6;//ע��
                        return 0;
                    }
                    else if(x>=346&&x<=368&&y>=301&&y<=327)
                    {
                        INDEX=4;//��ע��
                        return 0;
                    }
                }
            }
        }
    }

    return 0;
}
//ע��
int regist()
{
    PIMAGE img;

    char nam[100];

    inputbox_getline("������", "�˺ţ��س�ȷ�ϣ�", nam, 100);
    inputbox_getline("������", "�˺ţ��س�ȷ�ϣ�", nam, 100);

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

        //�ص����˵�
        INDEX=0;

        strcpy(user[LEN].name,nam);
        LEN++;
        return 0;
    }
}

//����
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
//������
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

            //�˻���ս
            if(x>=233&&x<=383&&y>=161&&y<=201)
            {
                INDEX=1;
                return 0;
            }

            //����Լս
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

            //����ģʽ
            else if(x>=230&&x<=388&&y>=327&&y<=358)
            {
                INDEX=3;
                return 0;
            }

            //�˳���Ϸ
            else if(x>=226&&x<=387&&y>=405&&y<=444)
            {
                INDEX=-1;
                return 0;
            }
            //����
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
