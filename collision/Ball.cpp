#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

#define X 0
#define Y 1
#define PI 3.1415


//子彈的位置、速度、半径
GLfloat bullet_pos[] = { 
                         -0.5, 
                         0.0 
                        };

GLfloat bullet_vel[] = { 
                        0.01,
                        0.0 
                        };

GLfloat bullet_radius = 0.01;

//子彈質量(kg)
GLfloat bullet_mass = 1;


//第二个球的位置、速度、半径
GLfloat ball_pos[] = { 
                          0.5
                        , 0.0
                        };

GLfloat ball_vel[] = {
                          //x方向速度
                          0.0
                          //y方向速度
                        , 0.0
                        };

GLfloat ball_radius = 0.1;

//球質量(kg)
GLfloat ball_mass = 10;

//偵測是否發生碰撞
bool collides = false;

//子彈撞擊點
GLfloat hitpoint[2];

GLdouble VR ;
GLdouble RR ;
//20230517新增
//(速度向量inner product碰撞點到圓心之向量)/碰撞點到圓心之向量inner product碰撞點到圓心之向量
GLdouble translate_vel_K;
GLdouble translate_vector[2];
GLdouble rotate_vector[2];
GLdouble BallCircumference = 2*ball_radius*PI;
GLdouble rotate_vel;
GLdouble rotate_degree;
GLdouble accumulated_rotate_degree;
GLfloat drag_coefficent_sphere = 0.47;


GLfloat* move(GLfloat* pos, GLfloat* vel,GLfloat drag_coefficent);
void handleCollisions();
void renderScene();
GLfloat ComputeHitPoint();
GLdouble InnerProduct(GLfloat x1, GLfloat x2,GLfloat y1, GLfloat y2);

//移动球
GLfloat*  move(GLfloat* pos, GLfloat* vel,GLfloat drag_coefficent){

    vel[X] = vel[X]*drag_coefficent;
    vel[Y] = vel[Y]*drag_coefficent;
    printf("ball_vel[X]=%f\n",ball_vel[X]);
    //將傳入的Pos加上傳入的vel
    pos[X] += vel[X];
    pos[Y] += vel[Y];

    return pos,vel;
}

GLfloat ComputeHitPoint() {
    //計算撞擊點
    hitpoint[X] = bullet_pos[X] - ball_pos[X];
    hitpoint[Y] = bullet_pos[Y] - ball_pos[Y];

    return hitpoint[X], hitpoint[Y];
}
GLdouble InnerProduct(GLfloat x1,GLfloat x2,GLfloat y1,GLfloat y2) {
    GLdouble scalar=(x1 * x2) + (y1 * y2);

    return scalar;
}




GLfloat compute_rotate_degree() {
    //VR = InnerProduct(bullet_vel[X], hitpoint[X], bullet_vel[Y], hitpoint[Y]);
    VR = (bullet_vel[X] * hitpoint[X]) + (bullet_vel[Y] * hitpoint[Y]);
    //RR= InnerProduct(hitpoint[X], hitpoint[X], hitpoint[Y], hitpoint[Y]);
    RR = (hitpoint[X] * hitpoint[X]) + (hitpoint[Y] * hitpoint[Y]);
    translate_vel_K = VR / RR;

    translate_vector[X] =translate_vel_K * hitpoint[X];
    translate_vector[Y] =translate_vel_K * hitpoint[Y];

    rotate_vector[X] =bullet_vel[X] - translate_vector[X];
    rotate_vector[Y] =bullet_vel[Y] - translate_vector[Y];

    rotate_vel = sqrt(
        //X軸長度^2+Y軸長度^2，開根號
        pow(rotate_vector[X], 2)
        + pow(rotate_vector[Y], 2)
    );

    if(hitpoint[Y]>0){
        rotate_degree = -(360 * (rotate_vel / BallCircumference));
    }else {
        rotate_degree = 360 * (rotate_vel / BallCircumference);
    }


    printf("rotate_degree=%f\n", rotate_degree);
    return rotate_degree;
}


