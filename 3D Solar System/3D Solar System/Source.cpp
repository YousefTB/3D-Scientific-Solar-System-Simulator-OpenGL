#define GLUT_LEGACY
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GL/glut.h>
#define STB_IMAGE_IMPLEMENTATION
#include "GL/stb_image.h"
#include <cmath>
#define PI 3.14159265358979323846f
using namespace std;


// Global Variables - متغيرات عامةً عشان تسمّع في الكود كله
// Textures Part - الجزء الخاص بالتيكتشر
GLuint bgTextureID[11];                // اراي هنخزن فيها التيكتشرز بتاعت الكواكب
unsigned char* data = NULL;
int width, height, channels;

// Planets Designing Part - الجزء الخاص بتصميم الكواكب وحركتها
GLUquadricObj* sphere = NULL;
// The angles of the planets individually - التحكم في زاوية كل كوكب منفرد (عشان مايبقوش كلهم بنفس الزاوية -اعتمادية السرعة
GLfloat angle_1 = 0.0f;
GLfloat angle_2 = 0.0f;
GLfloat angle_3 = 0.0f;
GLfloat angle_4 = 0.0f;
GLfloat angle_5 = 0.0f;
GLfloat angle_6 = 0.0f;
GLfloat angle_7 = 0.0f;
GLfloat angle_8 = 0.0f;
GLfloat speed_1 = 0.68f;         // The speed of each planet - سرعة كل كوكب
GLfloat speed_2 = 0.44f;
GLfloat speed_3 = 0.31f;
GLfloat speed_4 = 0.23f;
GLfloat speed_5 = 0.15f;
GLfloat speed_6 = 0.10f;
GLfloat speed_7 = 0.97f;
GLfloat speed_8 = 0.54f;
GLfloat rot_angle1 = 0.0f;       // The rotational angle of planets - الدوران الزاوّي لكل كوكب (الي بيحدد دوران الكوكب
GLfloat rot_angle2 = 0.0f;
GLfloat rot_angle3 = 0.0f;
GLfloat rot_angle4 = 0.0f;
GLfloat rot_angle5 = 0.0f;
GLfloat rot_angle6 = 0.0f;
GLfloat rot_angle7 = 0.0f;
GLfloat rot_angle8 = 0.0f;
GLfloat temb_speed_1;           // Temperuary Speed Variables to be restored - لاستعادة سرعات الكواكب بعد انتهاء مشهد الاصطدام
GLfloat temb_speed_2;
GLfloat temb_speed_3;
GLfloat temb_speed_4;
GLfloat temb_speed_5;
GLfloat temb_speed_6;
GLfloat temb_speed_7;
GLfloat temb_speed_8;
int limit = 7.0f;                  // تحديد الحد الأقصى والأدنى لسرعات الكواكب
int limit2 = 0.0f;
bool collision = false;            // Collision Mode Controller - التحكّم في مود الاصطدام
float mars_radius = 25.0f;
float asteroid_radius = 5.0f;
float translation;                 // Translation part of the Asteroid - الجزء الحركي الخاص بالكويكب


// Camera Moving Part - الجزء الخاص بالتحكّم في الكاميرا
float lastX = 0, lastY = 0;
float cameraPos[3] = { 0.0f, 0.0f, 9000.0 };
float centerPos[3] = { 0.0f, 0.0f, 0.0f };
float distanceToCenter = 9000.0f;
float cameraYaw = 1.0f;
float cameraPitch = 1.0f;
bool firstMouse = true;
float speed = 0.1f;
float yawRadians = 0;




// specialKeys Function - دالة تعريف المفاتيح الخاصة بالمكتبة نفسها بتاعت opengl
void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_PAGE_UP:             // the camera goes up on y-axis
        centerPos[1] += speed * 1000;
        cameraPos[1] += speed * 1000;
        break;
    case GLUT_KEY_PAGE_DOWN:          // the camera goes down on y-axis
        centerPos[1] -= speed * 1000;
        cameraPos[1] -= speed * 1000;
        break;
    }
}

