//生の配列データへのアクセスマクロ
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

#define M__(i,j)	(m__[(i)*IX+(j)])
#define N__(i,j)	(n__[(i)*IX+(j)])
#define W__(i,j)	(w__[(i)*IX+(j)])
#define qM__(i,j)	(qm__[(i)*IX+(j)])
#define qN__(i,j)	(qn__[(i)*IX+(j)])
#define qW__(i,j)	(qw__[(i)*IX+(j)])
#define pM__(i,j)	(pm__[(i)*IX+(j)])
#define pN__(i,j)	(pn__[(i)*IX+(j)])
#define pW__(i,j)	(pw__[(i)*IX+(j)])
#define D2__(i,j)	(d2__[(i)*IX+(j)])

#define FM__(i,j)	(fm__[(i)*IX+(j)])
#define FN__(i,j)	(fn__[(i)*IX+(j)])
#define FW__(i,j)	(fw__[(i)*IX+(j)])

#define H__(i,j)	(h__[(i)*IX+(j)])
#define ELV__(i,j)	(elv__[(i)*IX+(j)])
#define T__(i,j)	(t__[(i)*IX+(j)])
#define H2__(i,j)	(( RunUp_ && ELV__(i,j) > 0 && T__(i,j) == 0 ) ? ( riverInfo.isWaterDepth(i,j)? riverInfo.WaterDepth(i,j) :D2__(i,j)): H__(i,j))
#define MAN__(i,j)	(man__[(i)*IX+(j)])

