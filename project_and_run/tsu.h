#ifndef TSU_H__

#define TSU_H__

//#pragma warning ( disable : 4996 )の代わり
#define _CRT_SECURE_NO_DEPRECATE 

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>

#define OMP_SCHEDULE schedule(guided)
//#define OMP_SCHEDULE schedule(static)
#include "bitmap.h"
#include "csv2obj.h"

#include "geo.h"
#include <vector>
#include <string>

#include "CsvReadWrite.hpp"
#include "utf8_printf.hpp"

#include "Storm.h"

#define USE_ARRAY_POINTER

#ifdef USE_ARRAY_POINTER
#include "tsu_macro.h"
#include "diff_cpu.h"
#endif

//#define TSU_USE_ACC
//#define TSU_USE_GPU

#include <amp.h>
#include <amp_math.h>
using namespace concurrency;
#ifdef TSU_USE_GPU
#define TSU_ACC_RESTRICTION restrict(amp)
#define ACC_TYPE "GPU"
#define FAST_MATH	fast_math::
#else if
#define TSU_ACC_RESTRICTION restrict(cpu)
#define ACC_TYPE "CPU"
#endif

#define GOOGLE_MAP_DATA_COPY_LIGHT	"(c)Google, Map Data (c) TerraMetrics"

//穴埋め・補間
#define IS_TOPO_FILL(rgb)	(rgb.r==255 && rgb.g >=102 && rgb.g <=104 && rgb.b == 0)
#define TOPO_FILL_FLAG(rgb)	(rgb.r)
#define TOPO_FILL_FLAG_SET(rgb, v)	(rgb.r=v)

//河川経路
#define IS_RIVER_PATH(rgb)	(rgb.r==113 && rgb.g >=100 && rgb.g <=201 && rgb.b == 255)

//川上の流入位置
#define IS_INFLOW(rgb) (rgb.r==255 && rgb.g==255 && rgb.b <= 100)

//堤防（障害壁）
#define IS_BARIR(rgb)	(rgb.r==255 && rgb.g < 100 && rgb.b == 255)

//堤防生成しない印
#define NO_CONFLUENCE(rgb)	(rgb.r==255 && rgb.g==200 && rgb.b == 255)

//侵入不可領域
#define IS_INVIOLABILITY(rgb)	(rgb.r == 255 && rgb.g == 102 && rgb.b == 0)

#define TO_LAND(rgb)	(rgb.r==255 && rgb.g==0 && rgb.b == 0)
#define TO_WATER(rgb)	(rgb.r==0 && rgb.g==0 && rgb.b == 255)



#define CDIFFx(w,n,W,i,j) do{			\
	int _k=0;							\
	for (int _i = n; _i >= 1; _i-- )	\
	{									\
		w[_k] = W(i,j-_i);				\
		_k++;							\
	}									\
	for (int _i = 1; _i <= n; _i++ )	\
	{									\
		w[_k] = W(i,j+_i);				\
		_k++;							\
	}									\
}while(0)

#define CDIFFy(w,n,W,i,j) do{			\
	int _k=0;							\
	for (int _i = n; _i >= 1; _i-- )	\
	{									\
		w[_k] = W(i-_i,j);				\
		_k++;							\
	}									\
	for (int _i = 1; _i <= n; _i++ )	\
	{									\
		w[_k] = W(i+_i,j);				\
		_k++;							\
	}									\
}while(0)

//#define WENO
inline double WENO1(double* f);

#define RAD		(57.295779513082320876846364344191)		//(180.0/M_PI)

#define C1_03	(0.33333333333333333333333333333333)	//1/3
#define C7_03	(2.3333333333333333333333333333333)		//7/3
#define C4_03	(1.3333333333333333333333333333333)		//4/3
#define C2_03	(0.66666666666666666666666666666667)		//2/3


#define R_HOUR	(2.7777777777777777777777777777778e-4)	//1/3600
#define R_MIN	(0.01666666666666666666666666666667)	//1/60

#define C1_06	(0.16666666666666666666666666666667)	// 1/6
#define C1_08	(0.12500000000000000000000000000000)	// 1/8
#define C1_12	(0.08333333333333333333333333333333)	// 1/12
#define C1_60	(0.01666666666666666666666666666667)	// 1/60
#define C1_90	(0.01111111111111111111111111111111)	// 1/60
#define C1_180	(0.00555555555555555555555555555556)	// 1/180
#define C1_240	(0.00416666666666666666666666666667)	// 1/240
#define C1_840	(0.00119047619047619047619047619048)	// 1/840
#define C1_2520	(3.968253968253968253968253968254e-4)	// 1/840
#define C1_5040	(1.984126984126984126984126984127e-4)	// 1/5040

//d=0.01付近で０になってdmin以上で1.0になる係数を返す
inline double AdvectionCuttOff(const double d)
{
	const double dmin = 0.3;
	const double exp_1 = 0.6321205588285577;	//(1.0 - exp(-1.0))
	return min(1.0, (1.0 - exp(-pow(d / 0.3, 6))) / (exp_1));
}

inline double CentralDiff(const int odr, const double* f);
inline double CentralDiff2(const int odr, const double* f);
inline double CentralDiff3(const int odr, const double* f);
inline double ForwardDiff(const int odr, const double* f);
inline double BackwardDiff(const int odr, const double* f);


#define BIT(num)                 ((unsigned char)1 << (num))

#define GAP_1		2	//(BIT(1))
#define GAP_2		4	//(BIT(2))
#define GAP_3		8	//(BIT(3))
#define GAP_4		16	//(BIT(4))
#define GAP_6		64	//(BIT(6))
#define GAP_7		128	//(BIT(7))

#define IS_WATER	(0)
#define IS_WET		(1)
#define IS_DRY		(2)
#define IS_OVERFLOW	(3)

//移流項を適用する最大水深
#define ADVECTION_TERM_DEPTH	100.0

//差分計算で連続性が保持できないほどのギャップのマーキング
//メッシュサイズからの割合で決定
#define GAP_MAX		0.20			//無視出来ないギャップ高がとみなす最低高さ係数(地形修正)
#define GAP_DEPTH	20.0			//ギャップ識別最低水深係数

#define ADVECTION_TERM_GAP_CUTAMX	(2.0)	//移流項構成項の異常値をカットする。

//急勾配
#define STEEP_SLOPE1_1	(200.0)	//スロープの最大水深1
#define STEEP_SLOPE1_2	(30.0)	//最大段差1
#define STEEP_SLOPE2_1	(30.0)	//スロープの最大水深2
#define STEEP_SLOPE2_2	(15.0)	//最大段差2

//地形修正
#define EDIT_ELV_HEIGHT_LIM	(-15.0)	//標高下限値
#define EDIT_WTR_DEPTH  (0.5)	//これ以上なら陸地とみなす

//断層破壊発生の最終最大時間
#define MAX_FAULT_TIMES	(21600)	//6時間

//仮想水深法
#define VIRTUAL_DEPTH_SIGMA	(0.000005)

//非線形計算で全水位が#cm以下のときはDを分母の持つ移流項を省略する
#define D_UNDER_LIMIT	(0.00001)

//流れ量（絶対値）は境界水位の#倍を上限とする
//#define MN_UPPER_LIMIT1	(8.5)
//#define MN_UPPER_LIMIT2	(15.0)
//#define MN_UPPER_LIMIT1	(8.5)
//#define MN_UPPER_LIMIT2	(6.0)
//#define MN_UPPER_LIMIT1	(30.0)
//#define MN_UPPER_LIMIT2	(30.0)
#define MN_UPPER_LIMIT1	(50.0)
#define MN_UPPER_LIMIT2	(70.0)	//(70.0)



//#define MN_UPPER_LIMIT3	(35.0)	//遡上+陸地境界付近
//#define MN_UPPER_LIMIT4	(35.0)	//遡上+陸地境界付近
//
//#define MN_UPPER_LIMIT5	(35.0)	//遡上+陸地
//#define MN_UPPER_LIMIT6	(35.0)	//遡上+陸地
//
//#define MN_UPPER_LIMIT7	(35.0)	//陸地境界付近
//#define MN_UPPER_LIMIT8	(35.0)	//陸地境界付近


#define MN_FLOW_RESTRICTION_COEF	(1.00)	//流量制限
#define MN_FLOW_RESTRICTION_COEF2	(0.13)	//流量制限
#define MN_FLOW_RESTRICTION_COEF3	(0.32)	//流量制限

#define ELVBOUNDARY_WIDTH1	25	//遡上+陸地境界付近の検査幅
#define ELVBOUNDARY_WIDTH2	25	//遡上+陸地境界付近の検査幅
#define ELVBOUNDARY_WIDTH3	25	//陸地境界付近の検査幅


///////////////////////////////移流項の計算異常値の抑制パラメータ
#define ADVECTION_TERM_LIMIT	(0.25)	//移流項の大きさ制限(局所項にたいしての比率）

///////////////////////////////分散項の計算異常値の抑制パラメータ
#define DISPERSIN_TERM_LIMIT	(0.2)	//分散項の大きさ制限1(局所項+移流項にたいしての比率）

#define DISPERSIN_TERM_LIMIT2	(0.50)	//分散項適用条件（波高/水深比　がこの値以上の時)

#define DISPERSIN_TERM_COEF		(0.75)	//(0.75)

//流量フラックスの制限関数
#define	FLOW_DEPTH_LIMIT_COND_COEF1	(45.0)
#define	FLOW_DEPTH_LIMIT_COND_COEF2	(0.30)
#define	FLOW_DEPTH_LIMIT_COND_COEF3	(150.0)	// 4.0以上


//#define FLOW_LIMIT_DEBUG


//数値振動（チェッカーボード不安定）閾値
#define CHECKER_BORD_THESHOLD1	(1.96)	//チェッカーボード模様との偏差（小さい程合致) 1.0,1.1,1.2=NG
#define CHECKER_BORD_THESHOLD11	(0.95)	
//3.5位だとチェッカーボード模様に関係なく変化のある場所が合致する。
#define CHACKER_BORD_THESHOLD1_1	(0.01)	//チェッカーボード模様との偏差（小さい程合致)


#define CHECKER_BORD_THESHOLD2	(0.00001)	//(0.0022)	//チェッカーボード範囲での数値振動範囲
//#define CHACKER_BORD_DEBUG		1	//チェッカーボード検出
//#define CHACKER_BORD_DEBUG2		1	//チェッカーボード検)出=>除去


//数値振動除去サイクル
/*
   2     3     5     7    11    13    17    19    23    29
   31    37    41    43    47    53    59    61    67    71
   73    79    83    89    97   101   103   107   109   113
  127   131   137   139   149   151   157   163   167   173
  179   181   191   193   197   199   211   223   227   229
  233   239   241   251   257   263   269   271   277   281
  283   293   307   311   313   317   331   337   347   349
  353   359   367   373   379   383   389   397   401   409
  419   421   431   433   439   443   449   457   461   463
  467   479   487   491   499   503   509   521   523   541
  547   557   563   569   571   577   587   593   599   601
  607   613   617   619   631   641   643   647   653   659
  661   673   677   683   691   701   709   719   727   733
  739   743   751   757   761   769   773   787   797   809
  811   821   823   827   829   839   853   857   859   863
  877   881   883   887   907   911   919   929   937   941
  947   953   967   971   977   983   991   997  1009  1013

1019 
1021 1031 1033 1039 1049 1051 1061 1063 1069 1087 1091 1093 
1097 1103 1109 1117 1123 1129 1151 1153 1163 1171 1181 1187 
1193 1201 1213 1217 1223 1229 1231 1237 1249 1259 1277 1279 
1283 1289 1291 1297 1301 1303 1307 1319 1321 1327 1361 1367 
1373 1381 1399 1409 1423 1427 1429 1433 1439 1447 1451 1453 
1459 1471 1481 1483 1487 1489 1493 1499 1511 1523 1531 1543 
1549 1553 1559 1567 1571 1579 1583 1597 1601 1607 1609 1613 
1619 1621 1627 1637 1657 1663 1667 1669 1693 1697 1699 1709 
1721 1723 1733 1741 1747 1753 1759 1777 1783 1787 1789 1801 
1811 1823 1831 1847 1861 1867 1871 1873 1877 1879 1889 1901 
1907 1913 1931 1933 1949 1951 1973 1979 1987 1993 1997 1999 
2003 2011 2017 2027 2029 2039 2053 2063 2069 2081 2083 2087 
2089 2099 2111 2113 2129 2131 2137 2141 2143 2153 2161 2179 
2203 2207 2213 2221 2237 2239 2243 2251 2267 2269 2273 2281 
2287 2293 2297 2309 2311 2333 2339 2341 2347 2351 2357 2371 
2377 2381 2383 2389 2393 2399 2411 2417 2423 2437 2441 2447 
2459 2467 2473 2477 2503 2521 2531 2539 2543 2549 2551 2557 
2579 2591 2593 2609 2617 2621 2633 2647 2657 2659 2663 2671 
2677 2683 2687 2689 2693 2699 2707 2711 2713 2719 2729 2731 
2741 2749 2753 2767 2777 2789 2791 2797 2801 2803 2819 2833 
2837 2843 2851 2857 2861 2879 2887 2897 2903 2909 2917 2927 
2939 2953 2957 2963 2969 2971 2999 3001 3011 3019 3023 3037 
3041 3049 3061 3067 3079 3083 3089 3109 3119 3121 3137 3163 
3167 3169 3181 3187 3191 3203 3209 3217 3221 3229 3251 3253 
3257 3259 3271 3299 3301 3307 3313 3319 3323 3329 3331 3343 
3347 3359 3361 3371 3373 3389 3391 3407 3413 3433 3449 3457 
3461 3463 3467 3469 3491 3499 3511 3517 3527 3529 3533 3539 
3541 3547 3557 3559 3571 3581 3583 3593 3607 3613 3617 3623 
3631 3637 3643 3659 3671 3673 3677 3691 3697 3701 3709 3719 
3727 3733 3739 3761 3767 3769 3779 3793 3797 3803 3821 3823 
3833 3847 3851 3853 3863 3877 3881 3889 3907 3911 3917 3919 
3923 3929 3931 3943 3947 3967 3989 4001 4003 4007 4013 4019 
4021 4027 4049 4051 4057 4073 4079 4091 4093 4099 4111 4127 
4129 4133 4139 4153 4157 4159 4177 4201 4211 4217 4219 4229 
4231 4241 4243 4253 4259 4261 4271 4273 4283 4289 4297 4327 
4337 4339 4349 4357 4363 4373 4391 4397 4409 4421 4423 4441 
4447 4451 4457 4463 4481 4483 4493 4507 4513 4517 4519 4523 
4547 4549 4561 4567 4583 4591 4597 4603 4621 4637 4639 4643 
4649 4651 4657 4663 4673 4679 4691 4703 4721 4723 4729 4733 
4751 4759 4783 4787 4789 4793 4799 4801 4813 4817 4831 4861 
4871 4877 4889 4903 4909 4919 4931 4933 4937 4943 4951 4957 
4967 4969 4973 4987 4993 4999 5003 5009 5011 5021 5023 5039 
5051 5059 5077 5081 5087 5099 5101 5107 5113 5119 5147 5153 
5167 5171 5179 5189 5197 5209 5227 5231 5233 5237 5261 5273 
5279 5281 5297 5303 5309 5323 5333 5347 5351 5381 5387 5393 
5399 5407 5413 5417 5419 5431 5437 5441 5443 5449 5471 5477 
5479 5483 5501 5503 5507 5519 5521 5527 5531 5557 5563 5569 
5573 5581 5591 5623 5639 5641 5647 5651 5653 5657 5659 5669 
5683 5689 5693 5701 5711 5717 5737 5741 5743 5749 5779 5783 
5791 5801 5807 5813 5821 5827 5839 5843 5849 5851 5857 5861 
5867 5869 5879 5881 5897 5903 5923 5927 5939 5953 5981 5987 
6007 6011 6029 6037 6043 6047 6053 6067 6073 6079 6089 6091 
6101 6113 6121 6131 6133 6143 6151 6163 6173 6197 6199 6203 
6211 6217 6221 6229 6247 6257 6263 6269 6271 6277 6287 6299 
6301 6311 6317 6323 6329 6337 6343 6353 6359 6361 6367 6373 
6379 6389 6397 6421 6427 6449 6451 6469 6473 6481 6491 6521 
6529 6547 6551 6553 6563 6569 6571 6577 6581 6599 6607 6619 
6637 6653 6659 6661 6673 6679 6689 6691 6701 6703 6709 6719 
6733 6737 6761 6763 6779 6781 6791 6793 6803 6823 6827 6829 
6833 6841 6857 6863 6869 6871 6883 6899 6907 6911 6917 6947 
6949 6959 6961 6967 6971 6977 6983 6991 6997 7001 7013 7019 
7027 7039 7043 7057 7069 7079 7103 7109 7121 7127 7129 7151 
7159 7177 7187 7193 7207 7211 7213 7219 7229 7237 7243 7247 
7253 7283 7297 7307 7309 7321 7331 7333 7349 7351 7369 7393 
7411 7417 7433 7451 7457 7459 7477 7481 7487 7489 7499 7507 
7517 7523 7529 7537 7541 7547 7549 7559 7561 7573 7577 7583 
7589 7591 7603 7607 7621 7639 7643 7649 7669 7673 7681 7687 
7691 7699 7703 7717 7723 7727 7741 7753 7757 7759 7789 7793 
7817 7823 7829 7841 7853 7867 7873 7877 7879 7883 7901 7907 
7919 7927 7933 7937 7949 7951 7963 7993 8009 8011 8017 8039 
8053 8059 8069 8081 8087 8089 8093 8101 8111 8117 8123 8147 
8161 8167 8171 8179 8191 8209 8219 8221 8231 8233 8237 8243 
8263 8269 8273 8287 8291 8293 8297 8311 8317 8329 8353 8363 
8369 8377 8387 8389 8419 8423 8429 8431 8443 8447 8461 8467 
8501 8513 8521 8527 8537 8539 8543 8563 8573 8581 8597 8599 
8609 8623 8627 8629 8641 8647 8663 8669 8677 8681 8689 8693 
8699 8707 8713 8719 8731 8737 8741 8747 8753 8761 8779 8783 
8803 8807 8819 8821 8831 8837 8839 8849 8861 8863 8867 8887 
8893 8923 8929 8933 8941 8951 8963 8969 8971 8999 9001 9007 
9011 9013 9029 9041 9043 9049 9059 9067 9091 9103 9109 9127 
9133 9137 9151 9157 9161 9173 9181 9187 9199 9203 9209 9221 
9227 9239 9241 9257 9277 9281 9283 9293 9311 9319 9323 9337 
9341 9343 9349 9371 9377 9391 9397 9403 9413 9419 9421 9431 
9433 9437 9439 9461 9463 9467 9473 9479 9491 9497 9511 9521 
9533 9539 9547 9551 9587 9601 9613 9619 9623 9629 9631 9643 
9649 9661 9677 9679 9689 9697 9719 9721 9733 9739 9743 9749 
9767 9769 9781 9787 9791 9803 9811 9817 9829 9833 9839 9851 
9857 9859 9871 9883 9887 9901 9907 9923 9929 9931 9941 9949 
9967 9973 10007 10009 10037 10039 10061 10067 10069 10079
*/