// updateCameraDirection function to calculate the direction - دالة لحساب متجهات الكاميرا في فضاء ثلاثي الابعاد
void updateCameraDirection()
{
    // calculate the new camera direction vector - حساب المتجه الجديد الي هتاخده الكاميرا سواء بالماوس أو الكيبورد
    float yawRadians = cameraYaw * PI / 180.0f;
    float pitchRadians = cameraPitch * PI / 180.0f;
    float direction[3] = {
        cosf(yawRadians) * cosf(pitchRadians),
        sinf(pitchRadians),
        sinf(yawRadians) * cosf(pitchRadians)
    };


    for (int i = 0; i < 3; i++)
        centerPos[i] = cameraPos[i] + direction[i] * distanceToCenter;

}

// mouse function to update the center of the camera - دالة لتحريك الكاميرا وتحريك السنتر باستخدام حركة الماوس
void mouse(int xpos, int ypos)
{

    if (firstMouse)           // if mouse never used before - اذا لم يتم تحريك الماوس ، هياخد قيم مبدئية لمكان الماوس
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // the rate change of x, y positions of the mouse - حساب مقدار التغير في مكان الماوس عشان التوجيه
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;   // مدى حساسية حركة الماوس للكاميرا
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    cameraYaw += xoffset;
    cameraPitch += yoffset;


    // عشان لما اجي ابص لفوق اوي ، او لتحت اوي ، الكاميرا ما تتقلبش واحافظ على المستوى (اقل من زاوية 90)
    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;

    float yawRadians = cameraYaw * PI / 180.0f;

    updateCameraDirection();
}


//Keyboard Function for different keys - استخدام الكيبورد في تحكمات متعددة
void keyboard_callback(unsigned char key, int x, int y)
{
    
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT) // Shift to increase the speed of camera movement - زيادة سرعة حركة الكاميرا
    {
        speed *= 10;
    }
    if (glutGetModifiers() & GLUT_ACTIVE_CTRL) // تقليل سرعة حركة الكاميرا بـ زر كنترول
        speed /= 10.0f;

    switch (key)
    {
        case 'w': // forward movement - حركة امامية
            for (int i = 0; i < 3; i++)
                cameraPos[i] += speed * (centerPos[i] - cameraPos[i]);
            break;
        case 's': // backward movement - حركة خلفية
            for (int i = 0; i < 3; i++)
                cameraPos[i] -= speed * (centerPos[i] - cameraPos[i]);
            break;
        case 'a': // left movement - حركة يسارية
        {
            float right[3] = {
                sinf(yawRadians + PI / 2.0f),
                0.0,
                cosf(yawRadians + PI / 2.0f)
            };
            for (int i = 0; i < 3; i++)
                cameraPos[i] -= speed * 1000 * right[i];
        }
        break;
        case 'd': // right movement - حركة يمينية
        {
            float right[3] = {
                sinf(yawRadians + PI / 2.0f),   
                0.0,
                cosf(yawRadians + PI / 2.0f)
            };
            for (int i = 0; i < 3; i++)
                cameraPos[i] += speed * 1000 * right[i];
        }
        break;
        case 27:     // ESC Key - زر الاسكيب عشان اطلع من البرنامج
            exit(1);
            break;
        case 'p':   // زيادة سرعة الكواكب الدورانية
            if (speed_1 == limit2) // عشان لما السرعة تتصفّر وارجعها تاني ، ترجع للسرعات الطبيعية ومايبقوش متساويين
            {
                speed_1 = 0.68f;
                speed_2 = 0.44f;
                speed_3 = 0.31f;
                speed_4 = 0.23f;
                speed_5 = 0.15f;
                speed_6 = 0.10f;
                speed_7 = 0.97f;
                speed_8 = 0.54f;
            }
            else
            {
                speed_1 += 1.0f;
                speed_2 += 1.0f;
                speed_3 += 1.0f;
                speed_4 += 1.0f;
                speed_5 += 1.0f;
                speed_6 += 1.0f;
                speed_7 += 1.0f;
                speed_8 += 1.0f;
                if (speed_1 > limit)
                {
                    speed_1 = limit;
                    speed_2 = limit;
                    speed_3 = limit;
                    speed_4 = limit;
                    speed_5 = limit;
                    speed_6 = limit;
                    speed_7 = limit;
                    speed_8 = limit;
                }
            }
            break;
        case 'o': // ابطاء السرعات الدورانية للكواكب
            speed_1 -= 1.0f;
            speed_2 -= 1.0f;
            speed_3 -= 1.0f;
            speed_4 -= 1.0f;
            speed_5 -= 1.0f;
            speed_6 -= 1.0f;
            speed_7 -= 1.0f;
            speed_8 -= 1.0f;
            if (speed_1 < limit2)
            {
                speed_1 = limit2;
                speed_2 = limit2;
                speed_3 = limit2;
                speed_4 = limit2;
                speed_5 = limit2;
                speed_6 = limit2;
                speed_7 = limit2;
                speed_8 = limit2;
            }
            break;
        case 'm': // Activate the Collision Mode - تفعيل وضع الاصطدام
            if (!collision) // اذا وضع الاصطدام مش شغال (هيشتغل بعد كدا لأني ضغطت الزر) هيخزن السرعات القديمة عشان هتتعدل
            {
                temb_speed_1 = speed_1;
                temb_speed_2 = speed_2;
                temb_speed_3 = speed_3;
                temb_speed_4 = speed_4;
                temb_speed_5 = speed_5;
                temb_speed_6 = speed_6;
                temb_speed_7 = speed_7;
                temb_speed_8 = speed_8;
                collision = true; // فعّل وضع الاصطدام ، المتغير دا هيتم استخدامه تحت بعد كدا عشان أتحكم في المود
                translation = 100.0f;
            }
            break;
        case 'n': // Activate Normal Mode - تفعيل الوضع الطبيعي للفضاء
            if (collision) // لو مود الاصطدام لسة شغال ، هنرجع السرعات الي كانت موجودة ، ونقفل مود الاصطدام
            {
                speed_1 = temb_speed_1;
                speed_2 = temb_speed_2;
                speed_3 = temb_speed_3;
                speed_4 = temb_speed_4;
                speed_5 = temb_speed_5;
                speed_6 = temb_speed_6;
                speed_7 = temb_speed_7;
                speed_8 = temb_speed_8;
                collision = false;
            }
            break;
    }

    updateCameraDirection();
}

