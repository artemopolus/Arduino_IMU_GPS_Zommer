#ifndef __SEGPOLY_H__
#define __SEGPOLY_H__

#include <math.h>
//#include <QDebug>

//#define S_COUNT 5
//#define T_COUNT 2
#define S_COUNT 6
#define T_COUNT 3

//#define SG_LOG_ON 1

struct ASegPoly{
	//Минимальное отклонение
    //аппроксимированных данных от исходных
    float d_min;
    // Результат сравнения ранее записанного отклонения с текущим
    int flag_cond;
    // Исходные данные
    float * mass;
    int mass_num;
    // Данные, с которыми производится работа
    float * mass_tmp;
    // Коэффициенты аппроксимирующего полинома
    int coeff_res_num = 6;
    // Массив коэффициентов
    float * coeff_res;
    // Отклонение аппроксимированных данных от исходных
    float d_res;
    // Результат аппроксимации нормированных данных
    float * approx_res;
    // Хранение временных данных
    float * approx_min;
    // Результат аппроксимации, возвращенный в исходный масштаб
    float * approx_res_rn;
    // Нормированные исходные данные
    float * norm_res;  
    // Список сегментов
    int * seg;
    // Количество сегментов
    int seg_num;
    // Результат аппроксимации найденного сегмента
    float * res_approx;
    // Коэффициенты для сегментов
    float * seg_coeff;
};



//struct ASegPoly obj;


void NormData_v2(float *src, int src_num, float x_norm, float y_norm, float &v, float *x, float *y, float & k_p, float & k_m);
void PrepMtoGauss(float * R, float * T, int len2);
void GaussSolveM(float * R, float * T, float * res, int len);
void GausCalc(float * M_a, float * M_t, int len, float * res);
void ExtrNullSeg_p5(float * x, float * y, int len, float v, float *K);
void GenPolyN(float * K, int K_len, float * x, float * y, int len);
void CalcSqMorePt(float * y, int y_num, float * A, int A_num, int pt_count, float & sum);
void ApproxData(int st, int en, float x_len, float y_len, float y_prev);
void CreateSegPoly( float * str, int str_num);
void InitSet();
void CompareD();
void GetCoeff(int st, int en, float y_prev);
void GetSegment(int mincount, int step, float x_len, float y_len, int stat);
void GetSegmentResultCount( int & seg_count);
void GetSegmentResultData( int * seg_result);


float powMy(float val, int st);

void ExtrNullSeg_v2(float * K, float * x, float * y, const float y_prev, const int count);
void ExtrNullSeg_v2_1(float * K, float * x, float * y, const float y_prev, const int st,const int count);
void ExtrNullSeg_v2_2(float * K, float * x, float * y, const float y_prev, const int st,const int count);
void GenPoly3(float * K, float * x, float * y, int len);
void GenPoly3_1(float * K, float * x, float * y, const int st, const int len);


void calcDeltaPoly3(float * K, float * x, float * y, const int len, float * sq);
void calcDeltaPoly3_1(float * K, float * x, float * y, const int st, const int len, float * sq);

void getSqFromSrcData(float * x, float * y, const int len, const float y_prev, float * sq, float * K);
int getMinDeltaSq(float * x, float * y, const int start, const int len, const float y_prev, float * K_res, float * sq_res);

#endif