#define FILTER_CYCLE1	9999943		//遡上+陸地境界付近
#define FILTER_CYCLE2	9999971		//遡上+陸地
//
//#define FILTER_CYCLE3	9999973		//浅瀬+全体
//#define FILTER_CYCLE4	9999991		//全体
/*
#define FILTER_CYCLE1	19		//遡上+陸地境界付近
#define FILTER_CYCLE2	181		//遡上+陸地

#define FILTER_CYCLE3	25		//浅瀬+全体
#define FILTER_CYCLE4	200		//全体

#define FILTER_STEP1	3		//浅瀬+指定時間の少し前から徐々に振動を除去する
#define FILTER_STEP2	3		//全体+指定時間の少し前から徐々に振動を除去する
*/

//#define FILTER_CYCLE1	19		//遡上+陸地境界付近
//#define FILTER_CYCLE2	181		//遡上+陸地

/*
#define FILTER_CYCLE3	19		//浅瀬+全体
#define FILTER_CYCLE4	199		//全体
*/
#define FILTER_CYCLE3	37		//浅瀬+全体
#define FILTER_CYCLE4	19		//全体

#define FILTER_STEP1	3		//浅瀬+指定時間の少し前から徐々に振動を除去する
#define FILTER_STEP2	3		//全体+指定時間の少し前から徐々に振動を除去する

//Scale-space and edge detection using anisotropic diffusion(1990 perona & malik)パラメータ
#define PERONA_MALIK_SCALE_PRAM1	(0.006*0.006)	//浅瀬+全体
#define PERONA_MALIK_SCALE_PRAM2	(0.006*0.006)	//全体
#define PERONA_MALIK_SCALE_PRAM3	(0.007*0.007)	//遡上+陸地境界付近

#define PMD(x, s)	(1.0/(1.0 + (s)*(x)*(x)))
//#define PMD(x, s)	(exp(-0.5*(x)*(x)/s))

//#define PMD_FILTER
#define LELE_FILTER

#ifdef LELE_FILTER
#undef FILTER_CYCLE3

//#define FILTER_ORDER	6
//#define FILTER_CYCLE3	15

//#define FILTER_ORDER	8
//#define FILTER_CYCLE3	20

#define FILTER_ORDER	10
//#define FILTER_CYCLE3	21
//#define FILTER_CYCLE4	9999971
 
#define FILTER_CYCLE3	9999971//17//1951//17//29//31				//流量フラックス
#define FILTER_CYCLE4	9999971										//陸地境界
#define FILTER_CYCLE5	9999971//1951//5507//2399//1487//1303//1051	//波形
#define FILTER_CYCLE6	1											//圧力振動
#define FILTER_CYCLE7	9999971//43//11//61//41						//格子ギャップ

#define FILTER_METHOD	1

#endif

//出力
#define	OUTPUT_VALUE		0	//波高さW
//#define	OUTPUT_VALUE		1	//流量フラックスM
//#define	OUTPUT_VALUE		2	//流量フラックスN
//#define	OUTPUT_VALUE		3	//流速


#define REVETMENT	(0.5)	//（護岸）越流判定
#define DRYDEPTH	(0.0001)
#define DRYDEPTH2	(0.00001)	//これ以下はゼロとみなす

/*
海岸工学論文集 45(1998)
非線形分散波理論式に関する多段階混合差分スキームの提案
原信彦・岩瀬浩之・後藤智明
「非線形分散波理論を計算の支配方程式とするならば移流項を中央差分としても安定に計算することが可能であり、かつ精度の良い結果を得ることが確認できる」
非線形長波理論では移流項は風上差分でないと数値振動がおきるが非線形分散波理論の場合は中央差分で計算できかつ風上差分より精度が良い。
*/

#define USE_UPWINDOW	(0)		//移流項に風上差分を使う(1) 使わない(0)
#define UPWINDOW_ORDER	3

#define USE_UPWINDOW1	(0)	//移流項に1次精度風上差分を使う(1) 使わない(0)
#define USE_UPWINDOW3	(0)	//移流項に3次精度風上差分を使う(1) 使わない(0) 使う場合はUSE_UPWINDOW1=1にする


#define USE_FIVE_ORDER	1

#define PR_BLUR_EFFECT1  (1.0000)	//ぼかし効果への割り増し分
#define PR_BLUR_EFFECT2  (1.0000)	//フィルター処理でのぼかし効果への割り増し分
#define PR_BLUR_EFFECT3  (1.0000)	//フィルター処理でのぼかし効果への割り増し分

#define STAGGERED_GRID	(0)
#define ZERO_AREA_WD	(6)

//地盤変異による地形と水深、初期波高さ補正
//#define OFFSET1		(0.6)
#define OFFSET1		(1.0)

//初期波高さが水深より深い値になってしまった場合の補正
//#define OFFSET2		(0.65)
#define OFFSET2		(0.85)

//#define MINIMUM_DEPTH1 (20.0)	//最低水深(完全反射）
#define MINIMUM_DEPTH2 (0.05)	//最低水深(遡上考慮時)


//吸収領域の判定
#define IsAbsorbingZoneBounray( i, j)(( absorbingZone_ && ( i < absorbingZone_ || i > JY-absorbingZone_ || j < absorbingZone_ || j > IX-absorbingZone_ ))?true:false)


class TempBuffer
{
public:
	char* p;
	TempBuffer()
	{
		p = 0;
		p = new char[512];
	}
	TempBuffer(int sz)
	{
		p = 0;
		p = new char[sz];
	}
	~TempBuffer()
	{
		delete[] p;
		p = 0;
	}
};

#define RIVER_WATER_STEP	160		//河川の指定水深までの分割数

//河川
class River
{
public:
	Rgb color;
	double depth;
	double initflow;
	double Barrier_height;
	double velocity;
};


//湧き出し
class Inflow
{
public:
	Inflow()
	{
		dir = -1.0;
		velocity = 0.0;
	}
	Rgb color;
	double depth;
	double flow;
	double dir;
	double velocity;
};

//堤防
class Barir
{
public:
	Barir()
	{
		zup = 0.0;
	}
	Rgb color;
	double zup;
};

class RainfallIntensityConst
{
public:
	double rainfall;	//雨量[mm/h](1時間当たり）
	double start;		//降水開始[sec](河川初期化を0とする）
	double end;			//降水終了[sec]

	RainfallIntensityConst()
	{
		rainfall = 0.0;
		start = 0;
		end = 0;
	}
};


//河川情報
class RiverInfo
{
	int W;
	int H;
	int init_end;
	int init_end_timeIndex;
public:

	RainfallIntensityConst	rainfallIntensityConst;

	std::vector<River> riverList;	//河川リスト
	std::vector<Inflow> inflowList;	//流入口リスト
	std::vector<Barir> barirList;	//障壁リスト

	unsigned char* riverFlag;		//河川位置(ID)
	unsigned char* inflowFlag;		//流入口位置(ID)
	unsigned char* barirFlag;		//障壁位置(ID)

	int* Flag;

	double* waterDepth;
	RiverInfo()
	{
		init_end = 0;
		riverFlag = 0;
		inflowFlag = 0;
		barirFlag = 0;
		waterDepth = 0;

		Flag = 0;
	}
	~RiverInfo()
	{
		Clear();
	}

	inline bool existRiver() const
	{
		if (riverFlag) return true;
		return false;
	}
	inline bool existInFlow() const
	{
		if (inflowFlag) return true;
		return false;
	}
	inline bool existBarir() const
	{
		if (barirFlag) return true;
		return false;
	}

	inline void InitEnd(int timeIndex)
	{
		init_end = 1;
		init_end_timeIndex = timeIndex;
	}
	inline int isInitEnd() const
	{
		return init_end;
	}
	inline int getInitEndTimeIndex() const
	{
		return init_end_timeIndex;
	}
	void Clear()
	{
		if (riverFlag) delete[] riverFlag;
		if (inflowFlag) delete[] inflowFlag;
		if (barirFlag) delete[] barirFlag;
		if (waterDepth) delete[] waterDepth;

		if (Flag) delete[] Flag;

		Flag = 0;
		barirFlag = 0;
		riverFlag = 0;
		inflowFlag = 0;
		waterDepth = 0;
		riverList.clear();
		inflowList.clear();
		barirList.clear();
	}
	void Init(int w, int h)
	{
		W = w;
		H = h;
		riverFlag = new unsigned char[w*h];
		inflowFlag = new unsigned char[w*h];
		barirFlag = new unsigned char[w*h];
		memset(riverFlag, '\0', sizeof(unsigned char)*w*h);
		memset(inflowFlag, '\0', sizeof(unsigned char)*w*h);
		memset(barirFlag, '\0', sizeof(unsigned char)*w*h);

		Flag = new int[w*h];
		memset(Flag, '\0', sizeof(int)*w*h);
	}

	inline void AutoWaterDepth()
	{
		waterDepth = new double[W*H];
		memset(waterDepth, '\0', sizeof(double)*W*H);
	}
	inline void setBarir(int i, int j, int id)
	{
		barirFlag[i*W + j] = id + 1;
	}

	inline void setRiver( int i, int j, int id)
	{
		riverFlag[i*W + j] = id + 1;
	}

	inline void setInflow(int i, int j, int id)
	{
		inflowFlag[i*W + j] = id + 1;
	}

	inline int isBarir(int i, int j) const
	{
		if (existBarir()) return barirFlag[i*W + j];
		return 0;
	}
	inline int isRiver(int i, int j) const
	{
		if (existRiver() ) return riverFlag[i*W + j];
		return 0;
	}
	inline int isInflow(int i, int j) const
	{
		if (existInFlow())	return inflowFlag[i*W + j];
		else return 0;
	}
	inline int isRiver_or_Inflow(int i, int j) const
	{
		return isRiver(i, j) || isInflow(i, j);
	}
	inline double WaterDepth(int i, int j ) const
	{
		if (existRiver() && waterDepth) return waterDepth[i*W + j];
		return 0;
	}
	inline bool isWaterDepth(int i, int j ) const
	{
		if (waterDepth ) return true;
		return false;
	}
};


//東北地方太平洋沖地震を教訓とした地震・津波対策に関する専門調査会 第７回会合
//http://www.bousai.go.jp/kaigirep/chousakai/tohokukyokun/7/pdf/sub6.pdf
/*
＜避難しない人＞
死者数＝津波影響人口×浸水深別死者率
＜避難しようとする人＞
死者数＝津波影響人口×避難未完了率×浸水深別死者率
低い場合
A地震直後に避難しようとする人 5%
B津波警報入手した時に避難しようとする人 44%
B'津波警報入手出来ない人 11%
C全く避難しない人 40%

高い場合
A地震直後に避難しようとする人 15%
B津波警報入手した時に避難しようとする人 80%
B'津波警報入手出来ない人 0%
C全く避難しない人 5%
*/

//避難意識が低い場合
#define LOW_FAST_EVACUATE			0.05	//地震直後に避難しようとする人
#define LOW_ALARM_EVACUATE			0.44	//津波警報入手した時に避難しようとする人
#define LOW_NO_GET_ALARM_EVACUATE	0.11	//津波警報入手出来ない人
#define LOW_NO_EVACUATE				0.4		//全く避難しない人

//避難意識が高い場合
#define HIG_FAST_EVACUATE			0.15	//地震直後に避難しようとする人
#define HIG_ALARM_EVACUATE			0.80	//津波警報入手した時に避難しようとする人
#define HIG_NO_GET_ALARM_EVACUATE	0.0		//津波警報入手出来ない人
#define HIG_NO_EVACUATE				0.05	//全く避難しない人

#define HOUSE_NUMBER_RATE			2.5		//人口#人に１棟と計算
#define HOUSE_DEST_BOUND			3		//内陸部における浸水では、流速が弱まっているものと考え、
											//家屋は破壊には至らないものと考えられる。
											//このため、全壊棟数・半壊棟数については、海岸線等に接している#メッシュから
											//のみ発生するものとする。

#define FLOOR_AREA_AVERAGE			130.0f		//[m^2]住宅の延べ床面積は平均
#define WOODEN_RATE					0.75f		//木造率
#define RUBBLE_WEGHT_BAS1			1.9f		//1トンあたり木造の場合
#define RUBBLE_WEGHT_BAS2			0.64f		//1トンあたり非木造の場合

#define SEDIMENT					2.9f		//津波堆積物(2.5cm ～ 4.0cm)
#define SEDIMENT_WEGHT_BAS			1.35f		//1.46t/m^3 ～1.10t/m^3

#define RUBBLE_CODST				22000.0f	//1tあたり（阪神・淡路大震災）

class Victim
{
public:
	float average_population_density_adp;	//平均人口密度(1区画内)
	float average_population_density_n_adp;	//平均人口密度(1区画内)
	float adp;	//防災意識あり(awareness of disaster prevention)
	float n_adp;	//防災意識無し(awareness of disaster prevention)

	float house_density;			//家屋の密度（１区画内の家屋数)
	double house_number_rate;	//人口#人に１棟と計算
	float outflow_number;			//家屋流出数
	
	int type;
	float all_destruction;		//全壊
	float partial_destruction;	//半壊
	float inundation;				//床上浸水
	float under;					//床下浸水

	double area;
	double sediment;			//津波堆積量(1区画内)

	inline Victim()
	{
		area = 1000.0*1000.0;	// m^2 (1区画面積 = 1km^2)
		sediment = 0.0;
		adp = n_adp = 0;
		average_population_density_adp = 343;
		average_population_density_n_adp = average_population_density_adp;

		type = -1;
		HouseDensity(house_number_rate);
		outflow_number = 0;
		all_destruction = 0;
		partial_destruction = 0;
		inundation = 0;
		under = 0;
		house_density = 0;
	}

	inline void HouseDensity(double house_number_rate_)
	{
		house_number_rate = house_number_rate_;
		house_density = average_population_density_adp /house_number_rate;
	}