// Texture Loading Function - دالة تحميل صور التيكتشرز للكواكب والمجرّة
void loadBackgroundTexture(unsigned char* data, int index) {
    if (data)
    {
        glGenTextures(1, &bgTextureID[index]);
        glBindTexture(GL_TEXTURE_2D, bgTextureID[index]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        GLenum format = (channels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cerr << "Error loading background texture." << std::endl;
    }
    stbi_image_free(data);
}

// تحديد كل صورة في انديكس جوا اراي تحميل الصور
void load()
{
    unsigned char* image = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/stars.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image, 0);
    unsigned char* image1 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/sun.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image1, 1);
    unsigned char* image2 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/earth.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image2, 2);
    unsigned char* image3 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/mars.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image3, 3);
    unsigned char* image4 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/saturn.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image4, 4);
    unsigned char* image5 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/neptune.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image5, 5);
    unsigned char* image6 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/uranus.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image6, 6);
    unsigned char* image7 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/venus.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image7, 7);
    unsigned char* image8 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/sphere.bmp", &width, &height, &channels, 0);
    loadBackgroundTexture(image8, 8);
    unsigned char* image9 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/mercury.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image9, 9);
    unsigned char* image10 = stbi_load("C:/Users/yousef elbaroudy/source/repos/3D Solar System/3D Solar System/assets/jupiter.jpg", &width, &height, &channels, 0);
    loadBackgroundTexture(image10, 10);
}