//检测和处理碰撞
void handleCollisions(){
    //不停计算兩球距離
    GLfloat dist = sqrt(
        //X軸長度^2+Y軸長度^2，開根號
        pow(bullet_pos[X] - ball_pos[X], 2)
        + pow(bullet_pos[Y] - ball_pos[Y], 2)
    );

    

    //若兩球發生碰撞
    if (dist <= bullet_radius + ball_radius){
        collides = true;
        ComputeHitPoint();
        compute_rotate_degree();
        
        


        ball_mass += bullet_mass;
        
        //计算碰撞面的法向量，
        GLfloat normal[] = { 
                            ball_pos[X] - bullet_pos[X]
                            ,ball_pos[Y] - bullet_pos[Y]
                            };

        GLfloat len = sqrt(
                                  pow(normal[0], 2) 
                                 +pow(normal[1], 2)
                              );

        //將向量單位化
        normal[0] /= len;
        normal[1] /= len;


        //计算碰撞面的切向量
        GLfloat tangent[] = { 
                                -normal[1]
                                ,normal[0]
        };
        printf("tangent[0]=%f,tangent[1]=%f\n",tangent[0], tangent[1]);



        //计算子彈的法向速度和相对速度
        GLfloat normal_vel = (bullet_vel[0] * normal[0]) 
                            + (bullet_vel[1] * normal[1])
                            ;

        GLfloat relative_vel[] = { 
                                    ball_vel[X] - bullet_vel[X]
                                    , 
                                    ball_vel[Y] - bullet_vel[Y]
                                };
        printf("line 188!\n");
       
        //计算新的法线速度
        GLfloat new_normal_vel = -normal_vel;

        //计算新的相对速度
        GLfloat new_relative_vel[] = { 

            /*
            當兩個球發生碰撞時，會產生法向量和切向量上的力。
            法向量上的力會導致translate，
            切向量上的力會導致rotate。
            為了計算法向量上的反彈速度，
            我們需要先計算法向量反彈後的速度（即新的法向量速度）。
            而為了計算切向量上的反彈速度，我們需要使用相對速度（即兩個球之間的相對速度）。
            由於相對速度包含了法向量和切向量的兩個分量，
            所以我們需要將相對速度的切向量分量和新的法向量速度的法向量分量合併為一個新的速度向量，
            以表示球碰撞後的總速度向量。
            最終，將這個新的速度向量賦值給球的速度向量，使球在碰撞後發生反彈運動。
            */
                                    //法向量速度+相對速度
                                    new_normal_vel * normal[0] 
                                    +relative_vel[0] * tangent[0]
                                    ,
                                    new_normal_vel * normal[1] 
                                    + relative_vel[1] * tangent[1]
                                    };


    bullet_vel[X] -= new_relative_vel[X];
    bullet_vel[Y] -= new_relative_vel[Y];

    //20230518
    ball_vel[X] -= ((bullet_mass * new_relative_vel[X]) / ball_mass);
    ball_vel[Y] -= ((bullet_mass * new_relative_vel[Y]) / ball_mass);
    printf("ball_vel_place_line_224\n");
    
    
        //撞到時移動球
        while (dist <= bullet_radius + ball_radius){
            
            
            //move(ball_pos,ball_vel,1);

            dist = sqrt(
                pow(bullet_pos[0] - ball_pos[0], 2) 
                + pow(bullet_pos[1] - ball_pos[1], 2)
            );
        }

    }
}

//渲染场景
void renderScene(){
    //清除颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //當沒有碰撞時移動子彈
    if(collides==false){
        move(bullet_pos, bullet_vel,1);
    }
    move(ball_pos, ball_vel,0.95);
    handleCollisions();

    //绘制子彈
    if(collides==false){
    glColor3f(1.0, 1.0, 0.0);
    }
    else {
        glColor3f(0.0, 0.0, 0.0);
    }
    glPushMatrix();
    //動態劃出子彈圖案
    glTranslatef(
                 bullet_pos[X]
                ,
                bullet_pos[Y]
                ,0
                );
    glutSolidSphere(bullet_radius, 20, 20);

    glPopMatrix();




    //绘制球
    glColor3f(0.0, 0.0, 1.0); 
    glPushMatrix();
    
    glTranslatef(
                 ball_pos[X]
                 ,
                ball_pos[Y]
                 ,0
                );


    if (collides == true) {
        accumulated_rotate_degree += rotate_degree;
        accumulated_rotate_degree *= 0.95;
        printf("accumulated_rotate_degree=%f\n", accumulated_rotate_degree);
        glRotatef(accumulated_rotate_degree, 0.0, 0.0, 1.0);
    }
    glutSolidSphere(ball_radius, 20, 20);
    

    


    //畫彈著點位置
    glColor3f(1.0, 1.0, 0.0);

    glTranslatef(
        hitpoint[X]
        ,hitpoint[Y]
        ,0
    );
    glutSolidSphere(bullet_radius, 20, 20);
    glPopMatrix();

    glutSwapBuffers();
}
/*--------------------------------------------------
 * Keyboard callback func.
 */
void key_func(unsigned char key, int x, int y){
    if (key == 'w'){
        //子彈往上移
        bullet_pos[Y] += 0.01;

    }else if (key == 's') {
        //子彈往下移
        bullet_pos[Y] -= 0.01;

    }
    //t=重新開始
    else if (key==' ') {
        //子彈靜止
        bullet_pos[X] = -0.5;
        bullet_pos[Y] = 0.0;
        bullet_vel[X] = 0.01;
        bullet_vel[Y] = 0.0;

        //球初始化
        ball_pos[X] = 0.5;
        ball_pos[Y] = 0.0;
        ball_vel[X] = 0.0;
        ball_vel[Y] = 0.0;
        ball_mass = 0.1;
        rotate_degree = 0.0;
        accumulated_rotate_degree = 0.0;

        collides = false;

    }
    else if (key == '-') {
        bullet_vel[X] -= 0.01;
    }
    else if (key == '+') {
        bullet_vel[X] += 0.01;
    }
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
    glutKeyboardFunc(key_func);

    //设置清除颜色
    glClearColor(0.0, 0.0, 0.0, 0.0);

    //启动glut主循环
    glutMainLoop();

    return 0;
}