	inline void eval(double w1, double t1)
	{
		const float w = w1;
		const float t = t1;

		//家屋流出計算
		float outflow_number_tmp = 0.0f;
		if ( w < 5.0 )
		{
			outflow_number_tmp = house_density*(0.188f*w - 0.172f);
		}else
		{
			outflow_number_tmp = house_density*(0.7421f*exp(0.0424f*w));
			if ( w > 10.0 ) outflow_number_tmp = house_density*0.99;
		}
		if ( outflow_number_tmp > 0 && outflow_number_tmp > outflow_number )
		{
			outflow_number = outflow_number_tmp;
		}

		//浸水深さと建造物被害の関係（首藤1992)
		if ( w >= 2.0f )
		{
			type = 4;
			all_destruction     = house_density*WOODEN_RATE;
			partial_destruction = 0;
			inundation          = 0;
			under               = 0;
		}else
		if ( w >= 1.0f && w < 2.0f && type < 3)
		{
			type = 3;
			all_destruction     = 0;
			partial_destruction = house_density*WOODEN_RATE;
			inundation          = 0;
			under               = 0;
		}else
		if ( w >= 0.5f && w < 1.0f && type < 2)
		{
			type = 2;
			all_destruction     = 0;
			partial_destruction = 0;
			inundation          = house_density;
			under               = 0;
		}else
		if ( w < 0.5 && type < 1)
		{
			type = 1;
			all_destruction     = 0;
			partial_destruction = 0;
			inundation          = 0;
			under               = house_density;
		}

		//津波堆積物
		if ( w > 0.5f )
		{
			sediment = area*(0.1f*SEDIMENT);	//m^3
		}

		//浸水深別死者率
		double c = 0.0282f*fast_math::exp(0.2328f*w);	//1.0m以上の関係式
		if ( w < 1.0f )
		{
			//線形補間
			c =  0.0282f*fast_math::exp(0.2328f*1.0f)*w;
		}else if ( w > 10.0 )
		{
			c = 0.9999;
		}

		//避難未完了率
		double d = 1.0f/(1.0f+0.00525f*fast_math::exp(0.7f*t/3.565f));
		if ( t < 5.0f ) d = 1.0f;

		//＜避難しない人＞&避難意識が高いの 津波影響人口率
		float adp_tmp1 = c*(HIG_NO_EVACUATE+HIG_NO_GET_ALARM_EVACUATE)*average_population_density_adp;

		//＜避難しない人＞&避難意識が低いの 津波影響人口率
		float adp_tmp2 = c*(LOW_NO_EVACUATE+LOW_NO_GET_ALARM_EVACUATE)*average_population_density_n_adp;

		//＜避難しようとする人＞&避難意識が高い
		adp_tmp1 = adp_tmp1 + c*d*(1.0-(HIG_FAST_EVACUATE+HIG_ALARM_EVACUATE))*average_population_density_adp;

		//＜避難しようとする人＞&避難意識が低い
		adp_tmp2 = adp_tmp2 + c*d*(1.0-(LOW_FAST_EVACUATE+LOW_ALARM_EVACUATE))*average_population_density_n_adp;

		if ( adp_tmp1 >   adp ) adp   = adp_tmp1;
		if ( adp_tmp2 > n_adp ) n_adp = adp_tmp2;

		//死者の重複カウント除去
		average_population_density_adp = average_population_density_adp - adp;
		average_population_density_n_adp = average_population_density_n_adp - n_adp;

		if (average_population_density_adp < 0)
		{
			average_population_density_adp = 0;
		}
		if (average_population_density_n_adp < 0)
		{
			average_population_density_n_adp = 0;
		}

	}
	inline float Apd()
	{
		return adp;
	}
	inline float N_Apd()
	{
		return n_adp;
	}
};


class ChkPointMarker
{
public:
	int r;
	int g;
	int b;
	std::string name;
	int rank;		//信頼度
	int ref;
	double wmax;	//計算値(水位)
	double runup;	//計算値(遡上標高)
	char s[16];		//津浪到達時間(＋20cm以上)
	char t[16];		//最大波高到達時間

	double lon;		//経度
	double lat;		//緯度
	int xx;			//格子上（経度）のインデックス
	int yy;			//格子上（緯度）のインデックス

	int xxx;
	int yyy;

	int type;		//痕跡パターン
					/*
					 遡上高 = 1
					 浸水高 = 2
					 浸水深 = 3
					 その他 = 999
					*/

	ChkPointMarker()
	{
		name = "";
		wmax = 0.0;
		runup = 0.0;
		ref = false;
		victim1 = 0;
		victim2 = 0;
		s[0] = '\0';
		t[0] = '\0';
		lon = 0.0;
		lat = 0.0;
		xx = -1;
		yy = -1;
	}
	double trace;	//痕跡値
	int victim1;
	int victim2;
	int wrk;
};


class HTable
{
public:
	int level;
	double depth;
};

class FunctionValue
{
	int w;
	int h;
public:
	double* Fm;
	double* Fn;
	double* Fw;
	
	FunctionValue()
	{
		Fm = NULL;
		Fn = NULL;
		Fw = NULL;
		w = 0;
		h = 0;
	}

	void create(int width, int height)
	{
		w = width;
		h = height;
		Fm = new double[w*h];
		Fn = new double[w*h];
		Fw = new double[w*h];
		memset(Fm, '\0', w*h*sizeof(double));
		memset(Fn, '\0', w*h*sizeof(double));
		memset(Fw, '\0', w*h*sizeof(double));
	}

	~FunctionValue()
	{
		if ( Fm ) delete [] Fm;
		if ( Fn ) delete [] Fn;
		if ( Fw ) delete [] Fw;
	}
	inline double& FM(const int i, const int j) const
	{
		return Fm[i*w+j];
	}
	inline double& FN(const int i, const int j) const
	{
		return Fn[i*w+j];
	}
	inline double& FW(const int i, const int j) const
	{
		return Fw[i*w+j];
	}

	void Updata(FunctionValue& p)
	{
		if ( w == 0 && h == 0 )
		{
			create(p.w, p.h);
		}
		memcpy(Fm, p.Fm, sizeof(double)*w*h);
		memcpy(Fn, p.Fn, sizeof(double)*w*h);
		memcpy(Fw, p.Fw, sizeof(double)*w*h);
	}
	void Updata(FunctionValue* p)
	{
		if ( w == 0 && h == 0 )
		{
			create(p->w, p->h);
		}
		memcpy(Fm, p->Fm, sizeof(double)*w*h);
		memcpy(Fn, p->Fn, sizeof(double)*w*h);
		memcpy(Fw, p->Fw, sizeof(double)*w*h);
	}

};



class AbeNoguera
{
	bool isCopy;
public:
	double* m;			//X方向流量
	double* n;			//Y方向流量
	double* h;			//水深
	double* h_org;		//水深
	double* w;			//波高さ
	int* topog;			//地形データ
	double* elevation;	//標高データ
	double* latitude;	//緯度
	double* manning;	//粗度(Manning の粗度係数)

	int iX;			//X方向メッシュサイズ
	int jY;			//Y方向メッシュサイズ

	double *r_dx;

	double h_min_org;	//最浅側
	double h_max_org;	//最深側
	double h_min;		//最浅側(この値はかならずゼロであること！！）
	double h_max;		//最深側
	double w_scale[2];	//w_scale[0]:隆起側へのスケール w_scale[1]:沈降側へのスケール
	
	double e_min_org;	//標高最浅側
	double e_max_org;	//標高最高
	double e_min;		//標高基準
	double e_max;		//標高標高

	double* w_start_org;	//初期波源（陸地側の変位も含んだ状態)

	std::vector<HTable> htable;
	std::vector<HTable> etable;
	std::vector<HTable> wuptable;
	std::vector<HTable> wdntable;

	bool wave_LinearInterpolation;

	FunctionValue right;

	AbeNoguera()
	{}
	AbeNoguera(char* topography_data, char* water_depth_data, char* elevation_data, char* Initial_wave_dataUp, char* Initial_wave_dataDown, char* Initial_wave_data2, int impact);

	~AbeNoguera()
	{
		if ( m ) delete [] m;
		if ( n ) delete [] n;
		if ( w ) delete [] w;
		if (!isCopy)
		{
			if (r_dx ) delete [] r_dx;
			if ( h ) delete [] h;
			if ( h_org ) delete  [] h_org;
			if ( topog ) delete [] topog;
			if ( latitude ) delete [] latitude;
			if ( manning ) delete [] manning;
			if ( elevation ) delete [] elevation;
			if ( w_start_org ) delete [] w_start_org;
		}
	}

	void MakeManninig(double manning, double manning2, char* manninig_data);
	void Manning(double manning, double manning2, char* manninig_data, FILE* logfp);

	void MakeLatitude(double latitude[4], char* latitude_data);
	void Latitude(char* latitude_data);

	void ToBitmap(BitMap& bmp);
	void BitmapMask(BitMap& bmp);
	void BitmapMask(BitMap& bmp, BitMap& maskbmp, double* z);
	void BitmapMaskTopog(BitMap& bmp);

	inline double& FM(const int i, const int j) const
	{
		return right.Fm[i*iX+j];
	}
	inline double& FN(const int i, const int j) const
	{
		return right.Fn[i*iX+j];
	}
	inline double& FW(const int i, const int j) const
	{
		return right.Fw[i*iX+j];
	}

	inline double& M(const int i, const int j) const
	{
		return m[i*iX+j];
	}
	inline double& N(const int i, const int j) const
	{
		return n[i*iX+j];
	}
	inline double& W(const int i, const int j) const
	{
		return w[i*iX+j];
	}
	inline double H(const int i, const int j) const
	{
		return h[i*iX+j];
	}
	inline double D(const AbeNoguera*p, const int i, const int j) const
	{
		return p->h[i*iX+j] + w[i*iX+j];
	}
	inline int& T(const int i, const int j) const
	{
		return topog[i*iX+j];
	}

	inline double LAT(const int i, const int j) const
	{
		if ( !latitude) return 0.0;
		return latitude[i*iX+j];
	}

	inline double& MAN(const int i, const int j) const
	{
		return manning[i*iX+j];
	}

	inline double& ELV(const int i, const int j) const
	{
		return elevation[i*iX+j];
	}

	void Output(char* filename, double scale);
	void Output(char* drive, char* dir, int index, double scale );

	AbeNoguera* Copy()
	{
		AbeNoguera* p = new AbeNoguera;
		p->isCopy = true;
		p->iX = iX;
		p->jY = jY;
		p->m = new double[iX*jY];
		p->n = new double[iX*jY];
		p->w = new double[iX*jY];
		memset(p->m, '\0', sizeof(double)*iX*jY);
		memset(p->n, '\0', sizeof(double)*iX*jY);
		memset(p->w, '\0', sizeof(double)*iX*jY);

		p->r_dx = this->r_dx;
		p->h = this->h;
		p->topog = this->topog;
		p->latitude = this->latitude;
		p->manning = this->manning;
		p->elevation = this->elevation;

		memcpy(p->m, m, sizeof(double)*iX*jY);
		memcpy(p->n, n, sizeof(double)*iX*jY);
		memcpy(p->w, w, sizeof(double)*iX*jY);
		return p;
	}

	void Updata(AbeNoguera* p)
	{
		memcpy(m, p->m, sizeof(double)*iX*jY);
		memcpy(n, p->n, sizeof(double)*iX*jY);
		memcpy(w, p->w, sizeof(double)*iX*jY);
	}

};

#define R_DX( i ) (Data->r_dx ? Data->r_dx[i]:(r_dx_flg?_r_dx:(r_dx_flg = true,_r_dx = 1.0/dx,_r_dx)))

class initial_wave
{
public:
	initial_wave()
	{
		displacement_time = -1.0;
		displacement_count = 0;
	}
	std::string fname;
	double start_time;
	double displacement_time;
	int displacement_num;
	int displacement_count;
	double* w_last;
};

class Solver
{
	FILE* log_fp;
	int timeIndex;

	double* data_m;
	double* data_n;
	double* data_w;
	double* data_elv;
	int* data_t;
	double* data_h;
	double* data_man;

	double _r_dx;

#ifndef R_DX
	inline double R_DX(int i)
	{
		if ( Data->r_dx ) return  Data->r_dx[i];

		if ( r_dx_flg ) return _r_dx;

		r_dx_flg = true;
		_r_dx = 1.0/dx;
		return _r_dx;
	}
#endif

	inline double& M(const int i, const int j) const
	{
		return data_m[i*iX+j];
	}
	inline double& N(const int i, const int j) const
	{
		return data_n[i*iX+j];
	}
	inline double& W(const int i, const int j) const
	{
		return data_w[i*iX+j];
	}
	inline double H(const int i, const int j) const
	{
		return data_h[i*iX+j];
	}
	inline double H2(const int i, const int j) const
	{
		//if ( RunUp && Data->ELV(i,j) > 0 ) return -ELV(i,j);
		if ( RunUp && Data->ELV(i,j) > 0 && T(i,j) == 0 ) return D2(i,j);
		return H(i,j);
	}
	inline double absH(const int i, const int j) const
	{
		//if ( RunUp && Data->ELV(i,j) > 0 ) return ELV(i,j);
		if ( RunUp && Data->ELV(i,j) > 0  && T(i,j) == 0 ) return D2(i,j);
		return H(i,j);
	}

	inline double D(const int i, const int j) const
	{
		return Data->D(Data, i,j);
	}

	////inline double D2__(int i, int j, int& stat)
	////{
	////	double diff = 0.0;
	////	stat = RunupConditionFlagCondition(this->Data, i,j , diff );
	////	if (stat == 1 )
	////	{
	////		return diff;
	////	}
	////	if ( stat == 2 ) return 0;

	////	return Data->D(Data, i,j);
	////}
	////inline double D2_(int i, int j )
	////{
	////	int stat;
	////	return D2__( i, j, stat);
	////}
	
	int AbnormalGapFilter();	//流量制限で上端をカットしている場合に滑らかに修正する
	int Filter(int flag);		//数値振動を除去する

	inline void MakeD2()
	{
		const int xsz = iX-ZERO_AREA_WD;
		const int ysz = jY-ZERO_AREA_WD;

		int* runupflg = new int[iX*jY];
		memcpy(runupflg, RunupConditionFlag, sizeof(int)*iX*jY);

#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				int stat = IS_DRY;
				double diff = 0.0;
				stat = RunupConditionFlagCondition(this->Data, i,j , diff );
				if (stat == IS_WET || stat == IS_OVERFLOW )
				{
					D2(i,j) = diff;
				}else if (stat == IS_DRY)
				{
					D2(i,j) = 0.0;
				}else
				{
					D2(i,j) = Data->D(Data, i,j);
				}
				//RUNUPFLG(i,j) = stat;
				runupflg[i*iX + j] = stat;
			}
		}
		memcpy(RunupConditionFlag, runupflg, sizeof(int)*iX*jY);
		delete[] runupflg;
	}

	inline double& D2(const int i, const int j ) const
	{
		return d2vale[i*iX + j];
	}

	inline int& T(const int i, const int j) const
	{
		return data_t[i*iX+j];
	}
	inline double LAT(const int i, const int j) const
	{
		return Data->LAT(i,j);
	}
	inline double& MAN(const int i, const int j) const
	{
		return data_man[i*iX+j];
	}
	inline double& ELV(const int i, const int j) const
	{
		return data_elv[i*iX+j];
	}
	inline int& RUNUPFLG(const int i, const int j) const
	{
		return RunupConditionFlag[i*iX+j];
	}

	//現在の計算値で判定
	inline int RUNUPFLG2(const int i, const int j) const
	{
		if (T(i, j) != 0) return IS_WATER;
		return (W(i, j) < DRYDEPTH2) ? IS_DRY : IS_WET;
	}

	int Caution;
	bool setup;