// دالة موحدّة لرسم الكواكب ، مع تحديد نصف القطر ، بعدها عن المركز ، والزاوية الدورانية
void drawPlanet(GLfloat radius, GLfloat distance, GLfloat red, GLfloat green, GLfloat blue, GLfloat rot) {

    glPushMatrix();
    sphere = gluNewQuadric();
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);           // عشان تلف حوالين السنتر الأساسي (الي هو الشمس)
    glTranslatef(0.0f, 0.0f, distance);         // بعدها عن السنتر (بعدها عن الشمس)
    glRotatef(300, 1.0f, 0.0f, 0.0f);           // تدوير الكوكب حوالين محور إكس ، عشان اعدل شكل الكوكب
    glRotatef(rot, 0.0f, 0.0f, 1.0f);           // دوران الكوكب حوالين نفسه (محور زد
    glColor3f(red, green, blue);
    gluSphere(sphere, radius, 32, 32);          // إنشاء الكوكب بنصف القطر المحدّد
    gluDeleteQuadric(sphere);
    glPopMatrix();
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (!collision) // لو وضع الاصطدام مش شغال ، هنشغل الكاميرا العاديّة
    {
        gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2],
            centerPos[0], centerPos[1], centerPos[2],
            0.0f, 1.0f, 0.0f);
    }


    if (collision) // لو وضع الاصطدام مش شغال ، هنحدد مواصفات مشهد الاصطدام
    {
        // هنصفر كل السرعات ، عشان تبقى الكواكب ثابتة -بإعتبار انه مشهد الاصطدام ثابت
        speed_1 = 0;
        speed_2 = 0;
        speed_3 = 0;
        speed_4 = 0;
        speed_5 = 0;
        speed_6 = 0;
        speed_7 = 0;
        speed_8 = 0;

        // توجيه الكاميرا على مشهد الاصطدام
        gluLookAt(-40.0f, -10.0f, 920.0f,
            0.0, 0.0f, 1055.0f,
            0.0f, 1.0f, 0.0f);


        // إنشاء كويكب إستعداداً لمشهد الاصطدام بالمواصفات التالية
        // مواصفات استضاءة الكويكب
        GLfloat mat_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
        GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        // إنشاء الكوكب وعليه الصورة الخاصة بيه
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bgTextureID[8]);
        glPushMatrix();
        sphere = gluNewQuadric();
        gluQuadricTexture(sphere, GL_TRUE);
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        glTranslatef(translation, 0.0, 1055.f);
        glRotatef(300, 1.0f, 0.0f, 0.0f);
        gluSphere(sphere, 5.0f, 32, 32);
        gluDeleteQuadric(sphere);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);

        // تحريك الكويكب بمقدار 0.2 تناقصياً في اتجاه المريخ من محور إكس وتثبيت محور زد
        translation -= 0.2f;

        // لقطة الاصطدام
        if (translation < mars_radius + asteroid_radius)
        {
            translation = mars_radius + asteroid_radius; // ثبّت موضع الاصطدام

            // جزء إظهار رسالة على الشاشة بعد مقطع الاصطدام
            glColor3f(255, 255, 255);
            glRasterPos3d(0.0f, 0.0f, 1000.0f);
            const char* string1 = "An Asteroid has crashed into Mars !";

            while (*string1) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string1++);
            }

            glColor3f(255, 255, 255);
            glRasterPos3d(0.0f, -4.0f, 1000.0f);
            const char* string2 = "Press N to return to Normal Mode";

            while (*string2) {
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *string2++);
            }
        }
    }


    // تحديد مواصفات مصدر الضوء
    GLfloat light_position[] = { 0.0, 0.0, 0.0 };
    GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    // رسم الشمس
    glPushMatrix();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    glColor4f(0.2, 0.2, 0.2, 1.0);
    GLfloat amb[] = { 2.0f,2.0f,2.0f,2.0f };
    GLfloat diff[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat spec[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat shine = 128.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[1]);
    sphere = gluNewQuadric();
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 500.0, 32, 32);
    gluDeleteQuadric(sphere);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    

    // الـ (ماتريال) بتاعت كل الكواكب للإستضاءة
    GLfloat mat_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
    GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 100.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[7]);
    drawPlanet(47.0f, 650, 0.8f, 0.4f, 0.0f, rot_angle1); // رسم كوكب الزهرة
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[3]);
    drawPlanet(25.0f, 1055.0f, 0.2f, 0.2f, 1.0f, rot_angle2); // رسم كوكب المريخ
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[10]);
    drawPlanet(200.38f, 1578.0f, 0.7f, 0.2f, 0.2f, rot_angle3); // رسم كوكب المشتري
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[4]);
    drawPlanet(172.39f, 2278.0f, 0.0f, 0.0f, 1.0f, rot_angle4); // رسم كوكب زحل
    glDisable(GL_TEXTURE_2D);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[6]);
    drawPlanet(100.91f, 3850.39f, 0.8f, 0.1f, 0.8f, rot_angle5); // رسم كوكب أورانوس
    glDisable(GL_TEXTURE_2D);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[5]);
    drawPlanet(120.23f, 5651.3f, 0.9f, 0.9f, 0.9f, rot_angle6); // رسم كوكب نبتون
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[9]);
    drawPlanet(10.0f, 557.0f, 0.5f, 0.5f, 0.5f, rot_angle7); // رسم كوكب عطارد
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[2]);
    drawPlanet(50.0f, 850.0f, 0.0f, 1.0f, 0.0f, rot_angle8); // رسم الأرض
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);


    // رسم المجرّة حوالين النظام الشمسي ، وتحديد مادة الاستضاءة
    GLfloat amb2[] = { 2.0f,2.0f,2.0f,2.0f };
    GLfloat diff2[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat spec2[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat shine2 = 128.0f;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec2);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine2);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, bgTextureID[0]);
    glBegin(GL_QUADS);
    sphere = gluNewQuadric();
    gluQuadricTexture(sphere, GL_TRUE);
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 20000.0, 90, 90);
    gluDeleteQuadric(sphere);
    glDisable(GL_TEXTURE_2D);



    glutSwapBuffers();


    // إخفاء الماوس اثناء تشغيل المشهد
    glutSetCursor(GLUT_CURSOR_NONE);


    // تحديد الماوس في سنتر المشهد
    if (firstMouse)
    {
        firstMouse = false;
        glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
    }
    else
    {
        int centerX = glutGet(GLUT_WINDOW_WIDTH) / 2;
        int centerY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
        if (lastX != centerX || lastY != centerY)
            glutWarpPointer(centerX, centerY);
    }
    lastX = glutGet(GLUT_WINDOW_WIDTH) / 2;
    lastY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
}


