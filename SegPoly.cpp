#include "SegPoly.h"

ASegPoly obj;

float powMy(float val, int st)
{
	float tmp = 1;
	for (int i = 0; i < st; i++)
	{
		tmp *= val;
	}
	return tmp;
}

void NormData_v2(float *src, int src_num, float x_norm, float y_norm, float &v, float *x, float *y, float & k_p, float & k_m)
{
	int i;
	// Генерируем нормированный X
	for (i = 0; i < src_num; i++){
		x[i] = i*x_norm/(src_num - 1);
	}
	// Минимальное и максимальное значения
	float max_src = src[0];
	float min_src = src[0];
	for (i = 1; i < src_num; i++){
		if (src[i] > max_src)
			max_src = src[i];
		if (src[i] < min_src)
			min_src = src[i];
	}
	// Масштабный коэффициент
	k_m = y_norm/(max_src - min_src);
	// Коэффициент сдвига
	k_p = min_src;
	// Генерируем нормированный Y
	for (i = 0; i < src_num; i++){
		y[i] = (src[i] - k_p)*k_m;
	}
	// Начальное положение полинома
	v = (v - k_p)*k_m;
}
void PrepMtoGauss(float * R, float * T, int len2)
{
	// R - квадратная матрица, представленная линейно
	// R[i][j] = R[i + j*len2]
	float coeff;
	float * str = new float[len2];
	float val;
	for (int i = 0; i < (len2 - 1); i++){
		// Копируем i ую строку
		for ( int j = 0; j < len2; j++){
			str[j] = R[j + i*len2];
		}
		val = T[i];
		for (int j = (i+1); j < (len2); j++){
			// Рассчитываем коэффициенты для j ой строк
			coeff = R[i + j*len2]/R[i + i*len2];
			// Записываем данные
			for ( int k = 0; k < len2; k++){
				R[k + j*len2] = R[k + j*len2] - str[k]*coeff;
			}
			T[j] -= val*coeff;
		}
	}
}
void GaussSolveM(float * R, float * T, float * res, int len)
{
	res[len - 1] = T[len - 1]/R[len - 1 + (len - 1)*len];
	int k;
	float tmp;
	for ( int i = 1; i < len ; i++){
		// 1 < 3 2 < 3
		// обратный отсчет
		k = len - i - 1;
		// 4 - 1 - 1
		// 2 1 0
		tmp = T[k];
		for ( int j = (k + 1); j < len; j++){
			//2: 0 a1 a2 t
			//   02 12 22 2
			//0: 0 1 2 3
			//1: 4 5 6 7
			//2: 8 9 10 11
			//   3+2*4 = 11 | 3
			tmp -= R[j + k*len]*res[j];
		}
		res[k] = tmp/R[k + k*len];
	}
}
void GausCalc(float * M_a, float * M_t, int len, float * res)
{

	PrepMtoGauss(M_a, M_t, len);
	GaussSolveM(M_a, M_t, res, len);
}
void ExtrNullSeg_p5(float * x, float * y, int len, float v, float *K)
{
	int s_count = 11;
	int t_count = 6;
	float * s = new float[s_count];
	float * t = new float[t_count];
	float tmp;
	int i,j;
	float A0, A1, xn;
	float * A_2345 = new float[4];
	// Подгтотовка
	for (i = 0; i < s_count; i++){
		tmp = 0;
		for (j = 0; j < len; j++){
			tmp += powMy(x[j],i);
		}
		s[i] = tmp;
	}
	for (i = 0; i < t_count; i++){
		tmp = 0;
		for (j = 0; j < len; j++){
			tmp += y[j]*powMy(x[j],i);
		}
		t[i] = tmp;
	}
	// Известные условия
	A0 = v;
	A1 = 0;
	xn = x[len - 1];
	int Mlen = 4;
	float * M = new float[Mlen*Mlen];
	float * T = new float[Mlen];
	// Формируем матрицу коэффициентов
	M[0]   = 2*xn;  M[1]  = 3*xn*xn;  M[2]   = 4*xn*xn*xn;  M[3] = 5*xn*xn*xn*xn;
	M[4]   = s[5];  M[5]  = s[6];  M[6]   = s[7];  M[7] = s[8];
	M[8]   = s[6];  M[9]  = s[7];  M[10]  = s[8];  M[11] = s[9];
	M[12]  = s[7];  M[13] = s[8];  M[14]  = s[9];  M[15] = s[10];
	// Вектор значений
	T[0] = 0;
	T[1] = t[3] - s[3]*A0;
	T[2] = t[4] - s[4]*A0;
	T[3] = t[5] - s[5]*A0;
	GausCalc(M, T, Mlen, A_2345);
	K[0] = A0;
	K[1] = A1;
	K[2] = A_2345[0];
	K[3] = A_2345[1];
	K[4] = A_2345[2];
	K[5] = A_2345[3];
}
void GenPolyN(float * K, int K_len, float * x, float * y, int len)
{
	float tmp;
    /* K[0]*1  + K[1]*x + K[2]*x^2 + ... */
	for ( int i = 0; i < len; i++){
		tmp = K[0];
		for ( int j = 1; j < K_len; j++){
            float xi = 1;
            for (int p = 0; p < j; p++)
                xi *= x[i];
            tmp += K[j]*xi;
		}
		y[i] = tmp;
	}
}
void CalcSqMorePt(float * y, int y_num, float * A, int A_num, int pt_count, float & sum)
{
	sum = 0;
	float y_pt, res, val_y, tmp, val_poly;
	int x1, x2;
	for ( int i = 1; i < pt_count; i++){
		y_pt = (i - 1)*((float)y_num - 1)/((float)pt_count - 1);
		// Ниже нужно получить целую часть числа
		//modf((double)y_pt, (double*)&res);
		//x1 = (int) res;
		x1 = (int) y_pt;
		x2 = x1 + 1;
		val_y = y[x1] + (y[x2] - y[x1])*(y_pt - (float)x1)/ ((float)x2 - (float)x1);
		tmp = A[0];
		for ( int j = 1; j < A_num; j++){
			tmp += A[j]*powMy(y_pt, j);
		}
		val_poly = tmp;
		sum += (val_y - val_poly)*(val_y - val_poly);
	}
	y_pt = (float)y_num;
	val_y = y[y_num - 1];
	val_poly = A[0] + A[1]*y_pt + A[2]*y_pt*y_pt + A[3]*y_pt*y_pt*y_pt + A[4]*y_pt*y_pt*y_pt*y_pt + A[5]*y_pt*y_pt*y_pt*y_pt*y_pt;
	sum += (val_y - val_poly)*(val_y - val_poly);
}
void ApproxData(int st, int en, float x_len, float y_len, float y_prev)
{
	// if (SG_LOG_ON){
	// 	printf("Start approx data st = %d, en = %d \n", st,en);
	// 	printf("mass[0] = %f\n",obj.mass[0] );
	// }
	int len = en - st;
	for ( int i = 0; i < len; i++){
		obj.mass_tmp[i] = obj.mass[st+i];
	}
	float k_p, k_m;
	float * x = new float[len];
	float * y = new float[len];
	// if (SG_LOG_ON){
	// 	printf("Normalize\n");
	// }
	NormData_v2(obj.mass_tmp, len, x_len, y_len, y_prev, x, y, k_p, k_m);
	for ( int i = 0; i < len; i ++){
		obj.norm_res[i] = y[i];
	}
	// if (SG_LOG_ON){
	// 	printf("Find extremum\n");
	// }
	ExtrNullSeg_p5(x, y, len, y_prev, obj.coeff_res);
	//delete []obj.approx_res;
	//obj.approx_res = new float[len];
	// if (SG_LOG_ON){
	// 	printf("Generate poly\n");
	// }
	GenPolyN(obj.coeff_res, obj.coeff_res_num, x, obj.approx_res, len);
	//obj.approx_res_rn = new float[len];
	for ( int i = 0; i < len; i ++){
		obj.approx_res_rn[i] = obj.approx_res[i]/k_m + k_p;
	}
	int pt_count = 100;
	// if (SG_LOG_ON){
	// 	printf("Calculate sq\n");
	// }
	CalcSqMorePt(y, len, obj.coeff_res, obj.coeff_res_num, pt_count, obj.d_res);
}
void CreateSegPoly( float * str, int str_num)
{
	// if (SG_LOG_ON){
	// 	printf("Creating variables\n");
	// }
	obj.coeff_res = new float[obj.coeff_res_num];
	obj.flag_cond = 0;
	obj.mass = new float[str_num];
	obj.mass_tmp = new float[str_num];
	obj.norm_res = new float[str_num];
	obj.approx_res = new float[str_num];
	obj.approx_res_rn = new float[str_num];
	obj.res_approx = new float[str_num];
	obj.seg = new int[str_num];
	obj.seg_coeff = new float[str_num*obj.coeff_res_num];
	//obj.res_approx = new float[str_num];
	for ( int i = 0; i < str_num; i++){
		obj.mass[i] = str[i];
	}
	obj.mass_num = str_num;
}
void InitSet()
{
	obj.flag_cond = 0;
	// delete [] obj.res_approx;
}
void CompareD()
{
	if (obj.flag_cond == 0){
		obj.d_min = obj.d_res;
		obj.flag_cond = 1;
	}
	else{
		if (obj.d_res < obj.d_min){
			obj.d_min = obj.d_res;
			obj.flag_cond = 2;
		}
		else{
			obj.flag_cond = 1;
		}
	}
}
void GetCoeff(int st, int en, float y_prev)
{
	int len = en - st;
	float * x = new float[len];
	float * y = new float[len];
	for ( int i = 0; i < len; i++){
		x[i] = (float)i;
		y[i] = obj.mass[st+i];
	}
	ExtrNullSeg_p5(x, y, len, y_prev, obj.coeff_res);
}
void GetSegment(int mincount, int step, float x_len, float y_len, int stat)
{
	int flag = 1; 	// контроль цикла
	int st = 0;		// начальный элемент
	int j_seg = 0;		// счетчик сегментов
	obj.seg[j_seg] = 0;
	float y_prev = obj.mass[0];
	int imin;
	float * approx_min = new float [obj.mass_num];
	// if (SG_LOG_ON){
	// 	printf("Start segmentation\n");
	// }
	while (flag)
	{
		imin = st + mincount - 1;
		
		InitSet();
		int i;
		// if (SG_LOG_ON){
		// 	printf("Search %d segment\n", (j_seg + 1));
		// }
		for ( i = mincount; i < step; i++)
		{
			ApproxData(st, (st + i - 1), x_len, y_len, y_prev);
			if (i == mincount)
			{
				for ( int j = 0; j < mincount; j++){
					approx_min[j] = obj.approx_res_rn[j];
				}
			}
			CompareD();
			if (obj.flag_cond == 2)
			{
				imin = st + i - 1;
				for ( int j = 0; j < imin; j++)
				{
					approx_min[j] = obj.approx_res_rn[j];
				}
			}
		}
		j_seg ++;
		// if (SG_LOG_ON){
		// 	printf("Found segment %d [%d, %d]\n", (j_seg), st, imin );
		// }
		obj.seg[j_seg] = imin;
		// if (SG_LOG_ON){
		// 	printf("Write coeff\n");
		// }
		GetCoeff(st, imin, y_prev);
		for ( int j = 0; j < obj.coeff_res_num; j++){
			obj.seg_coeff[j + j_seg*obj.coeff_res_num] = obj.coeff_res[j];
		}
		// if (SG_LOG_ON){
		// 	printf("Write approx data\n");
		// }
		for ( int j = st; j < imin; j++){
			obj.res_approx[j] = approx_min[j - st];
		}
		// if (SG_LOG_ON){
		// 	printf("Get y_prev\n");
		// }
		if (stat == 1){
			y_prev = obj.mass[imin];
		}
		if (stat == 2){
			y_prev = approx_min[imin - st - 1];
		}
		//delete []approx_min;
		st = imin;
		// if (SG_LOG_ON){
		// 	printf("End iteration\n");
		// }
		if ((st + step) > obj.mass_num){
			flag = 0;
			// if (SG_LOG_ON){
			// 	printf("Searching segments over\n");
			// }
		}
	}
	obj.seg_num = j_seg;
}
void GetSegmentResultCount( int & seg_count)
{
	seg_count = obj.seg_num;
}
void GetSegmentResultData( int * seg_result)
{
	for (int i = 0; i < obj.seg_num; i++){
		seg_result[i] = obj.seg[i];
	}
}
void GenPoly3(float * K, float * x, float * y, int len)
{
    /* K[0]*1  + K[1]*x + K[2]*x^2 + ... */
    for ( int i = 0; i < len; i++){
        float xi = x[i];
        y[i] = K[0] + K[1]*xi + K[2]*xi*xi + K[3]*xi*xi*xi;
    }
}
void GenPoly3_1(float *K, float *x, float *y, const int st, const int len)
{
    for ( int i = 0; i < len; i++){
        float xi = x[st + i] - x[st];
        y[i] = K[0] + K[1]*xi + K[2]*xi*xi + K[3]*xi*xi*xi;
    }
}
void ExtrNullSeg_v2(float * K, float * x, float * y, const float y_prev, const int count)
{
	// const int s_count = 5;
	// const int t_count = 2;
	//float * s = (float*)malloc(s_count);
	//float * t = (float*)malloc(t_count);
	float s[S_COUNT];
	float t[T_COUNT];
	for (int i = 0; i < S_COUNT; i++)
	{
		float tmp = 0;
		for (int j = 0; j < count; j++)
		{
			float xj = 1;
			for (int k = 0; k < i; k++)
			{
				xj *= x[j];
			}
			tmp = tmp + xj;
		}
		s[i] = tmp;
	}
	for (int i = 0; i < T_COUNT; i++)
	{
		float tmp = 0;
		for (int j = 0; j < count; j++)
		{
			float xj = 1;
			for (int k = 0; k < i; k++)
			{
				xj *= x[j];
			}
			tmp = tmp + y[j]*xj;
		}
		t[i] = tmp;
	}
	K[0] = y_prev;
	K[1] = 0;
    K[2] = 0;   /* init */
//    qDebug() << "calc:";
//    qDebug() << s[2];
//    qDebug() << s[3];
//    qDebug() << s[4];
//    qDebug() << s[5];
	K[3] = (t[2] - K[0]*s[2])/(s[5] - 3.0f/2.0f*s[4]*x[count - 1]);
    K[2] = -3.0f/2.0f*K[3]*x[count - 1]; /* value */
}
void ExtrNullSeg_v2_1(float *K, float *x, float *y, const float y_prev, const int st, const int count)
{
    float s[S_COUNT];
    float t[T_COUNT];
    for (int i = 0; i < S_COUNT; i++)
    {
        float tmp = 0;
        for (int j = 0; j < count; j++)
        {
            float xj = 1;
            for (int k = 0; k < i; k++)
            {
                xj *= x[st + j] - x[st];
            }
            tmp = tmp + xj;
        }
        s[i] = tmp;
    }
    for (int i = 0; i < T_COUNT; i++)
    {
        float tmp = 0;
        for (int j = 0; j < count; j++)
        {
            float xj = 1;
            for (int k = 0; k < i; k++)
            {
                xj *= x[st + j] - x[st];
            }
            tmp = tmp + y[st + j]*xj;
        }
        t[i] = tmp;
    }
    K[0] = y_prev;
    K[1] = 0;
    K[2] = 0;   /* init */
    K[3] = (t[2] - K[0]*s[2])/(s[5] - 3.0f/2.0f*s[4]*(x[st + count - 1] - x[st]));
    K[2] = -3.0f/2.0f*K[3]*(x[st + count - 1] - x[st]); /* value */
}
void ExtrNullSeg_v2_2(float *K, float *x, float *y, const float y_prev, const int st, const int count)
{
    float t2 = 0,s2 = 0,s4 = 0,s5 = 0;
    for (int j = 0; j < count; j++)
    {
        float xj = x[st + j] - x[st];
        float xjxj = xj*xj;
        t2 += y[st + j]*xj*xj;
        s2 += xjxj;
        s4 += xjxj*xjxj;
        s5 += xjxj*xjxj*xj;
    }
    K[0] = y_prev;
    K[1] = 0;
    K[2] = 0;   /* init */
    K[3] = (t2 - K[0]*s2)/(s5 - 3.0f/2.0f*s4*(x[st + count - 1] - x[st]));
    K[2] = -3.0f/2.0f*K[3]*(x[st + count - 1] - x[st]); /* value */
}
// function [ K ] = ExtrNullSeg_v2( x, y, y_prev )
// %EXTRNULLSEG Âîçâðàùàåò êîýôôèöèåíòû àïïðîêñèìàöèè äàííûõ ïîëèíîìîì
// %   Ïîëèíîì èìååò ýêñòðåìóìû â íà÷àëå è êîíöå ñåãìåíòà
// [~,n] = size(y);
// s_count = 5;
// t_count = 2;
// s = nan(1,(s_count+1));
// t = nan(1,(t_count+1));
// for i = 0:s_count
//     tmp = 0;
//     for j = 1:n
//         tmp = tmp + x(j)^i;
//     end
//     s(i+1) = tmp;
// end
// for i = 0:t_count
//     tmp = 0;
//     for j = 1:n
//         tmp = tmp + y(j)*x(j)^i;
//     end
//     t(i+1) = tmp;
// end
// A0 = y_prev;
// A1 = 0;
// A3 = (t(3) - A0*s(3))/(s(6) - 3/2*s(5)*x(n));
// A2 = -3/2*A3*x(n);
// K(1) = A0;
// K(2) = A1;
// K(3) = A2;
// K(4) = A3;
// end
void calcDeltaPoly3(float *K, float *x, float *y, const int len, float *sq)
{
    /*
     * K0 + K1x + K2x2 + K3x3
     * kx + b
     * S(K0 + K1x + K2x^2 + K2x^3 - kx - b)
     * (K0x2 + K1x2^2/2 + K2x2^3/3 + K3x2^4/4 - kx2^2/2 - bx2) - ( K0x1 + ...
     *
     *
     * kx1 + b = y1
     * kx2 + b = y2
     * k(x2 - x1) = y2 - y1
     * k = (y2 - y1)/(x2 - x1)
     * b = y1 - kx1
     */

    /* перебор пар точек */
    *sq = 0;
    for (int i = 0; i <(len - 1);i++)
    {
        float p = (y[i+1] - y[i])/(x[i+1] - x[i]);
        float b = y[i] - p*x[i];
        float delta = (K[0]*x[i+1] + K[1]/2*x[i+1]*x[i+1] + K[2]/3*x[i+1]*x[i+1]*x[i+1] + K[3]/4*x[i+1]*x[i+1]*x[i+1]*x[i+1] - p/2*x[i+1]*x[i+1] - b*x[i+1]);
        delta -= K[0]*x[i] + K[1]/2*x[i]*x[i] + K[2]/3*x[i]*x[i]*x[i] + K[3]/4*x[i]*x[i]*x[i]*x[i] - p/2*x[i]*x[i] - b*x[i];
        if (delta < 0)
            *sq -= delta;
        else
            *sq += delta;
    }
}
void calcDeltaPoly3_1(float *K, float *x, float *y, const int st, const int len, float *sq)
{
    *sq = 0;
    float p, b;
    float xi1, xi1xi1;
    float xi, xixi;
    for (int i = 0; i <(len - 1);i++)
    {
        p = (y[st + i+1] - y[st + i])/(x[st + i+1] - x[ st + i]);
        b = y[st + i] - p*(x[st + i] - x[st]);
        xi1 = x[st + i + 1] - x[st];
        xi1xi1 = xi1*xi1;
        xi = x[st + i] - x[st];
        xixi = xi*xi;
        float delta = (K[0]*xi1 + K[1]/2*xi1xi1 + K[2]/3*xi1*xi1xi1 + K[3]/4*xi1xi1*xi1xi1 - p/2*xi1xi1 - b*xi1);
        delta -= K[0]*xi + K[1]/2*xixi + K[2]/3*xixi*xi + K[3]/4*xixi*xixi - p/2*xixi - b*xi;
        if (delta < 0)
            *sq -= delta;
        else
            *sq += delta;
    }
}
void getSqFromSrcData(float *x, float *y, const int len, const float y_prev, float *sq, float * K)
{
    ExtrNullSeg_v2(K, x, y, y_prev, len);
    calcDeltaPoly3(K, x, y, len, sq);
}
int getMinDeltaSq(float *x, float *y, const int start, const int len, const float y_prev, float *K_res, float *sq_res)
{
    float K[4];
    float sq, sq_min;
    int i_min;
    for (int i = start; i < len; i++)
    {
        ExtrNullSeg_v2(K,x,y,y_prev,i);
        calcDeltaPoly3(K,x,y,i,&sq);
        sq = sq/i;
        if (i == start)
        {
            sq_min = sq;
            i_min = i;
        }
        else
        {
            if (sq <= sq_min)
            {
                sq_min = sq;
                i_min = i;
            }
        }
    }
    *sq_res = sq;
    K_res[0] = K[0];
    K_res[1] = K[1];
    K_res[2] = K[2];
    K_res[3] = K[3];
    return i_min;
}