public:
	int iX;
	int jY;
	std::string drive;
	std::string dir;
	double omega;	//地球の角速度
	double g;		//重力加速度[m/s^2]
	double dt;		//時間ステップ[s]
	double dx;		//空間ステップ[m]
	double dy;		//空間ステップ[m]

	bool r_dx_flg;

	double hmax;	//最大水深
	double hmin;	//最小水深
	double emax;	//最大標高
	double emin;	//最小標高

	double wmax;	//最大波高さ(+)
	double wmin;	//最大波高さ(-)

	double Manning;	//粗度(Manning の粗度係数)
	double nu;		//水平渦動粘性係数(m2/s)

	//無反射境界条件パラメータ(Cerjan　method)

	int absorbingZone;		//吸収ゾーン（グリッド数）
	double attenuation;		//減衰係数

	double maxheight;			//到達最大波高さ
	double maxheight_cur;		//到達最大波高さ(計算時刻での）
	int	maxheightPos[2];		//到達最大波高さ位置

	double tide_level_offset;	//潮位補正値

	double maxheight_w2;			//到達最大遡上水位
	double maxheight_w_cur2;		//到達最大遡上水位(計算時刻での）
	double maxheight2;			//到達最大遡上高さ
	double maxheight_cur2;		//到達最大遡上高さ(計算時刻での）

	int Coriolis_force;		//コリオリ因子の考慮
	int Soliton;			//ソリトン分裂の考慮
	int RunUp;				//遡上の考慮
	int* RunUpMarker;
	int* RunupConditionFlag;
	
	double* d2vale;
	double* coriolisFactor;

	double* waterLevelRaising;	
	double* shearForceX;		//海面せん断力
	double* shearForceY;		//海面せん断力

	void DeleteStormInfo()
	{
		if ( shearForceX ) delete [] shearForceX;
		if ( shearForceY ) delete [] shearForceY;
		shearForceX = 0;
		shearForceY = 0;
	}

	int filterOrder;
	int filterCycle1;
	int filterCycle2;
	int filterCycle3;
	int filterCycle4;
	int filterCycle5;

	RiverInfo riverInfo;	//河川情報
	double river_running_time;

	int impact;		//隕石衝突による津波計算

	int* BoundaryBandMap;
	int* WaterBoundaryBandMap;

	double flow_res;	//流量制限係数(0< flow_res <=1)
	double flow_res2;	//流量制限係数(4<= flow_res2)

	float K_Value;		//Κ値
	float k_Value;		//κ値

	float Victim_tot1;	//人的被害
	float Victim_tot2;	//人的被害

	float outflow_number;	//家屋流出数
	float all_destruction;		//全壊
	float partial_destruction;	//半壊
	float inundation;				//床上浸水
	float under;					//床下浸水
	float evacuees_number;		//避難所避難者数(発災当日～発災2日後)

	double sediment;			//津波堆積物量(m^3)

	//海岸工学音文集(1998)小谷ほか 「Dがゼロに近いと必然的にMもゼロに近づく」
	//Dが小さいときは無視する
	//※本来は遡上を扱うときの対処だが非常に浅い箇所でも計算が発散する場合がある。
	double Dmin;			//計算安定化のための制限

	//相田の方法（護岸・越流の条件）
	double Revetment_h;
	bool LinearInterpolation;

	int* InviolabilityP;	//不可侵域
	
	int terrain_displacement;	//地形の変位を考慮

	double smooth[3][3];

	AbeNoguera* Data;
	Storm_Line* Storm_p;

	double* dispersion_correction_coefficient;

	//砕波減衰パラメータ
	float* Breaking_factor;
	char* Breaking_point;

	//非線形分散項の補正係数
	double dispersion_correction_coefficient1;
	double dispersion_correction_coefficient2;

	//段差（今は使っていない)
	double gap_max;
	double gap_max_coef;

	double gap_depth;
	double gap_depth_coef;

	//急勾配
	double steep_slope1[2];
	double steep_slope2[2];

	//変移開始状態へのリセット
	int w_init;	// =-1:全地殻変動終了 =0:今は地殻変動していない =1:これから地殻変動する

	FunctionValue tdiff;

	double checker_bord_theshold;		// CHECKER_BORD_THESHOLD1

	std::vector<initial_wave> initial_wave_list;

	inline std::vector<int> get_start_initial_wave_list(double timesum)
	{
		std::vector<int> displacementList;
		for (int i = 0; i < initial_wave_list.size(); i++)
		{
			if (initial_wave_list[i].start_time <= timesum && initial_wave_list[i].displacement_count == 0)
			{
				if (initial_wave_list[i].w_last == NULL)
				{
					const char* fname = initial_wave_list[i].fname.c_str();
					initial_wave_list[i].w_last = ReadCsv((char*)fname, iX, jY);
				}
				displacementList.push_back(i);

				for (int ii = 0; ii < Data->jY; ii++)
				{
					for (int jj = 0; jj < Data->iX; jj++)
					{
#if _MSC_VER > 1700
						double uz = initial_wave_list[i].w_last[ii*Data->iX + jj];
						if (isnan<double>(uz) || isinf<double>(uz))
						{
							printf("---- Nan ---\n");
							initial_wave_list[i].w_last[ii*Data->iX + jj] = 0.0;
						}
#endif
						//（陸地に行き成り波は立たない)
						if (Data->topog[ii*Data->iX + jj] == 0 || isAbsorbingZoneBounray(ii, jj))
						{
							initial_wave_list[i].w_last[ii*Data->iX + jj] = 0.0;
						}
					}
				}
			}
		}
		return displacementList;
	}

	inline std::vector<int> get_initial_wave_list(double timesum)
	{
		std::vector<int> displacementList;
		for (int i = 0; i < initial_wave_list.size(); i++)
		{
			if (initial_wave_list[i].start_time <= timesum && initial_wave_list[i].displacement_count >= 0)
			{
				if (initial_wave_list[i].displacement_count < initial_wave_list[i].displacement_num)
				{
					if (initial_wave_list[i].w_last == NULL)
					{
						const char* fname = initial_wave_list[i].fname.c_str();
						initial_wave_list[i].w_last = ReadCsv((char*)fname, iX, jY);
					}

					initial_wave_list[i].displacement_count++;
					displacementList.push_back(i);
				}
			}
		}
		return displacementList;
	}

	inline std::vector<int> get_next_initial_wave_list(double timesum)
	{
		std::vector<int> displacementList;
		for (int i = 0; i < initial_wave_list.size(); i++)
		{
			if (initial_wave_list[i].start_time <= timesum && initial_wave_list[i].displacement_count >= 0)
			{
				if (initial_wave_list[i].displacement_count < initial_wave_list[i].displacement_num)
				{
					displacementList.push_back(i);
				}
			}
		}
		return displacementList;
	}

	inline bool is_exist_initial_wave_list()
	{
		for (int i = 0; i < initial_wave_list.size(); i++)
		{
			if (initial_wave_list[i].displacement_count < initial_wave_list[i].displacement_num)
			{
				return true;
			}
		}
		return false;
	}

	int use_upwindow;
	int solver;


	void UpdateGridInfo()
	{
		data_m = Data->m;
		data_n = Data->n;
		data_w = Data->w;
		data_elv = Data->elevation;
		data_h = Data->h;
		data_t = Data->topog;
		data_man = Data->manning;
	}

	void SetData(AbeNoguera& data)
	{
		Data = &data;
		iX = Data->iX;
		jY = Data->jY;

		UpdateGridInfo();
	}

	Solver()
	{
		steep_slope1[0] = STEEP_SLOPE1_1;
		steep_slope1[1] = STEEP_SLOPE1_2;
		steep_slope2[0] = STEEP_SLOPE2_1;
		steep_slope2[1] = STEEP_SLOPE2_2;

		river_running_time = 600;

		checker_bord_theshold = CHECKER_BORD_THESHOLD1;
		terrain_displacement = 1;
		w_init = 1;
		gap_max_coef = GAP_MAX;
		gap_depth_coef = GAP_DEPTH;
		use_upwindow = 3;
		dispersion_correction_coefficient = 0;
		Breaking_point = 0;
		Breaking_factor = 0;
		impact = 0;
		gap_flg = 0;
		shearForceX = 0;
		shearForceY = 0;
		sediment = 0.0;
		evacuees_number = 0;
		outflow_number = 0;
		CheckerBord[0] = 0;
		CheckerBord[1] = 0;
		CheckerBord[2] = 0;
		Storm_p = 0;
		coriolisFactor = 0;
		flow_res = 1.0;
		flow_res2 = FLOW_DEPTH_LIMIT_COND_COEF3;
		r_dx_flg = false;
		Victim_tot1 = 0;
		Victim_tot2 = 0;
		K_Value = -1.0;
		k_Value = -1.0;
		WaterBoundaryBandMap = NULL;
		BoundaryBandMap = NULL;
		Revetment_h = REVETMENT;
		tide_level_offset = 0.0;
		filterOrder = FILTER_ORDER;
		filterCycle1 = FILTER_CYCLE3;
		filterCycle2 = FILTER_CYCLE4;
		filterCycle3 = FILTER_CYCLE5;
		filterCycle4 = FILTER_CYCLE6;
		filterCycle5 = FILTER_CYCLE7;
		d2vale = NULL;
		RunupConditionFlag = NULL;
		RunUpMarker = NULL;
		Dmin = D_UNDER_LIMIT;
		RunUp = 0;
		Soliton = 0;
		solver = 0;
		Coriolis_force = 0;
		maxheight = maxheight_cur = -1000000.0;
		maxheight_w2 = maxheight_w_cur2 = -1000000.0;
		maxheight2 = maxheight_cur2 = 0.0;
		wmax = wmin = 0.0;
		g = 9.80665;
		omega = 7.2921159e-5;
		timeIndex = 0;
		setup = false;
		absorbingZone = 20;
		attenuation = 0.015;
		Manning = 0.025;
		nu = 10.0;	//水の分子動粘性係数 (土木学会( 2 0 0 2 ))
		Caution = 0;
		LinearInterpolation = true;
		InviolabilityP = NULL;
		log_fp = 0;
	}

	inline int& SetInviolability(const int i, const int j)
	{
		if (InviolabilityP == NULL)
		{
			InviolabilityP = new int[iX*jY];
			memset(InviolabilityP, '\0', sizeof(int)*iX*jY);
		}

		return InviolabilityP[i*iX + j];
	}
	inline int Inviolability(const int i, const int j) const
	{
		if (InviolabilityP != NULL)
		{
			return InviolabilityP[i*iX + j];
		}
		return 0;
	}

	inline void DeleteInviolability()
	{
		if (InviolabilityP) delete[] InviolabilityP;
		InviolabilityP = NULL;
	}

	double* tmpX;
	double* tmpY;
	inline double& ARY_wrkX(const int i, const int j) const
	{
		return tmpX[i*iX+j];
	}
	inline double& ARY_wrkY(const int i, const int j) const
	{
		return tmpY[i*iX+j];
	}
	inline double& ARY_wrk(double* p, const int i, const int j) const
	{
		return p[i*iX+j];
	}
	void generalFilter(double* arry);
	void generalFilter(double* arry1, double* arry2);

	inline FILE* getLogFp() const
	{
		return log_fp;
	}
	inline void logOpen(char* filename)
	{
		log_fp = fopen(filename, "w");
	}
	inline void logClose()
	{
		if ( log_fp ) fclose(log_fp);
		log_fp = 0;
	}
	inline void log_printf(char* format, ...) const
	{
		va_list	argp;
		char pszBuf[ 4096];
		va_start(argp, format);
		vsnprintf( pszBuf, 4096, format, argp);
		va_end(argp);

		if ( log_fp != NULL )
		{
			fprintf(log_fp, "%s", pszBuf); fflush(log_fp);
		}
		::printf("%s", pszBuf);
	}
	inline void log_printf2(char* format, ...) const
	{
		va_list	argp;
		char pszBuf[ 4096];
		va_start(argp, format);
		vsnprintf( pszBuf, 4096, format, argp);
		va_end(argp);

		if ( log_fp != NULL )
		{
			fprintf(log_fp, "%s", pszBuf); fflush(log_fp);
		}
	}

	int __order;
	void eval(AbeNoguera* nxttime, AbeNoguera* cur, double delta_t);

	int MotionEquation2(double delta_t, double delta_x, double delta_y);		//運動方程式
	int ContinuityEquation2(double delta_t, double delta_x, double delta_y);	//連続の式

	int MotionEquation3(AbeNoguera* p, AbeNoguera* cur, double delta_t, double delta_x, double delta_y);		//運動方程式
	int ContinuityEquation3(AbeNoguera* , AbeNoguera* curp, double delta_t, double delta_x, double delta_y);	//連続の式

	double w_max;
	double w_min;

	void Setup0(double delta_t_coef, double delta_x, double delta_y)
	{
		w_max = -100000.0;
		w_min = 10000.0;
		double hw = 1.0;
		double ew = 1.0;
		
		if ( LinearInterpolation )
		{
			if ( Data->h_max_org - Data->h_min_org != 0.0 )
			{
				hw = (Data->h_max-Data->h_min)/(Data->h_max_org - Data->h_min_org);
			}else
			{
				hw = Data->h_max/Data->h_max_org;
				Data->h_min_org = 0;
			}
			if ( Data->h_max == Data->h_min )
			{
				hw = 1.0;
			}

			if ( Data->e_max_org - Data->e_min_org != 0.0 )
			{
				ew = (Data->e_max-Data->e_min)/(Data->e_max_org - Data->e_min_org);
			}else
			{
				ew = Data->e_max/Data->e_max_org;
				Data->e_min_org = 0;
			}
			if ( Data->e_max == Data->e_min )
			{
				ew = 1.0;
			}
		}

		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				if ( LinearInterpolation )
				{
					if ( T(i,j) != 0.0 )
					{
						if ( hw == 1.0 || Data->htable.size() <= 2 )
						{
							//単純線形補間
							Data->h[i*iX+j] = Data->h_min + (H(i,j) - Data->h_min_org)*hw;
						}else
						{
							if ( H(i,j) < Data->htable[0].level )
							{
								//指定テーブルの最小より小さい場合は単純線形補間
								Data->h[i*iX+j] = Data->h_min +  (Data->htable[0].depth-Data->h_min)*(H(i,j) - 0)/(Data->htable[0].level - 0);
							}else if ( H(i,j) >= Data->htable[0].level &&  H(i,j) < Data->htable[Data->htable.size()-1].level)
							{
								//区間で指定した線形補間
								for ( int k = 1; k < Data->htable.size(); k++ )
								{
									if ( H(i,j) >= Data->htable[k-1].level && H(i,j) < Data->htable[k].level )
									{
										Data->h[i*iX+j] = Data->htable[k-1].depth +  (Data->htable[k].depth-(double)Data->htable[k-1].depth)*((double)H(i,j) - (double)Data->htable[k-1].level)/((double)Data->htable[k].level - (double)Data->htable[k-1].level);
										break;
									}
								}
							}else if ( H(i,j) >= Data->htable[Data->htable.size()-1].level )
							{
								//指定テーブルの最大より大きい場合は単純線形補間
								Data->h[i*iX+j] = Data->h_min +  (Data->h_max-Data->h_min)*(H(i,j) - Data->h_min_org)/(Data->h_max_org - Data->h_min_org);
							}
							if ( Data->h[i*iX+j] < 0 ) Data->h[i*iX+j] = 0;
						}
					}

					if ( T(i,j) == 0.0 )
					{
						if ( Data->etable.size() <= 2 )
						{
							ELV(i,j) = Data->e_min + (ELV(i,j) - Data->e_min_org)*ew;
						}else
						{
							if ( ELV(i,j) < Data->etable[0].level )
							{
								ELV(i,j) = Data->e_min +  (Data->etable[0].depth-Data->e_min)*(ELV(i,j) - 0)/(Data->etable[0].level - 0);
							}else if ( ELV(i,j) >= Data->etable[0].level &&  ELV(i,j) < Data->etable[Data->htable.size()-1].level)
							{
								for ( int k = 1; k < Data->etable.size(); k++ )
								{
									if ( ELV(i,j) >= Data->etable[k-1].level && ELV(i,j) < Data->etable[k].level )
									{
										ELV(i,j) = Data->etable[k-1].depth +  (Data->etable[k].depth-(double)Data->etable[k-1].depth)*((double)ELV(i,j) - (double)Data->etable[k-1].level)/((double)Data->etable[k].level - (double)Data->etable[k-1].level);
										break;
									}
								}
							}else if ( ELV(i,j) >= Data->etable[Data->etable.size()-1].level )
							{
								ELV(i,j) = Data->e_min +  (Data->e_max-Data->e_min)*(ELV(i,j) - Data->e_min_org)/(Data->e_max_org - Data->e_min_org);
							}
						}
					}
				}

				if ( Data->wave_LinearInterpolation )
				{
					if ( W(i,j) > 0.0 )
					{
						if ( Data->wuptable.size() < 2 )
						{
							W(i,j) = W(i,j)*Data->w_scale[0];
						}
						else if ( W(i,j) < Data->wuptable[0].level ) W(i,j) = 0.0;
						else if ( W(i,j) >= Data->wuptable[Data->wuptable.size()-1].level ) W(i,j) = Data->wuptable[Data->wuptable.size()-1].depth;
						else
						{
							for ( int k = 1; k < Data->wuptable.size(); k++ )
							{
								if ( W(i,j) >= Data->wuptable[k-1].level && W(i,j) < Data->wuptable[k].level )
								{
									W(i,j) = Data->wuptable[k-1].depth +  (Data->wuptable[k].depth-(double)Data->wuptable[k-1].depth)*((double)W(i,j) - (double)Data->wuptable[k-1].level)/((double)Data->wuptable[k].level - (double)Data->wuptable[k-1].level);
									break;
								}
							}
						}
					}else
					if ( W(i,j) < 0.0 )
					{
						if ( Data->wdntable.size() < 2 )
						{
							W(i,j) = W(i,j)*Data->w_scale[1];
						}
						else if ( -W(i,j) < Data->wdntable[0].level ) W(i,j) = 0.0;
						else if ( -W(i,j) >= Data->wdntable[Data->wdntable.size()-1].level ) W(i,j) = Data->wdntable[Data->wdntable.size()-1].depth;
						else
						{
							for ( int k = 1; k < Data->wdntable.size(); k++ )
							{
								if ( -W(i,j) >= Data->wdntable[k-1].level && -W(i,j) < Data->wdntable[k].level )
								{
									W(i,j) = Data->wdntable[k-1].depth +  (Data->wdntable[k].depth-(double)Data->wdntable[k-1].depth)*((double)(-W(i,j)) - (double)Data->wdntable[k-1].level)/((double)Data->wdntable[k].level - (double)Data->wdntable[k-1].level);
									break;
								}
							}
						}
					}
				}
			}
		}

		printf("補正 START\n"); fflush(stdout);
		//補正
		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				//陸地定義なのに標高が負（ゼロメートル地帯)
				if ( T(i,j) == 0 && ELV(i,j) < 0.0 )
				{
					Data->h[i*iX+j] = 0.0;
					//ただし、下限を設ける（地形データとの不整合を緩和）
					if ( ELV(i,j) <= EDIT_ELV_HEIGHT_LIM ) ELV(i,j) = EDIT_ELV_HEIGHT_LIM;
				}

				//水域なのに水深が負（標高あり）
				if ( T(i,j) != 0 && Data->h[i*iX+j] < 0.0 )
				{
					//EDIT_WTR_DEPTH以下なら陸地とする
					if ( Data->h[i*iX+j] <= -EDIT_WTR_DEPTH )
					{
						//ELV(i,j) = -Data->h[i*iX+j];
						//不整合を緩和するため水域と陸地の境界なので０に設定する
						ELV(i,j) = 0.0;
						T(i,j) = 0.0;
					}
					Data->h[i*iX+j] = 0.0;
				}
			}
		}
		printf("補正 END\n"); fflush(stdout);

		//地盤変動による補正
		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				if ( W(i,j) != 0.0 )
				{
					if ( impact )
					{
						if ( W(i,j) < 0.0 )
						{
							if ( T(i,j) != 0 && -Data->h[i*iX+j] > W(i,j) )
							{
								Data->h[i*iX+j] = -W(i,j)+0.1;
							}
							else if ( T(i,j) == 0 )
							{
								ELV(i,j) = W(i,j)-0.1;
								T(i,j) = 255;
								Data->h[i*iX+j] = -W(i,j)+0.1;
							}
						}
					}else
					{
						if (!Data->wave_LinearInterpolation)W(i,j) *= Data->w_scale[0];
						if (terrain_displacement)
						{
#if _MSC_VER > 1700
							if (!isnan<double>(W(i, j)) && !isinf(W(i, j)))
							{
								if (T(i, j) != 0) Data->h[i*iX + j] -= (W(i, j))*OFFSET1;
								else ELV(i, j) += W(i, j)*OFFSET1;
							}
#else
							//if (!_isnan<double>(W(i, j)) && !_isinf(W(i, j)))
							{
							if (T(i, j) != 0) Data->h[i*iX + j] -= (W(i, j))*OFFSET1;
							else ELV(i, j) += W(i, j)*OFFSET1;
							}
#endif
						}
					}

					if ( T(i,j) == 0 && ELV(i,j) < 0.0 )
					{
						Data->h[i*iX+j] = 0.0;
						W(i,j) = 0.0;
						//printf("水没 %f\n", ELV(i,j));
						if ( ELV(i,j) <= EDIT_ELV_HEIGHT_LIM ) ELV(i,j) = EDIT_ELV_HEIGHT_LIM;
					}
					if (T(i,j) != 0 && Data->h[i*iX+j] < 0.0 )
					{
						if ( Data->h[i*iX+j] <= -EDIT_WTR_DEPTH )
						{
							//ELV(i,j) = -Data->h[i*iX+j];
							ELV(i,j) = 0.0;
							T(i,j) = 0.0;
							W(i,j) = 0.0;
						}
						Data->h[i*iX+j] = 0.0;
					}
				}
			}
		}

		if ( impact )
		{
			for ( int k = 0; k < 5; k++ )
			{
				double *wk = new double[jY*iX];
				memcpy(wk, Data->h, sizeof(double)*jY*iX);

				for ( int i = 1; i < jY-1; i++ )
				{
					for ( int j = 1; j < iX-1; j++ )
					{
						if ( 
							W(i-1,j-1) < 0.0|| W(i-1,j) < 0.0|| W(i-1,j+1) < 0.0 ||
							W(i  ,j-1) < 0.0|| W(i  ,j) < 0.0|| W(i,  j+1) < 0.0 ||
							W(i+1,j-1) < 0.0|| W(i+1,j) < 0.0|| W(i+1,j+1) < 0.0
							)
						{
							wk[i*iX+j] = 0.25*(H(i,j-1) + H(i,j+1) + H(i-1,j) + H(i+1,j));
						}
					}
				}
				memcpy(Data->h, wk, sizeof(double)*jY*iX);
				delete [] wk;
			}
			for ( int k = 0; k < 5; k++ )
			{
				double *wk = new double[jY*iX];
				memcpy(wk, Data->elevation, sizeof(double)*jY*iX);

				for ( int i = 1; i < jY-1; i++ )
				{
					for ( int j = 1; j < iX-1; j++ )
					{
						if ( 
							W(i-1,j-1) < 0.0|| W(i-1,j) < 0.0|| W(i-1,j+1) < 0.0 ||
							W(i  ,j-1) < 0.0|| W(i  ,j) < 0.0|| W(i,  j+1) < 0.0 ||
							W(i+1,j-1) < 0.0|| W(i+1,j) < 0.0|| W(i+1,j+1) < 0.0
							)
						{
							wk[i*iX+j] = 0.25*(H(i,j-1) + H(i,j+1) + H(i-1,j) + H(i+1,j));
						}
					}
				}
				memcpy(Data->elevation, wk, sizeof(double)*jY*iX);
				delete [] wk;
			}

		}

		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				if ( H(i,j) + W(i,j) < 0.0 )
				{
					if ( T(i,j) != 0 )
					{
						W(i,j) = 0.05-Data->h[i*iX+j];
					}else
					{
						W(i,j) = 0.0;
					}
				}
			}
		}

		double emax = -999999999;
		double emin = 9999999999;
		double hmin = 9999999999;
		double hmax = -999999999;
		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				if ( T(i,j) != 0.0 && H(i,j) < hmin ) hmin = H(i,j);
				if ( T(i,j) != 0.0 && H(i,j) > hmax ) hmax = H(i,j);
				if ( T(i,j) != 0.0 && W(i,j) > w_max) w_max = W(i,j);
				if ( T(i,j) != 0.0 && W(i,j) < w_min) w_min = W(i,j);
				if ( T(i,j) == 0.0 && ELV(i,j) > emax) emax = ELV(i,j);
				if ( T(i,j) == 0.0 && ELV(i,j) < emin) emin = ELV(i,j);
			}
		}
		log_printf("[補正後（水深）]最深hmax %f[m] 最浅:hmin[m] %f\n", hmax, hmin);
		log_printf("[補正後（標高）]最高emax %f[m] 最低:emin[m] %f\n", emax, emin);

		dx = delta_x;
		dy = delta_y;

		dt = dt*delta_t_coef;
	}

	int ele_area;
	int Setup(double delta_t_coef, double delta_x, double delta_y)
	{
		ele_area = 0;
		dx = delta_x;
		dy = delta_y;

		int s = 0;
		int t = 0;
		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				if ( T(i,j) != 0.0 ) s++;
				if ( T(i,j) == 0.0 ) t++;
			}
		}
		//if ( s == 0 ) return -1;
		ele_area = t;

		emax = -999999999;
		emin = 9999999999;
		hmin = 9999999999;
		hmax = -999999999;
		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				if ( T(i,j) != 0.0 && H(i,j) < hmin ) hmin = H(i,j);
				if ( T(i,j) != 0.0 && H(i,j) > hmax ) hmax = H(i,j);
				if ( T(i,j) != 0.0 && W(i,j) > w_max) w_max = W(i,j);
				if ( T(i,j) != 0.0 && W(i,j) < w_min) w_min = W(i,j);
				if ( T(i,j) == 0.0 && ELV(i,j) > emax) emax = ELV(i,j);
				if ( T(i,j) == 0.0 && ELV(i,j) < emin) emin = ELV(i,j);
			}
		}
		if ( t == 0 ) printf("陸地が存在しません\n");

		double hh = 2000.0;	//[m]	隕石衝突による波高（水位）を2000mを上限とする
		if ( !impact ) hh = 0.0;

		if (s == 0)
		{
			hmax = emax;
			hmin = emin;
			hmax = 15.0;
			log_printf("仮想の水域水深を設定\n");
		}

		double ds = sqrt(dx*dx + dy*dy);
		dt = ds/sqrt(fabs(2.0*g*(hmax+hh)));
		log_printf("計算用時間ステップ %.4f[s]\n", dt);
		if (  Data->r_dx )
		{
			ds = sqrt(R_DX(0)*R_DX(0)+ dy*dy);
			dt = ds/sqrt(fabs(2.0*g*(hmax+hh)));
			log_printf("計算用時間ステップ補正 %.4f[s]\n", dt);
		}
		
		log_printf("\n[補正後（水深）]最深hmax %f[m] 最浅:hmin[m] %f\n", hmax, hmin);
		log_printf("[補正後（標高）]最高emax %f[m] 最低:emin[m] %f\n", emax, emin);
		dt = dt*delta_t_coef;
		log_printf("■■■計算用時間ステップ補正2 %.4f[s]■■■\n", dt);
		log_printf("メッシュサイズdx %f dy %f dt/ds : %f\n", dx, dy, dt/ds);
		log_printf("初期水面隆起高さ:%f[m]\n", w_max);
		log_printf("初期水面沈降高さ:%f[m]\n", w_min);

		log_printf("実空間サイズlength:%.3f[m]x%.3f[m]\n", delta_x*iX, delta_y*jY);
		setup = true;

		if (s == 0) return -1;
		return 0;
	}

	double absorbingZoneBounray(const int i, const int j, int flg =  -1);//吸収境界条件 Crejan(1985)
	inline bool isAbsorbingZoneBounray(const int i, const int j) const
	{
		if ( absorbingZone )
		{
			if ( i < absorbingZone || i > jY-absorbingZone || j < absorbingZone || j > iX-absorbingZone )
			{
				return true;
			}
		}
		return false;
	}

	inline bool isDry(const int i, const int j) const
	{
		if ( W(i,j) < DRYDEPTH ) return true;
		return false;
	}
	inline bool isWet(const int i, const int j ) const
	{
		return !isDry(i,j);
	}

	inline int RunupConditionFlagCondition(const AbeNoguera* p, const int i, const int j, double& d, int* flag=0) const
	{
		if (flag) *flag = 0;
		d = 0.0;
		if ( !RunUp) return 0;
		
#include "array_expand.h"
		int s = 0;

		if ( T__(i,j) != 0 ) return IS_WATER;

		if (Inviolability(i, j) == 1)
		{
			return IS_DRY;
		}

		double revetment_h = 0.0;
		double w = W__(i,j);

		//遡上境界条件 [岩崎・真野（1979）の方法]

		//小谷の方法「GISを利用した津波遡上計算と被害推定法」
		//今村の方法(東北大)
		if ( T__(i,j) == 0 && isDry(i,j))
		{
			//T(i,j)が陸地、その隣が海の場合
			double dmax = -1.0;
			double elv1 = ELV__(i,j);

			for (int ki = -1; ki <= 1; ki++)
			{
				for (int kj = -1; kj <= 1; kj++)
				{
					if (ki == 0 && kj == 0) continue;

					if (i + ki < 0 || i + ki >= jY) continue;
					if (j + kj < 0 || j + kj >= iX) continue;

					double elv = elv1;
					if (elv < 0.0) elv = 0.0;
					if (T__(i + ki, j + kj) != 0 && isWet(i + ki, j + kj) && W__(i + ki, j + kj) - elv >= DRYDEPTH)
					{
						d = W__(i + ki, j + kj) - elv;
						if (dmax < d)
						{
							revetment_h = H__(i + ki, j + kj) + elv;
							dmax = d;
						}
						//if ( ELV(i,j) > 4.0 )printf("W:%f ELV:%f %f\n", W(i-1,j), ELV(i,j), dmax);
					}
					else
					{
						//T(i,j)が陸地、その隣も陸地の場合
						elv = elv1;
						if (T__(i+ki, j+kj) == 0 && isWet(i+ki, j+kj) && W__(i+ki, j+kj) + ELV(i+ki, j+kj) - elv >= DRYDEPTH)
						{
							d = W__(i + ki, j + kj) + ELV__(i + ki, j + kj) - elv;
							if (ELV__(i + ki, j + kj) > elv) d = W__(i + ki, j + kj);

							if (dmax < d)
							{
								if (ELV__(i+ki, j+kj) < elv) revetment_h = elv - ELV__(i+ki, j+kj);
								dmax = d;
							}
						}
					}
				}
			}
			if (flag) *flag = 1;	//移動境界条件
			d = dmax;

			if ( d < DRYDEPTH ) return IS_DRY;

			if (riverInfo.isRiver(i, j))
			{
				riverInfo.Flag[i*iX + j] = 1;
			}
			//if ( revetment_h > Revetment_h ) return IS_OVERFLOW;
			return IS_WET;
		}

		if (flag) *flag = 2;
		if ( T__(i,j) == 0 && isWet(i,j) )
		{
			d = w;
			if ( d < DRYDEPTH ) return IS_DRY;
			//if ( revetment_h > Revetment_h ) return IS_OVERFLOW;
			return IS_WET;
		}
		d = 0.0;
		return IS_DRY;
	}

	inline void TBoundaryConditionMain(AbeNoguera* p, int i, int j) const
	{
		//陸境界の条件(完全反射)
		if ( !RunUp )
		{
			TBoundaryCondition(p, i, j);
		}else
		{
			//移動境界での完全反射
			TBoundaryCondition2(p, i, j);
		}
	}

	//陸境界の条件(完全反射)
	inline void TBoundaryCondition(AbeNoguera* p, int i, int j) const
	{
#include "array_expand.h"

#if 10
		if ( T__(i,j) != 0 )
		{
			if ( T__(i-1,j) == 0 || T__(i+1,j) == 0)
			{
				p->N(i,j) = 0.0;
			}
			if ( T__(i,j-1) == 0 || T__(i,j+1) == 0)
			{
				p->M(i,j) = 0.0;
			}
		}
#else
		if ( T__(i,j) == 0 )
		{
			p->N(i,j) = 0.0;
			p->M(i,j) = 0.0;
		}
#endif
	}

	//移動境界の条件
	inline void TBoundaryCondition2(AbeNoguera* p, int i, int j) const
	{
#include "array_expand.h"
		bool boundary_cond = false;

		if (RUNUPFLG(i, j) != IS_DRY)
		{
			if (RUNUPFLG(i - 1, j) == IS_DRY || RUNUPFLG(i + 1, j) == IS_DRY)
			{
				p->N(i, j) = 0.0;
			}
			if (RUNUPFLG(i, j - 1) == IS_DRY || RUNUPFLG(i, j + 1) == IS_DRY)
			{
				p->M(i, j) = 0.0;
			}
		}
	}

	int WaveHeightBounrayCondition(AbeNoguera* p, int i, int j);

	void CreateBoundaryBandMap(int width)
	{
		if ( BoundaryBandMap ) return;

		BoundaryBandMap = new int[jY*iX];
		memset(BoundaryBandMap, '\0', sizeof(int)*jY*iX);

		const int ysz = jY-ZERO_AREA_WD;
		const int xsz = iX-ZERO_AREA_WD;


#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				BoundaryBandMap[i*iX+j] = (BoundaryBand(i,j,width, 1)==true)?1:0;
			}
		}
	}
	void DeleteBoundaryBandMap()
	{
		if ( !BoundaryBandMap ) return;
		delete [] BoundaryBandMap;
		BoundaryBandMap = 0;
	}

	void DeleteCoriolisFactor()
	{
		if ( coriolisFactor ) delete [] coriolisFactor;
		coriolisFactor = 0;
	}


	//陸地境界近傍かチェックする
	inline bool BoundaryBand(int i, int j, int width, int flag = 0) const
	{
		if ( flag==0 && BoundaryBandMap )
		{
			return (BoundaryBandMap[i*iX+j] == 1)?true:false;
		}
#include "array_expand.h"

		const int xsz = iX-ZERO_AREA_WD;
		const int ysz = jY-ZERO_AREA_WD;
		bool check = false;
		if ( T__(i,j) == 0 )
		{
			for ( int ii = i-width; ii < i+width; ii++ )
			{
				if ( ii < ZERO_AREA_WD ) continue;
				if ( ii >= ysz ) continue;
				for ( int jj = j-width; jj < j+width; jj++ )
				{
					if ( jj < ZERO_AREA_WD ) continue;
					if ( jj >= xsz ) continue;

					if ( T__(ii,jj) != 0 )
					{
						check = true;
						break;
					}
					if ( check ) break;
				}
			}
		}else if ( (T__(i,j) != 0))
		{
			for ( int ii = i-width; ii < i+width; ii++ )
			{
				if ( ii < ZERO_AREA_WD ) continue;
				if ( ii >= ysz ) continue;
				for ( int jj = j-width; jj < j+width; jj++ )
				{
					if ( jj < ZERO_AREA_WD ) continue;
					if ( jj >= xsz ) continue;

					if ( T__(ii,jj) == 0 )
					{
						check = true;
						break;
					}
					if ( check ) break;
				}
			}
		}
		return check;
	}


	inline void CreateWaterBoundaryBandMap(int width)
	{
		if ( WaterBoundaryBandMap ) return;

		WaterBoundaryBandMap = new int[jY*iX];
		memset(WaterBoundaryBandMap, '\0', sizeof(int)*jY*iX);

		const int xsz = iX-ZERO_AREA_WD;
		const int ysz = jY-ZERO_AREA_WD;

#pragma omp parallel for OMP_SCHEDULE
		for (int ii__ = 0; ii__ < iX*jY; ii__++)
		//for ( int i = ZERO_AREA_WD; i < ysz; i++ )
		{
			int i = ii__ / iX;
			int j = ii__ % iX;
			if (i < ZERO_AREA_WD || i >= ysz || j < ZERO_AREA_WD || j >= xsz)
			{
				continue;
			}
			//for ( int j = ZERO_AREA_WD; j < xsz; j++ )
			{
				WaterBoundaryBandMap[i*iX+j] = (WaterBoundaryBand(i,j,width,1)==true)?1:0;
			}
		}
	}
	void DeleteWaterBoundaryBandMap()
	{
		if ( !WaterBoundaryBandMap ) return;
		delete [] WaterBoundaryBandMap;
		WaterBoundaryBandMap = 0;
	}

	//先端水位境界近傍かチェックする
	inline bool WaterBoundaryBand(int i, int j, int width, int flag = 0) const
	{
#if 10
		//効率重視：遡上を考慮しているときは陸地はいつも「先端水位境界近傍」と考える。
		return (T(i,j) == 0);
#else
		if ( flag == 0 && WaterBoundaryBandMap )
		{
			return (WaterBoundaryBandMap[i*iX+j] == 1)?true:false;
		}

		bool check = false;
		if ( RUNUPFLG(ii,jj) != IS_DRY )
		{
			for ( int ii = i-width; ii < i+width; ii++ )
			{
				if ( ii < ZERO_AREA_WD ) continue;
				if ( ii >= jY-ZERO_AREA_WD ) continue;
				for ( int jj = j-width; jj < j+width; jj++ )
				{
					if ( jj < ZERO_AREA_WD ) continue;
					if ( jj >= iX-ZERO_AREA_WD ) continue;

					if ( RUNUPFLG(ii,jj) == IS_DRY )
					{
						check = true;
						break;
					}
					if ( check ) break;
				}
			}
		}else 
		{
			for ( int ii = i-width; ii < i+width; ii++ )
			{
				if ( ii < ZERO_AREA_WD ) continue;
				if ( ii >= jY-ZERO_AREA_WD ) continue;
				for ( int jj = j-width; jj < j+width; jj++ )
				{
					if ( jj < ZERO_AREA_WD ) continue;
					if ( jj >= iX-ZERO_AREA_WD ) continue;

					if ( RUNUPFLG(ii,jj) != IS_DRY)
					{
						check = true;
						break;
					}
					if ( check ) break;
				}
			}
		}
		return check;
#endif
	}

	int Iterat();
	int Iterat(int method);

	void dummyInitialW(int step, int n, double** ww)
	{
		if ( *ww == NULL )
		{
			*ww = new double[jY*iX];
		}

		for ( int i = 0; i < jY; i++ )
		{
			for ( int j = 0; j < iX; j++ )
			{
				double dw = step*W(i,j)/(double)(n);
				(*ww)[i*iX + j] += dw; 
			}
		}
	}

	double InitialHeght(double leng, double delta);

	inline void MaxHeightUpdate(int i, int j)
	{
		if ( isAbsorbingZoneBounray(i,j)) return;
		if (T(i, j) != 0)
		{
			if (T(i + 1, j - 1) == 0 || T(i + 1, j) == 0 || T(i + 1, j + 1) == 0 ||
				T(i, j - 1) == 0 || T(i, j + 1) == 0 ||
				T(i - 1, j - 1) == 0 || T(i - 1, j - 1) == 0 || T(i - 1, j + 1) == 0)
			{
				if (maxheight_cur < W(i, j))
				{
					maxheight_cur = W(i, j);
				}
				if (maxheight < W(i, j))
				{
					maxheight = W(i, j);
					maxheightPos[0] = i;
					maxheightPos[1] = j;
				}
			}
		}
	}
	inline void MaxHeight2Update(int i, int j)
	{
		if ( isAbsorbingZoneBounray(i,j)) return;
		if ( T(i,j) == 0 )
		{
			if ( maxheight_w_cur2 < W(i,j))
			{
				maxheight_w_cur2 = W(i,j);
				if (ELV(i, j) < 0) maxheight_w_cur2 += -ELV(i, j);
			}
			if ( maxheight_w2 < W(i,j) )
			{
				maxheight_w2 = W(i,j);
				if (ELV(i, j) < 0) maxheight_w2 += -ELV(i, j);
			}

			if ( isWet(i,j) )
			{
				if ( maxheight_cur2 < ELV(i,j))
				{
					maxheight_cur2 = ELV(i,j);
				}
				if ( maxheight2 < ELV(i,j) )
				{
					maxheight2 = ELV(i,j);
				}
			}
		}
	}

	double MaxHeight( int& ii, int& jj)
	{
		double max = 0.0;
		for ( int i = 1; i < jY-1; i++ )
		{
			for ( int j = 1; j < iX-1; j++ )
			{
				if ( isAbsorbingZoneBounray(i,j)) continue;
				if ( T(i,j) == 0 || T(i-1,j) == 0 || T(i-1,j-1) == 0 ||  T(i,j-1) == 0 )
				{
					if ( max < W(i,j) )
					{
						max = (W(i,j)+ W(i-1,j)+ W(i-1,j-1)+ W(i,j-1))/4.0;
						ii = i;
						jj = j;
					}
				}
			}
		}
		return max;
	}


	inline double Water_particle_velocity(int i, int j) const
	{
		const double F = pow(D2(i, j), 3.0) / 3.0;
		//ソリトン分裂波の砕波変形に関する水理実験と数値計算
		double mn[9];
		mn[0] = M(i,j-2)/D2(i,j-2);
		mn[1] = M(i,j-1)/D2(i,j-1);
		mn[2] = M(i,j)/D2(i,j);
		mn[3] = M(i,j+1)/D2(i,j+1);
		mn[4] = M(i,j+2)/D2(i,j+2);
		//double us = M(i,j)/D2(i,j) - (CentralDiff2(2, mn)/(dx*dx))*F;
		const double us = M(i, j) / D2(i, j) - (DIFF2( mn) / (dx*dx))*F;
		

		mn[0] = N(i-2,j)/D2(i-2,j);
		mn[1] = N(i-1,j)/D2(i-1,j);
		mn[2] = N(i,j)/D2(i,j);
		mn[3] = N(i+1,j)/D2(i+1,j);
		mn[4] = N(i+2,j)/D2(i+2,j);
		//double vs = N(i,j)/D2(i,j) - (CentralDiff2(2, mn)/(dy*dy))*F;
		const double vs = N(i, j) / D2(i, j) - (DIFF2(mn) / (dy*dy))*F;

		if ( W(i,j) < 0.0 || H2(i,j) <= 0.0 )
		{
			return 0.0;
		}

		//double C = sqrt(g*H(i,j))*(1.0+0.5*W(i,j)/H(i,j));
		//double s = sqrt(SQR(us)+SQR(vs))/C;
		double C = (g*H2(i,j))*(1.0+0.5*W(i,j)/H2(i,j));
		double s = (SQR(us)+SQR(vs))/C;
		return s;
	}


	unsigned char* gap_flg;
	void CreateGapFlg(double gapvalue=0.0)
	{
		const int ysz = jY;
		const int xsz = iX;
		gap_flg = new unsigned char[xsz*ysz];

		memset(gap_flg, '\0', sizeof(unsigned char)*xsz*ysz);

		double gap_max_value = gap_max;
		if ( gapvalue > 0.0001 )
		{
			gap_max_value = gapvalue;
		}

#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 1; i < ysz-1; i++ )
		{
			for ( int j = 1; j < xsz-1; j++ )
			{
			
				double max;
				double dd;
				bool tflg = false;	//陸地か
				bool flg = false;	//最低水深か
				if ( Data->T(i,j) == 0 )
				{
					dd = Data->ELV(i, j);
					//dd = 0.0;
					tflg = true;
				}else
				{
					dd = -Data->H(i, j);
					if ( -dd < gap_depth ) flg = true;
				}

				const int w = 2;
				bool flg2 = false;	//水域か
				max = -999999999.0;
				for ( int k = -w; k <= w; k++ )
				{
					for ( int kk = -w; kk <= w; kk++ )
					{
						if ( k+i < 0 || k+i >= ysz || kk+j < 0 || kk+j >= xsz )
						{
							continue;
						}
						double d;
						if ( Data->T(i+k,j+kk) == 0 )
						{
							d = Data->ELV((i+k), j+kk);
							//d = 0.0;
						}else
						{
							d = -Data->H((i+k), j+kk);
							flg2 = true;
						}
						d = fabs(d - dd);
						if ( d > max ) max = d;
					}
				}

				//ギャップが浅い水域にあるか陸地境界で隣接海域でギャップがある場合
				if ( max > gap_max && (flg || (tflg && flg2)) )
				{
					gap_flg[i*xsz+j] = GAP_1;
				}
			}
		}

		for ( int i = 1; i < ysz-1; i++ )
		{
			for ( int j = 1; j < xsz-1; j++ )
			{
				if ( gap_flg[i*xsz+j] & GAP_1)
				{
					const int d = 2;
					for ( int k = -d; k <= d; k++ )
					{
						for ( int kk = -d; kk <= d; kk++ )
						{
							if ( k+i < 0 || k+i >= ysz || kk+j < 0 || kk+j >= xsz )
							{
								continue;
							}
							if ( gap_flg[(k+i)*xsz+(kk+j)] == 0 ) gap_flg[(k+i)*xsz+(kk+j)] = GAP_4;
						}
					}
				}
			}
		}
	}

	inline void DeleteGapFlg()
	{
		if ( gap_flg ) delete [] gap_flg;
		gap_flg = 0;
	}
	inline unsigned char& GAP(int i, int j) const
	{
		return gap_flg[i*iX+j];
	}
	inline void GapFlgReste(unsigned char flgBit)
	{
		const int ysz = jY;
		const int xsz = iX;
#pragma omp parallel for OMP_SCHEDULE
		for ( int i = 0; i < ysz; i++ )
		{
			for ( int j = 0; j < xsz; j++ )
			{
				if ( GAP(i,j) & flgBit ) GAP(i,j) &= ~flgBit;
			}
		}
	}

	char* CheckerBord[3];
	inline void CheckerBordCheckSetup()
	{
		if ( !CheckerBord[0] )
		{
			CheckerBord[0] = new char[ jY* iX];
		}
		if ( !CheckerBord[1] )
		{
			CheckerBord[1] = new char[ jY* iX];
		}
		if ( !CheckerBord[2] )
		{
			CheckerBord[2] = new char[ jY* iX];
		}
		memset(CheckerBord[0], '\0', sizeof(char)*jY* iX);
		memset(CheckerBord[1], '\0', sizeof(char)*jY* iX);
		memset(CheckerBord[2], '\0', sizeof(char)*jY* iX);
	}

	inline char& isCheckerBord(int s, int i, int j)
	{
		return CheckerBord[s][i*iX + j];
	}

	inline void CheckerBordCheckTerm()
	{
		if ( CheckerBord[0] ) delete [] CheckerBord[0];
		CheckerBord[0] = 0;
		if ( CheckerBord[1] ) delete [] CheckerBord[1];
		CheckerBord[1] = 0;
		if ( CheckerBord[2] ) delete [] CheckerBord[2];
		CheckerBord[2] = 0;
	}

