#ifndef _DOUBLE2FLOAT_H
#define _DOUBLE2FLOAT_H


#ifdef GPGPU_CPP_AMP
#include <amp.h>
#include <amp_math.h>
#include <amprt.h >
using namespace concurrency;
#endif

class Double2Float
{
	double* d_;
	float* f;
	int num;
public:
	
	inline Double2Float(double*d, int n)
	{
		d_ = d;
		num = n;
		f = new float[n];
		for ( int i = 0; i < n; i++ )
		{
			f[i] = d[i];
		}
	}
	inline ~Double2Float()
	{
		delete [] f;
		f = 0;
	}

	inline void Restor(double* d) const
	{
		for ( int i = 0; i < num; i++ )
		{
			d[i] = f[i];
		}
	}
#ifdef GPGPU_CPP_AMP
	inline void Restor(double* d,  array_view<float, 1>& a)
	{
		for ( int i = 0; i < num; i++ )
		{
			d[i] = a[i];
		}
	}
	inline void Restor(array_view<float, 1>& a)
	{
		for ( int i = 0; i < num; i++ )
		{
			d_[i] = a[i];
		}
	}
#endif

	inline float* Pointer() const
	{
		return f;
	}
};

#ifdef GPGPU_CPP_AMP
inline void Restor(float* d,  int num, array_view<float, 1>& a)
{
	//parallel_for_each の呼び出し後に、array_view オブジェクトからデータにアクセスすると、
	//データはアクセラレータからホスト メモリにコピーされます。

	//for ( int i = 0; i < num; i++ )
	//{
	//	if ( Concurrency::fast_math::fabs( d[i] - a[i] ) > 0.00001 )
	//	{
	//		printf("###############################\n#");
	//	}
	//	d[i] = a[i];

	//}
}
#endif
#endif
