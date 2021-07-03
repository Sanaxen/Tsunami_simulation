
// array_view配列データへのアクセスマクロへの切り替え
#ifdef GPGPU_CPP_AMP
#undef D2__
#undef M__
#undef N__
#undef W__
#undef ELV__
#undef T__
#undef H__
#undef qM__
#undef qN__
#undef qW__
#undef pM__
#undef pN__
#undef pW__

#define D2__(i,j)	d2_((i),(j))
#define M__(i,j)	mm_((i),(j))
#define N__(i,j)	nn_((i),(j))
#define W__(i,j)	ww_((i),(j))
#define ELV__(i,j)	elv_((i),(j))
#define T__(i,j)	t_((i),(j))
#define H__(i,j)	h_((i),(j))
#define qM__(i,j)	qmm_((i),(j))
#define qN__(i,j)	qnn_((i),(j))
#define qW__(i,j)	qww_((i),(j))
#define pM__(i,j)	pmm_((i),(j))
#define pN__(i,j)	pnn_((i),(j))
#define pW__(i,j)	pww_((i),(j))
#endif