#define CHECKER_BORD_PATTERN_MAX	12
#define CHECKER_BORD_PATTERN_NUM	6
	//チェッカーボード模様にになっているかパターンマッチング
	inline void CheckerBordCheck( int i, int j, int flag=-1) const
	{
#include "array_expand.h"

		////チェッカーボード模様のパターン
		const double check[CHECKER_BORD_PATTERN_MAX][3][3] = {
			//チェッカーボード状
			{ { 0, 1, 0 }, { 1, 0, 1 }, { 0, 1, 0 } },	//1
			{ { 1, 0, 1 }, { 0, 1, 0 }, { 1, 0, 1 } },	//2

			//直線(横)
			{ { 0, 0, 0 }, { 1, 1, 1 }, { 0, 0, 0 } },	//9
			{ { 1, 1, 1 }, { 0, 0, 0 }, { 1, 1, 1 } },	//10

			//直線(縦)
			{ { 0, 1, 0 }, { 0, 1, 0 }, { 0, 1, 0 } },	//11
			{ { 1, 0, 1 }, { 1, 0, 1 }, { 1, 0, 1 } },	//12 == CHECKER_BORD_PATTERN_MAX

			{ { 0, 1, 0 }, { 0, 0, 0 }, { 0, 1, 0 } },	//3
			{ { 0, 0, 0 }, { 1, 0, 1 }, { 0, 0, 0 } },	//4

			{ { 1, 0, 1 }, { 0, 0, 0 }, { 1, 0, 1 } },	//5
			{ { 0, 1, 0 }, { 1, 1, 1 }, { 0, 1, 0 } },	//6

			//ピーク（特異点)
			{ { 0, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0 } },	//7
			{ { 1, 1, 1 }, { 1, 0, 1 }, { 1, 1, 1 } }	//8

		};

		double hei_w[3][3] = {{W__(i+1,j-1),W__(i+1,j),W__(i+1,j+1)},
							{W__(i,j-1),  W__(i,j),  W__(i,j+1)},
							{W__(i-1,j-1),W__(i-1,j),W__(i-1,j+1)}};

		double hei_m[3][3] = {{M__(i+1,j-1),M__(i+1,j),M__(i+1,j+1)},
							{M__(i,j-1),  M__(i,j),  M__(i,j+1)},
							{M__(i-1,j-1),M__(i-1,j),M__(i-1,j+1)}};

		double hei_n[3][3] = {{N__(i+1,j-1),N__(i+1,j),N__(i+1,j+1)},
							{N__(i,j-1),  N__(i,j),  N__(i,j+1)},
							{N__(i-1,j-1),N__(i-1,j),N__(i-1,j+1)}};


		double min[3] = {99999999.0, 99999999.0, 99999999.0};
		double max[3] = {-999999999.0, -999999999.0, -999999999.0};
		for ( int ii = -1; ii <= 1; ii++ )
		{
			for ( int jj = -1; jj <= 1; jj++ )
			{
				if ( i+ii < 0 || i+ii>=JY || j+jj < 0 || j+jj >= IX )
				{
					
					continue;
				}
				const double w = W__(i+ii,j+jj);
				const double m = M__(i+ii,j+jj);
				const double n = N__(i+ii,j+jj);
				if ( min[0] > w ) min[0] = w;
				if ( max[0] < w ) max[0] = w;
				if ( min[1] > m ) min[1] = m;
				if ( max[1] < m ) max[1] = m;
				if ( min[2] > n ) min[2] = n;
				if ( max[2] < n ) max[2] = n;
			}
		}

		double e[CHECKER_BORD_PATTERN_NUM][3];
		for (int k = 0; k < CHECKER_BORD_PATTERN_NUM; k++)
		{
			e[k][0] = e[k][1] = e[k][2] = 0.0;
		}

		const double d[3] = { max[0] - min[0], max[1] - min[1], max[2] - min[2] };

		//W,M,Nに対して値を正規化してパターンとの二乗差を計算
		int s_s = 0;
		int s_e = 3;
		if ( flag == 0 )
		{
			s_s = 1;
		}
		if ( flag == 1 )
		{
			s_e = 1;
		}
		for ( int s = s_s; s < s_e; s++ )
		{
			//if ( fabs( d[s] ) > CHECKER_BORD_THESHOLD2 )
			if ( d[s]  > CHECKER_BORD_THESHOLD2 || d[s]  < -CHECKER_BORD_THESHOLD2)
			{
				const double invd = 1.0/d[s];
				for ( int ii = 0; ii < 3; ii++ )
				{
					for ( int jj = 0; jj < 3; jj++ )
					{
						if ( s == 0 )
						{
							hei_w[ii][jj] = (hei_w[ii][jj] - min[s])*invd;
							for (int k = 0; k < CHECKER_BORD_PATTERN_NUM; k++)
							{
								const double d = (check[k][ii][jj] - hei_w[ii][jj]);
								e[k][s] += (d*d);
							}
						}
						if ( s == 1 )
						{
							hei_m[ii][jj] = (hei_m[ii][jj] - min[s])*invd;
							for (int k = 0; k < CHECKER_BORD_PATTERN_NUM; k++)
							{
								const double d = (check[k][ii][jj] - hei_m[ii][jj]);
								e[k][s] += (d*d);
							}
						}
						if ( s == 2 )
						{
							hei_n[ii][jj] = (hei_n[ii][jj] - min[s])*invd;
							for (int k = 0; k < CHECKER_BORD_PATTERN_NUM; k++)
							{
								const double d = (check[k][ii][jj] - hei_n[ii][jj]);
								e[k][s] += (d*d);
							}
						}
					}

				}

				bool chackerbord = false;
				for (int k = 0; k < CHECKER_BORD_PATTERN_NUM; k++)
				{
					if (k < 2)
					{
						chackerbord = chackerbord || (e[k][s] < checker_bord_theshold);
					}
					else
					{
						chackerbord = chackerbord || (e[k][s] < CHECKER_BORD_THESHOLD11);
					}
				}

				//パターンと一致すると判定された場合
				if (chackerbord)
				{
#if 0
					const int d = 2;
					//printf("min %f max %f\n", min, max);
					//printf("e1 %f e2 %f\n", e1, e2);
					for ( int ii = -d; ii <= d; ii++ )
					{
						for ( int jj = -d; jj <= d; jj++ )
						{
							if ( i+ii < 0 || i+ii>=JY || j+jj < 0 || j+jj >= IX )
							{
								continue;
							}
//#pragma omp critical
							{
								CheckerBord[s][(i+ii)*IX + j+jj] += 1;
							}
						}
					}
#else
					//CheckerBord[s][(i-2)*IX + (j-2)] += 1;
					//CheckerBord[s][(i-2)*IX + (j-1)] += 1;
					//CheckerBord[s][(i-2)*IX + (j-0)] += 1;
					//CheckerBord[s][(i-2)*IX + (j+1)] += 1;
					//CheckerBord[s][(i-2)*IX + (j+2)] += 1;

					//CheckerBord[s][(i-1)*IX + (j-2)] += 1;
					//CheckerBord[s][(i-1)*IX + (j-1)] += 1;
					//CheckerBord[s][(i-1)*IX + (j-0)] += 1;
					//CheckerBord[s][(i-1)*IX + (j+1)] += 1;
					//CheckerBord[s][(i-1)*IX + (j+2)] += 1;

					//CheckerBord[s][(i-0)*IX + (j-2)] += 1;
					//CheckerBord[s][(i-0)*IX + (j-1)] += 1;
					//CheckerBord[s][(i-0)*IX + (j-0)] += 1;
					//CheckerBord[s][(i-0)*IX + (j+1)] += 1;
					//CheckerBord[s][(i-0)*IX + (j+2)] += 1;

					//CheckerBord[s][(i+1)*IX + (j-2)] += 1;
					//CheckerBord[s][(i+1)*IX + (j-1)] += 1;
					//CheckerBord[s][(i+1)*IX + (j-0)] += 1;
					//CheckerBord[s][(i+1)*IX + (j+1)] += 1;
					//CheckerBord[s][(i+1)*IX + (j+2)] += 1;

					//CheckerBord[s][(i+2)*IX + (j-2)] += 1;
					//CheckerBord[s][(i+2)*IX + (j-1)] += 1;
					//CheckerBord[s][(i+2)*IX + (j-0)] += 1;
					//CheckerBord[s][(i+2)*IX + (j+1)] += 1;
					//CheckerBord[s][(i+2)*IX + (j+2)] += 1;



					CheckerBord[s][i*IX + j] += 1;
					CheckerBord[s][(i+1)*IX + (j-1)] += 1;
					CheckerBord[s][(i-1)*IX + (j-1)] += 1;
					CheckerBord[s][(i-1)*IX + (j+1)] += 1;
					CheckerBord[s][(i+1)*IX + (j+1)] += 1;

					CheckerBord[s][(i+1)*IX + j] += 1;
					CheckerBord[s][(i-1)*IX + j] += 1;
					CheckerBord[s][i*IX + (j+1)] += 1;
					CheckerBord[s][i*IX + (j-1)] += 1;
#endif
				}
			}
		}
	}

	inline double Amplitude(const int bw, int i, int j, bool& top) const
	{
#include "array_expand.h"
		const double wij = W__(i,j);
		double min = wij;
		double max = min;
		int ki = -1;
		int kj = -1;
		top = false;

		for ( int is = -bw; is <= bw; is++ )
		{
			if ( i+is < 0 || i+is >= jY ) continue;
			const double w1 = W__(i+is,j);
			if ( min > w1 ) min = w1;
			if ( max < w1 ) max = w1;

			if ( j+is < 0 || j+is >= iX ) continue;
			const double w2 = W__(i,j+is);
			if ( min > w2 ) min = w2;
			if ( max < w2 ) max = w2;
		}
		if ( max > 0.0 && fabs(max-wij) < 0.01  ) top = true;	//波峰

		if ( min > 0.0 ) min = 0.0;
		return max - min;
	}


	inline double Dispersion_correction_coefficient(const int i, const int j) const
	{
		return dispersion_correction_coefficient[i*iX+j];
	}
	inline void Dispersion_correction_coefficient(const double ds)
	{
		if ( dispersion_correction_coefficient )
		{
			return;
		}

		dispersion_correction_coefficient = new double[iX*jY];
		memset(dispersion_correction_coefficient, '\0', sizeof(double)*iX*jY);

#include "array_expand.h"
		const double inv_log_exp = 0.76146285961465999797147703;
		const double inv_exp1 = 0.36787944117144232159552377;
		const double a = DISPERSIN_TERM_COEF;

		for ( int i = 1; i < jY-1; i++ )
		{
			for ( int j = 1; j < iX-1; j++ )
			{
				double *r = &(dispersion_correction_coefficient[i*iX+j]);

				const int bw = 3;
				double hmax = -999999.0;

				for ( int ii = -bw; ii <= bw; ii++ )
				{
					if ( i+ii < 0 || i+ii >= JY ) continue;
					for ( int jj = -bw; jj <= bw; jj++ )
					{
						if ( j+jj < 0 || j+jj >= IX ) continue;

						if ( H2__(i+ii, j+jj) > hmax ) hmax =  H2__(i+ii, j+jj);
					}
				}
				const double b = 1.0;	//0.5;
				double h = hmax;
				if ( a*b*ds*inv_log_exp >= h )
				{
					*r = 1.0;
					continue;
				}

				double c =inv_exp1*(exp(a*b*ds/h) - 1.0);
				if ( c < 0.0 ) c = 0.0;
				if ( c > 1.0 ) c = 1.0;
				*r = SQR(c);
			}
		}
	}
	void AbnormalFilter(AbeNoguera* p);

	inline void advection_term_coef(int i, int j, double* mm, int oder)
	{
		return;

		const double value_gap_max = ADVECTION_TERM_GAP_CUTAMX;

		if ( oder == 3 )
		{
			int id1 = 0;
			int id2 = 0;
			double min = 9999999999999.0;
			double max = -9999999999999.0;

			if ( fabs(mm[0]) < min ) {min = fabs(mm[0]); id1 = 0;} 
			if ( fabs(mm[1]) < min ) {min = fabs(mm[1]); id1 = 1;} 
			if ( fabs(mm[2]) < min ) {min = fabs(mm[2]); id1 = 2;} 
			if ( fabs(mm[3]) < min ) {min = fabs(mm[3]); id1 = 3;} 
			if ( fabs(mm[5]) < min ) {min = fabs(mm[5]); id1 = 5;} 
			if ( fabs(mm[6]) < min ) {min = fabs(mm[6]); id1 = 6;} 

			if ( fabs(mm[0]) > max ) {max = fabs(mm[0]); id2 = 0;} 
			if ( fabs(mm[1]) > max ) {max = fabs(mm[1]); id2 = 1;} 
			if ( fabs(mm[2]) > max ) {max = fabs(mm[2]); id2 = 2;} 
			if ( fabs(mm[3]) > max ) {max = fabs(mm[3]); id2 = 3;} 
			if ( fabs(mm[5]) > max ) {max = fabs(mm[5]); id2 = 5;} 
			if ( fabs(mm[6]) > max ) {max = fabs(mm[6]); id2 = 6;} 

			double sumall = (fabs(mm[0])+fabs(mm[1])+fabs(mm[2])+fabs(mm[3])+fabs(mm[5])+fabs(mm[6]));
			double av = (sumall -min - max)/4.0;

			double coef = 1.0;
			if ( max > 0.0001 )
			{
				if ( value_gap_max*min < max )
				{
					 //printf("max %f %f %f\n", min, max, av);
					 coef = 6.0*av/sumall;
				}
				else if ( value_gap_max*av < max )
				{
					//printf("av %f %f %f\n", min, max, av);
					coef = 6.0*av/sumall;
				}
				if ( coef < 1.0)
				{
					mm[0] *= coef;
					mm[1] *= coef;
					mm[2] *= coef;
					mm[3] *= coef;
					mm[5] *= coef;
					mm[6] *= coef;
				}
			}
		}
		if ( oder == 1 )
		{
			int id1 = 0;
			int id2 = 0;

			double min = 9999999999999.0;
			double max = -9999999999999.0;

			if ( fabs(mm[0]) < min ) {min = fabs(mm[0]); id1 = 0;} 
			if ( fabs(mm[1]) < min ) {min = fabs(mm[1]); id1 = 1;} 
			if ( fabs(mm[3]) < min ) {min = fabs(mm[3]); id1 = 2;} 
			if ( fabs(mm[4]) < min ) {min = fabs(mm[4]); id1 = 3;}

			if ( fabs(mm[0]) > max ) {max = fabs(mm[0]); id2 = 0;} 
			if ( fabs(mm[1]) > max ) {max = fabs(mm[1]); id2 = 1;} 
			if ( fabs(mm[3]) > max ) {max = fabs(mm[3]); id2 = 2;} 
			if ( fabs(mm[4]) > max ) {max = fabs(mm[4]); id2 = 3;} 

			double sumall = (fabs(mm[0])+fabs(mm[1])+fabs(mm[3])+fabs(mm[4]));
			double av = (sumall - min - max)/2.0;

			double coef = 1.0;
			if ( max > 0.0001 )
			{
				if ( value_gap_max*min < max )
				{
					 //printf("max %f %f %f\n", min, max, av);
					 coef = 4.0*av/sumall;
				}
				else if ( value_gap_max*av < max )
				{
					//printf("av %f %f %f\n", min, max, av);
					coef = 4.0*av/sumall;
				}
				if ( coef < 1.0 )
				{
					mm[0] *= coef;
					mm[1] *= coef;
					mm[3] *= coef;
					mm[4] *= coef;
				}
			}
		}
	}
};