// Timer function to update the rotational angle - speed - دالة التايمر لتحديث الزاوية الدورانية لكل كوكب منفرد
void timer(int value) {
    angle_1 += 1.0;
    angle_2 += 1.0;
    angle_3 += 1.0;
    angle_4 += 1.0;
    angle_5 += 1.0;
    angle_6 += 1.0;
    angle_7 += 1.0;
    angle_8 += 1.0;
    rot_angle1 = speed_1 * angle_1;
    rot_angle2 = speed_2 * angle_2;
    rot_angle3 = speed_3 * angle_3;
    rot_angle4 = speed_4 * angle_4;
    rot_angle5 = speed_5 * angle_5;
    rot_angle6 = speed_6 * angle_6;
    rot_angle7 = speed_7 * angle_7;
    rot_angle8 = speed_8 * angle_8;
    if (rot_angle1 >= 360.0)
    {
        rot_angle1 = 0.0f;
        angle_1 = 0.0f;
    }
    if (rot_angle2 >= 360.0)
    {
        rot_angle2 = 0.0f;
        angle_2 = 0.0f;
    } 
    if (rot_angle3 >= 360.0)
    {
        rot_angle3 = 0.0f;
        angle_3 = 0.0f;
    }
    if (rot_angle4 >= 360.0)
    {
        rot_angle4 = 0.0f;
        angle_4 = 0.0f;
    }
    if (rot_angle5 >= 360.0)
    {
        rot_angle5 = 0.0f;
        angle_5 = 0.0f;
    }
    if (rot_angle6 >= 360.0)
    {
        rot_angle6 = 0.0f;
        angle_6 = 0.0f;
    }
    if (rot_angle7 >= 360.0)
    {
        rot_angle7 = 0.0f;
        angle_7 = 0.0f;
    }
    if (rot_angle8 >= 360.0)
    {
        rot_angle8 = 0.0f;
        angle_8 = 0.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(25, timer, 0);
}

void reshape(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);


    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, aspect, 0.1, 50000); // zfar is large to see the whole details from space perspective

}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Solar System");
    load();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(mouse);
    glutKeyboardFunc(keyboard_callback);
    glutSpecialFunc(specialKeys);
    glutFullScreen();
    glutTimerFunc(25, timer, 0);
    glutMainLoop();
    return 0;
}