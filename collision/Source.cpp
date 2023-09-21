#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define X 0
#define Y 1




//第一个球的位置、速度、半径
GLfloat ball_X_pos[] = { 
                         0.0, 
                         0.0 
                        };

GLfloat ball_X_vel[] = { 
                        1.0, 
                        0.0 
                        };

GLfloat ball_X_radius = 0.01;


//第二个球的位置、速度、半径
GLfloat ball_Y_pos[] = { 
                          1.0
                        , 0.0
                        };

GLfloat ball_Y_vel[] = {
                         -1.0
                        , 0.0
                        };

GLfloat ball_Y_radius = 0.1;







//移动球
void moveBall(GLfloat* pos, GLfloat* vel, GLfloat dt){
    //將傳入的Position加上傳入的vel
    pos[X] += vel[X] 
        * dt
        ;
    pos[Y] += vel[Y] 
        * dt
        ;
}





//检测和处理碰撞
void handleCollisions(){
   
    //不停计算兩球距離
    GLfloat dist = sqrt(
        //X軸長度^2+Y軸長度^2，開根號
        pow(ball_X_pos[X] - ball_Y_pos[X], 2)
        + pow(ball_X_pos[Y] - ball_Y_pos[Y], 2)
    );


    //printf("dist=%f\n",dist);
    


    //如果距离小于两球半径之和，说明两球发生碰撞
    if (dist < ball_X_radius + ball_Y_radius){
        //计算碰撞面的法向量，
        GLfloat normal[] = { 
                            ball_Y_pos[X] - ball_X_pos[X]
                            ,ball_Y_pos[Y] - ball_X_pos[Y]
                            };

        GLfloat length = sqrt(
                                  pow(normal[0], 2) 
                                 +pow(normal[1], 2)
                              );

        //將向量單位化
        normal[0] /= length;
        normal[1] /= length;


        //计算碰撞面的切向量
        GLfloat tangent[] = { 
                                -normal[1]
                                ,normal[0]
        };
        printf("tangent[0]=%f,tangent[1]=%f\n",tangent[0], tangent[1]);

        //计算球1的法向速度和相对速度
        GLfloat normal_vel = (ball_X_vel[0] * normal[0]) 
                            + (ball_X_vel[1] * normal[1])
                            ;

        GLfloat relative_vel[] = { 
                                    ball_Y_vel[0] - ball_X_vel[0]
                                    , 
                                    ball_Y_vel[1] - ball_X_vel[1]
                                };
       
        //计算新的法线速度
        GLfloat new_normal_vel = -normal_vel;

        //计算新的相对速度
        GLfloat new_relative_vel[] = { 

            /*
            当两个球发生碰撞时，会产生法向量和切向量上的力。
            法向量上的力會導致translate，
            切向量上的力會導致rotate。

            为了计算法向量上的反弹速度，
            我们需要先计算法向量反弹后的速度（即新的法向量速度）。
            而为了计算切向量上的反弹速度，我们需要使用相对速度（即两个球之间的相对速度）。
            由于相对速度包含了法向量和切向量的两个分量，
            所以我们需要将相对速度的切向量分量和新的法向量速度的法向量分量合并为一个新的速度向量，
            以表示球碰撞后的总速度向量。
            
            最终，将这个新的速度向量赋值给球的速度向量，使球在碰撞后发生反弹运动。
            */
                                    //法向量速度+相對速度
                                    new_normal_vel * normal[0] 
                                    +relative_vel[0] * tangent[0]
                                    ,
                                    new_normal_vel * normal[1] 
                                    + relative_vel[1] * tangent[1]
                                    };


//计算新的速度
ball_X_vel[0] -= new_relative_vel[0];
ball_X_vel[1] -= new_relative_vel[1];

ball_Y_vel[0] += new_relative_vel[0];
ball_Y_vel[1] += new_relative_vel[1];



//當兩球距離小於兩球半徑和
while (dist <= ball_X_radius + ball_Y_radius)
{
    moveBall(ball_X_pos, ball_X_vel, -0.001);
    moveBall(ball_Y_pos, ball_Y_vel, -0.001);

    dist = sqrt(
        pow(ball_X_pos[0] - ball_Y_pos[0], 2) 
        + pow(ball_X_pos[1] - ball_Y_pos[1], 2)
    );
}
    }
}

//渲染场景
void renderScene()
{
    //清除颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //移动球并检测碰撞
    moveBall(ball_X_pos, ball_X_vel, 0.001);
    moveBall(ball_Y_pos, ball_Y_vel, 0.001);
    handleCollisions();



    //绘制球X，为红色
    glColor3f(1.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(
                 ball_X_pos[0]
                ,ball_X_pos[1]
                ,0
                );
    glutSolidSphere(ball_X_radius, 20, 20);
    glPopMatrix();




    //绘制球Y，为蓝色
    glColor3f(0.0, 0.0, 1.0); 
    glPushMatrix();
    glTranslatef(
                 ball_Y_pos[0]
                 ,ball_Y_pos[1]
                 ,0
                );
    glutSolidSphere(ball_Y_radius, 20, 20);




    glPopMatrix();
    glutSwapBuffers();
}



int main(int argc, char** argv)
{
    //初始化glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("OpenGL Collision Detection");

    //注册回调函数
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);

    //设置清除颜色
    glClearColor(0.0, 0.0, 0.0, 0.0);

    //启动glut主循环
    glutMainLoop();

    return 0;
}