// 1階微分 (dx で割ること!!
//fの値は左から（格子番号の小さい順 */
inline double CentralDiff(const int odr, const double* f)
{
	if ( odr <= 1 )
	{
		//-1  +1
		return 0.5*(-f[0] + f[1]);
	}
	if ( odr <= 2 )
	{
		//-2 -1  +1 +2
		return (f[0] - 8*f[1] + 8*f[2] - f[3])*C1_12;
	}
	if ( odr <= 3 )
	{
		// -3 -2 -1  +1 +2 +3
		return (-f[0] + 9*f[1] -45*f[2] + 45*f[3] - 9*f[4] + f[5])*C1_60;
	}
	if ( odr <= 4 )
	{
		// -4 -3 -2 -1  +1 +2 +3 +4
		return (3*f[0] - 32*f[1] +168*f[2] - 672*f[3] + 672*f[4] - 168*f[5] + 32*f[6] -3*f[7])*C1_840;
	}
	if ( odr <= 5 )
	{
		// -5 -4 -3 -2 -1  +1 +2 +3 +4 +5
		return (-2*f[0] + 25*f[1] -150*f[2] + 600*f[3] - 2100*f[4] + 2100*f[5] - 600*f[6] +150*f[7] - 25*f[8] + 2*f[9])*C1_2520;
	}
	return 0.0;
}


