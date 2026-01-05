#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>

void create_perspective_matrix(float* matrix, float fov, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov * 0.5f * 3.14159f / 180.0f);
    
    matrix[0] = f / aspect;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = 0.0f;
    
    matrix[4] = 0.0f;
    matrix[5] = f;
    matrix[6] = 0.0f;
    matrix[7] = 0.0f;
    
    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = (far + near) / (near - far);
    matrix[11] = -1.0f;
    
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = (2.0f * far * near) / (near - far);
    matrix[15] = 0.0f;
}

// Функция для создания матрицы вида (камера)
void create_view_matrix(float* matrix, float eyeX, float eyeY, float eyeZ, 
                        float centerX, float centerY, float centerZ,
                        float upX, float upY, float upZ) {
    // Направление взгляда
    float z[3] = {eyeX - centerX, eyeY - centerY, eyeZ - centerZ};
    float zLength = sqrtf(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
    z[0] /= zLength; z[1] /= zLength; z[2] /= zLength;
    
    // Вектор вправо
    float up[3] = {upX, upY, upZ};
    float x[3] = {
        up[1]*z[2] - up[2]*z[1],
        up[2]*z[0] - up[0]*z[2],
        up[0]*z[1] - up[1]*z[0]
    };
    float xLength = sqrtf(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
    x[0] /= xLength; x[1] /= xLength; x[2] /= xLength;
    
    // Вектор вверх
    float y[3] = {
        z[1]*x[2] - z[2]*x[1],
        z[2]*x[0] - z[0]*x[2],
        z[0]*x[1] - z[1]*x[0]
    };
    
    // Создаем матрицу
    matrix[0] = x[0]; matrix[1] = y[0]; matrix[2] = z[0]; matrix[3] = 0.0f;
    matrix[4] = x[1]; matrix[5] = y[1]; matrix[6] = z[1]; matrix[7] = 0.0f;
    matrix[8] = x[2]; matrix[9] = y[2]; matrix[10] = z[2]; matrix[11] = 0.0f;
    
    matrix[12] = -(x[0]*eyeX + x[1]*eyeY + x[2]*eyeZ);
    matrix[13] = -(y[0]*eyeX + y[1]*eyeY + y[2]*eyeZ);
    matrix[14] = -(z[0]*eyeX + z[1]*eyeY + z[2]*eyeZ);
    matrix[15] = 1.0f;
}

// Функция для создания матрицы модели (трансформации объекта)
void create_model_matrix(float* matrix, float x, float y, float z, 
                         float rx, float ry, float rz, float sx, float sy, float sz) {
    // Начинаем с единичной матрицы
    for(int i = 0; i < 16; i++) matrix[i] = 0.0f;
    matrix[0] = 1.0f; matrix[5] = 1.0f; matrix[10] = 1.0f; matrix[15] = 1.0f;
    
    // Масштаб
    matrix[0] *= sx;
    matrix[5] *= sy;
    matrix[10] *= sz;
    
    // Вращение вокруг X
    if(rx != 0.0f) {
        float cosX = cosf(rx * 3.14159f / 180.0f);
        float sinX = sinf(rx * 3.14159f / 180.0f);
        
        float temp[16];
        for(int i = 0; i < 16; i++) temp[i] = matrix[i];
        
        matrix[5] = temp[5] * cosX - temp[9] * sinX;
        matrix[6] = temp[6] * cosX - temp[10] * sinX;
        matrix[7] = temp[7] * cosX - temp[11] * sinX;
        
        matrix[9] = temp[5] * sinX + temp[9] * cosX;
        matrix[10] = temp[6] * sinX + temp[10] * cosX;
        matrix[11] = temp[7] * sinX + temp[11] * cosX;
    }
    
    // Вращение вокруг Y
    if(ry != 0.0f) {
        float cosY = cosf(ry * 3.14159f / 180.0f);
        float sinY = sinf(ry * 3.14159f / 180.0f);
        
        float temp[16];
        for(int i = 0; i < 16; i++) temp[i] = matrix[i];
        
        matrix[0] = temp[0] * cosY + temp[8] * sinY;
        matrix[1] = temp[1] * cosY + temp[9] * sinY;
        matrix[2] = temp[2] * cosY + temp[10] * sinY;
        matrix[3] = temp[3] * cosY + temp[11] * sinY;
        
        matrix[8] = -temp[0] * sinY + temp[8] * cosY;
        matrix[9] = -temp[1] * sinY + temp[9] * cosY;
        matrix[10] = -temp[2] * sinY + temp[10] * cosY;
        matrix[11] = -temp[3] * sinY + temp[11] * cosY;
    }
    
    // Вращение вокруг Z
    if(rz != 0.0f) {
        float cosZ = cosf(rz * 3.14159f / 180.0f);
        float sinZ = sinf(rz * 3.14159f / 180.0f);
        
        float temp[16];
        for(int i = 0; i < 16; i++) temp[i] = matrix[i];
        
        matrix[0] = temp[0] * cosZ - temp[4] * sinZ;
        matrix[1] = temp[1] * cosZ - temp[5] * sinZ;
        matrix[2] = temp[2] * cosZ - temp[6] * sinZ;
        matrix[3] = temp[3] * cosZ - temp[7] * sinZ;
        
        matrix[4] = temp[0] * sinZ + temp[4] * cosZ;
        matrix[5] = temp[1] * sinZ + temp[5] * cosZ;
        matrix[6] = temp[2] * sinZ + temp[6] * cosZ;
        matrix[7] = temp[3] * sinZ + temp[7] * cosZ;
    }
    
    // Позиция
    matrix[12] = x;
    matrix[13] = y;
    matrix[14] = z;
}

#endif