// 2階微分 (dx^2 で割ること!!
//fの値は左から（格子番号の小さい順 */
inline double CentralDiff2(const int odr, const double* f)
{
	if ( odr <= 1 )
	{
		//-1 0  +1
		return f[0] -2*f[1] + f[2];
	}
	if ( odr <= 2 )
	{
		//-2 -1  0 +1 +2
		return (-f[0] + 16 * (f[1] + f[3] )- 30 * f[2] - f[4])*C1_12;
	}
	if ( odr <= 3 )
	{
		// -3 -2 -1  0 +1 +2 +3
		return (2*f[0] -27*f[1] + 270*f[2] - 490*f[3] + 270*f[4] - 27*f[5] + 2*f[6])*C1_180;
	}
	if ( odr <= 4 )
	{
		// -4 -3 -2 -1 0 +1 +2 +3 +4
		return (-9*f[0] + 128*f[1] - 1008*f[2] + 8064*f[3] - 14350*f[4] + 8064*f[5] - 1008*f[6] + 128*f[7] - 9*f[8])*C1_5040;
	}
	return 0.0;
}

// 3階微分 (dx^3 で割ること!!
//fの値は左から（格子番号の小さい順 */
inline double CentralDiff3(const int odr, const double* f)
{
	if ( odr <= 1 )
	{
		return 0.0;
	}
	if ( odr <= 2 )
	{
		//-2 -1  +1 +2
		return 0.5*(-f[0] + 2*f[1] -2*f[2] +f[3]);
	}
	if ( odr <= 3 )
	{
		// -3 -2 -1   +1 +2 +3
		return (f[0] -8*f[1] + 13*f[2] - 13*f[3] + 8*f[4] - f[5])*C1_08;
	}
	if ( odr <= 4 )
	{
		// -4 -3 -2 -1  +1 +2 +3 +4
		return (-7*f[0] + 72*f[1] - 338*f[2] + 488*f[3] - 488*f[4] + 338*f[5] - 72*f[6] + 7*f[7])*C1_240;
	}
	return 0.0;
}

//fの値は左から（格子番号の小さい順 (BackwardDiffも同じ。引数をf[i+1]かf[i-1]と変えるだけ)*/
inline double ForwardDiff(const int odr, const double* f)
{
	if ( odr <= 1 )
	{
		//0  +1
		return (-f[0] + f[1]);
	}
	if ( odr <= 2 )
	{
		//0  +1 +2
		return 0.5*(-3*f[0] + 4*f[1] - f[2]);
	}
	if ( odr <= 3 )
	{
		// 0  +1 +2 +3
		return (-11*f[0] + 18*f[1] -9*f[2] + 2*f[3])*C1_06;
	}
	if ( odr <= 4 )
	{
		// 0  +1 +2 +3 +4
		return (-25*f[0] + 48*f[1] -36*f[2] + 16*f[3] -3*f[4])*C1_12;
	}
	if ( odr <= 5 )
	{
		// 0  +1 +2 +3 +4 +5
		return (-137*f[0] + 300*f[1] -300*f[2] +200*f[3] -75*f[4] +12*f[5])*C1_60;
	}
	return 0.0;
}

inline double BackwardDiff(const int odr, const double* f)
{
	if ( odr <= 1 )
	{
		//-1 0
		return (-f[0] + f[1]);
	}
	if ( odr <= 2 )
	{
		//-2 -1 0
		return 0.5*(3*f[2] - 4*f[1] + f[0]);
	}
	if ( odr <= 3 )
	{
		// -3 -2 -1 0
		return (11*f[3] - 18*f[2] + 9*f[1] - 2*f[0])*C1_06;
	}
	if ( odr <= 4 )
	{
		// -4 -3 -2 0
		return (25*f[4] - 48*f[3] + 36*f[2] - 16*f[1] + 3*f[0])*C1_12;
	}
	if ( odr <= 5 )
	{
		// -5 -4 -3 -2 0
		return (-12*f[5] +75*f[4] - 200*f[3] + 137*f[2] + 300*f[1] - 300*f[0])*C1_60;
	}
	return 0.0;
}

//Δf/Δt = (-f[t-1] + f[t])/Δt = F(...)
//Δf/Δt = (-f[0] + f[1])/Δt = F(...)
// f[t] = f[t-1] + ΔtF(...)
// f[1] = f[0] + ΔtF(...)
//
//0.5*(3*f[2] - 4*f[1] + f[0])/Δt = F(...)
//f[2] = (4f[1] - f[0])/3 + (2/3)ΔtF(...)
//
//(11*f[3] - 18*f[2] + 9*f[1] - 2*f[0])*C1_06/Δt = F(...)
//f[3] = (18*f[2] - 9*f[1] + 2*f[0])/11 + (6/11)ΔtF(...)
//
//(25*f[4] - 48*f[3] + 36*f[2] - 16*f[1] + 3*f[0])*C1_12/Δt = F(...)
//f[4]  = ( 48*f[3] - 36*f[2] + 16*f[1] - 3*f[0])/25 + (12/25)ΔtF(...)
inline double BackwardDiffE(int odr, double* f, double& coef)
{
	if ( odr <= 1 )
	{
		coef = 1.0;
		//-1 0
		return f[0];
	}
	if ( odr <= 2 )
	{
		coef = 2.0/3.0;
		//-2 -1 0
		return (-f[0] + 4*f[1])/3.0;
	}
	if ( odr <= 3 )
	{
		coef = 6.0/11.0;
		// -3 -2 -1 0
		return (2*f[0] + 18*f[2] - 9*f[1])/11.0;
	}
	if ( odr <= 4 )
	{
		coef = 12.0/25.0;
		// -4 -3 -2 0
		return (-3*f[0] + 48*f[3] - 36*f[2] + 16*f[1])/25.0;
	}
	return 0.0;
}
//fの値は左から（格子番号の小さい順) */
inline double UpWindDiff(int odr, int sgn, double* f)
{
	if ( odr <=1 )
	{
		return -f[0] + f[1];
	}
	if ( odr <= 2 )
	{
		//-2 -1 0
		if ( sgn > 0 )	return 0.5*(f[0] - 4*f[1] + 3.0*f[2]);
		else			return 0.5*(-f[2] + 4*f[1] - 3.0*f[0]);
	}
	if ( odr <= 3 )
	{
		//-3 -2 -1 0 
		if ( sgn > 0 )	return (f[0] - 6*f[1] + 3*f[2] + 2*f[3])*C1_06;
		else			return (-f[3] + 6*f[2] - 3*f[1] - 2*f[0])*C1_06;
	}
	if ( odr <= 4 )
	{
		if ( sgn > 0 )	return (-5*f[0] + 30*f[1] - 90*f[2] + 50*f[3] + 15*f[4])*C1_60;
		else			return ( 5*f[4] - 30*f[3] + 90*f[2] - 50*f[1] - 15*f[0])*C1_60;
	}
	if ( odr <= 5 )
	{
		if ( sgn > 0 )	return (-2*f[0] + 15*f[1] - 60*f[2] + 20*f[3] + 30*f[4] - 3*f[5])/60.0;
		else			return ( 2*f[5] - 15*f[4] + 60*f[3] - 20*f[2] - 30*f[1] + 3*f[0])/60.0;
	}

	return 0.0;
}

inline double WENO1d(double* f, int i)
{
	double* g = f+3;
	return (g[i] - g[i-1]);
}

inline double WENO1(double* f)
{
	double p[3];

	p[0] = WENO1d(f, -2)/3.0 +( -7.0*WENO1d(f, -1) + 11.0*WENO1d(f, 0))/6.0;
	p[1] = (-WENO1d(f,-1) + 5.0*WENO1d(f, 0))/6.0 + WENO1d(f, 1)/3.0;
	p[2] = WENO1d(f,0)/3.0 +( 5.0*WENO1d(f, 1) - WENO1d(f, 2))/6.0;

	double s1, s2, s3;

	s1 = (13.0/12.0)*pow(WENO1d(f,-2)-2.0*WENO1d(f,-1)+WENO1d(f,0),2)+0.25*(pow(WENO1d(f,-2)-4.0*WENO1d(f,-1)+3.0*WENO1d(f,0),2));
	s2 = (13.0/12.0)*pow(WENO1d(f,-1)-2.0*WENO1d(f,0)+WENO1d(f,1),2)+0.25*(pow(WENO1d(f,-1)-WENO1d(f,1),2));
	s3 = (13.0/12.0)*pow(WENO1d(f,0)-2.0*WENO1d(f,1)+WENO1d(f,2),2)+0.25*(pow(3.0*WENO1d(f,0)-4.0*WENO1d(f,1)+WENO1d(f,2),2));

	const double eps = 0.000001;

	double a1 = 0.1/SQR(s1+eps);
	double a2 = 0.6/SQR(s2+eps);
	double a3 = 0.3/SQR(s3+eps);

	double w = a1 + a2 + a3;
	double w1 = a1/w;
	double w2 = a2/w;
	double w3 = a3/w;


	return w1*p[0] + w2*p[1] + w3*p[2];
}


#define LELEFILTER_10(M,i,j) ff*M(i+5,j+5)+e*f*M(i+5,j+4)+d*f*M(i+5,j+3)+c*f*M(i+5,j+2)+b*f*\
    M(i+5,j+1)+a*f*M(i+5,j)+b*f*M(i+5,j-1)+c*f*M(i+5,j-2)+d*f*M(i+5,j-3)+e*f*\
    M(i+5,j-4)+ff*M(i+5,j-5)+e*f*M(i+4,j+5)+ee*M(i+4,j+4)+d*e*\
    M(i+4,j+3)+c*e*M(i+4,j+2)+b*e*M(i+4,j+1)+a*e*M(i+4,j)+b*e*M(i+4,j-1)+c*e*\
    M(i+4,j-2)+d*e*M(i+4,j-3)+ee*M(i+4,j-4)+e*f*M(i+4,j-5)+d*f*\
    M(i+3,j+5)+d*e*M(i+3,j+4)+dd*M(i+3,j+3)+c*d*M(i+3,j+2)+b*d*\
    M(i+3,j+1)+a*d*M(i+3,j)+b*d*M(i+3,j-1)+c*d*M(i+3,j-2)+dd*M(i+3,j-3)+d*\
    e*M(i+3,j-4)+d*f*M(i+3,j-5)+c*f*M(i+2,j+5)+c*e*M(i+2,j+4)+c*d*\
    M(i+2,j+3)+cc*M(i+2,j+2)+b*c*M(i+2,j+1)+a*c*M(i+2,j)+b*c*\
    M(i+2,j-1)+cc*M(i+2,j-2)+c*d*M(i+2,j-3)+c*e*M(i+2,j-4)+c*f*\
    M(i+2,j-5)+b*f*M(i+1,j+5)+b*e*M(i+1,j+4)+b*d*M(i+1,j+3)+b*c*\
    M(i+1,j+2)+bb*M(i+1,j+1)+a*b*M(i+1,j)+bb*M(i+1,j-1)+b*c*\
    M(i+1,j-2)+b*d*M(i+1,j-3)+b*e*M(i+1,j-4)+b*f*M(i+1,j-5)+a*f*M(i,j+5)+a*e*\
    M(i,j+4)+a*d*M(i,j+3)+a*c*M(i,j+2)+a*b*M(i,j+1)+aa*M(i,j)+a*b*\
    M(i,j-1)+a*c*M(i,j-2)+a*d*M(i,j-3)+a*e*M(i,j-4)+a*f*M(i,j-5)+b*f*\
    M(i-1,j+5)+b*e*M(i-1,j+4)+b*d*M(i-1,j+3)+b*c*M(i-1,j+2)+bb*\
    M(i-1,j+1)+a*b*M(i-1,j)+bb*M(i-1,j-1)+b*c*M(i-1,j-2)+b*d*M(i-1,j-3)+b*\
    e*M(i-1,j-4)+b*f*M(i-1,j-5)+c*f*M(i-2,j+5)+c*e*M(i-2,j+4)+c*d*\
    M(i-2,j+3)+cc*M(i-2,j+2)+b*c*M(i-2,j+1)+a*c*M(i-2,j)+b*c*\
    M(i-2,j-1)+cc*M(i-2,j-2)+c*d*M(i-2,j-3)+c*e*M(i-2,j-4)+c*f*\
    M(i-2,j-5)+d*f*M(i-3,j+5)+d*e*M(i-3,j+4)+dd*M(i-3,j+3)+c*d*\
    M(i-3,j+2)+b*d*M(i-3,j+1)+a*d*M(i-3,j)+b*d*M(i-3,j-1)+c*d*\
    M(i-3,j-2)+dd*M(i-3,j-3)+d*e*M(i-3,j-4)+d*f*M(i-3,j-5)+e*f*\
    M(i-4,j+5)+ee*M(i-4,j+4)+d*e*M(i-4,j+3)+c*e*M(i-4,j+2)+b*e*\
    M(i-4,j+1)+a*e*M(i-4,j)+b*e*M(i-4,j-1)+c*e*M(i-4,j-2)+d*e*\
    M(i-4,j-3)+ee*M(i-4,j-4)+e*f*M(i-4,j-5)+ff*M(i-5,j+5)+e*f*\
    M(i-5,j+4)+d*f*M(i-5,j+3)+c*f*M(i-5,j+2)+b*f*M(i-5,j+1)+a*f*M(i-5,j)+b*f*\
    M(i-5,j-1)+c*f*M(i-5,j-2)+d*f*M(i-5,j-3)+e*f*M(i-5,j-4)+ff*M(i-5,j-5)

#define LELEFILTER_8(M,i,j) ee*M(i+4,j+4)+d*e*M(i+4,j+3)+c*e*M(i+4,j+2)+b*e*M(i+4,j+1)+a*e*M(i+4,j)+b*\
    e*M(i+4,j-1)+c*e*M(i+4,j-2)+d*e*M(i+4,j-3)+ee*M(i+4,j-4)+d*e*\
    M(i+3,j+4)+dd*M(i+3,j+3)+c*d*M(i+3,j+2)+b*d*M(i+3,j+1)+a*d*M(i+3,j)+b*\
    d*M(i+3,j-1)+c*d*M(i+3,j-2)+dd*M(i+3,j-3)+d*e*M(i+3,j-4)+c*e*\
    M(i+2,j+4)+c*d*M(i+2,j+3)+cc*M(i+2,j+2)+b*c*M(i+2,j+1)+a*c*M(i+2,j)+b*\
    c*M(i+2,j-1)+cc*M(i+2,j-2)+c*d*M(i+2,j-3)+c*e*M(i+2,j-4)+b*e*\
    M(i+1,j+4)+b*d*M(i+1,j+3)+b*c*M(i+1,j+2)+bb*M(i+1,j+1)+a*b*\
    M(i+1,j)+bb*M(i+1,j-1)+b*c*M(i+1,j-2)+b*d*M(i+1,j-3)+b*e*M(i+1,j-4)+a*\
    e*M(i,j+4)+a*d*M(i,j+3)+a*c*M(i,j+2)+a*b*M(i,j+1)+aa*M(i,j)+a*b*\
    M(i,j-1)+a*c*M(i,j-2)+a*d*M(i,j-3)+a*e*M(i,j-4)+b*e*M(i-1,j+4)+b*d*\
    M(i-1,j+3)+b*c*M(i-1,j+2)+bb*M(i-1,j+1)+a*b*M(i-1,j)+bb*\
    M(i-1,j-1)+b*c*M(i-1,j-2)+b*d*M(i-1,j-3)+b*e*M(i-1,j-4)+c*e*M(i-2,j+4)+c*d*\
    M(i-2,j+3)+cc*M(i-2,j+2)+b*c*M(i-2,j+1)+a*c*M(i-2,j)+b*c*\
    M(i-2,j-1)+cc*M(i-2,j-2)+c*d*M(i-2,j-3)+c*e*M(i-2,j-4)+d*e*\
    M(i-3,j+4)+dd*M(i-3,j+3)+c*d*M(i-3,j+2)+b*d*M(i-3,j+1)+a*d*M(i-3,j)+b*\
    d*M(i-3,j-1)+c*d*M(i-3,j-2)+dd*M(i-3,j-3)+d*e*M(i-3,j-4)+ee*\
    M(i-4,j+4)+d*e*M(i-4,j+3)+c*e*M(i-4,j+2)+b*e*M(i-4,j+1)+a*e*M(i-4,j)+b*e*\
    M(i-4,j-1)+c*e*M(i-4,j-2)+d*e*M(i-4,j-3)+ee*M(i-4,j-4)

#define LELEFILTER_6(M,i,j) dd*M(i+3,j+3)+c*d*M(i+3,j+2)+b*d*M(i+3,j+1)+a*d*M(i+3,j)+b*d*M(i+3,j-1)+c*\
    d*M(i+3,j-2)+dd*M(i+3,j-3)+c*d*M(i+2,j+3)+cc*M(i+2,j+2)+b*c*\
    M(i+2,j+1)+a*c*M(i+2,j)+b*c*M(i+2,j-1)+cc*M(i+2,j-2)+c*d*M(i+2,j-3)+b*\
    d*M(i+1,j+3)+b*c*M(i+1,j+2)+bb*M(i+1,j+1)+a*b*M(i+1,j)+bb*\
    M(i+1,j-1)+b*c*M(i+1,j-2)+b*d*M(i+1,j-3)+a*d*M(i,j+3)+a*c*M(i,j+2)+a*b*\
    M(i,j+1)+aa*M(i,j)+a*b*M(i,j-1)+a*c*M(i,j-2)+a*d*M(i,j-3)+b*d*\
    M(i-1,j+3)+b*c*M(i-1,j+2)+bb*M(i-1,j+1)+a*b*M(i-1,j)+bb*\
    M(i-1,j-1)+b*c*M(i-1,j-2)+b*d*M(i-1,j-3)+c*d*M(i-2,j+3)+cc*\
    M(i-2,j+2)+b*c*M(i-2,j+1)+a*c*M(i-2,j)+b*c*M(i-2,j-1)+cc*M(i-2,j-2)+c*\
    d*M(i-2,j-3)+dd*M(i-3,j+3)+c*d*M(i-3,j+2)+b*d*M(i-3,j+1)+a*d*\
    M(i-3,j)+b*d*M(i-3,j-1)+c*d*M(i-3,j-2)+dd*M(i-3,j-3)


#define LELEFILTER_4(M,i,j) cc*M(i+2,j+2)+b*c*M(i+2,j+1)+a*c*M(i+2,j)+b*c*M(i+2,j-1)+\
	cc*M(i+2,j-2)+b*c*M(i+1,j+2)+bb*M(i+1,j+1)+a*b*M(i+1,j)+bb*M(i+1,j-1)+\
	b*c*M(i+1,j-2)+a*c*M(i,j+2)+a*b*M(i,j+1)+aa*M(i,j)+a*b*M(i,j-1)+a*c*M(i,j-2)+\
	b*c*M(i-1,j+2)+bb*M(i-1,j+1)+a*b*M(i-1,j)+bb*M(i-1,j-1)+b*c*M(i-1,j-2)+cc*M(i-2,j+2)+\
	b*c*M(i-2,j+1)+a*c*M(i-2,j)+b*c*M(i-2,j-1)+cc*M(i-2,j-2)

#define LELEFILTER_2(M,i,j) bb*M(i+1,j+1)+a*b*M(i+1,j)+bb*M(i+1,j-1)+a*b*M(i,j+1)+\
	aa*M(i,j)+a*b*M(i,j-1)+bb*M(i-1,j+1)+a*b*M(i-1,j)+bb*M(i-1,j-1)

#define LELEFILTER_2a(M,i,j) 0.000625*M[i+1][j+1]+0.125*M[i+1][j]+0.000625*M[i+1][j-1]+0.125*M[i][j+1]+\
	0.25*M[i][j]+0.0125*M[i][j-1]+0.000625*M[i-1][j+1]+0.125*M[i-1][j]+0.000625*M[i-1][j-1]


#define LELEFILTER_2DMACRO

/* 符号無し char 型の値と，そのビットの状態を表示する */
inline void show_unsigned_char(Solver& solv, int bit, unsigned char x) 
{
	int i;
	solv.log_printf("[%d] %d ", bit, (int)x);
	for (i = 7; i >= 0; i--) {    
	solv.log_printf("%d", (x>>i) & 1);
	}
	solv.log_printf("\n");
}


//#ifdef _OPENMP
#define USE_OPENMP_1
#define USE_OPENMP_2
#define USE_OPENMP_3
#define USE_OPENMP_4
//#endif

//#define USE_HIGH_ORDER


#endif
