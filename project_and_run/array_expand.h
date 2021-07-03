	const int absorbingZone_ = absorbingZone;
	const int IX = iX;
	const int JY = jY;
	const double g_ = g;

	const double* m__ = Data->m;
	const double* n__ = Data->n;
	const double* w__ = Data->w;
	const double* d2__ = this->d2vale;

	double* fm__ = Data->right.Fm;
	double* fn__ = Data->right.Fn;
	double* fw__ = Data->right.Fw;

	const double* h__ = Data->h;
	const double* elv__ = Data->elevation;
	const int* t__ = Data->topog;
	const double* man__ = Data->manning;

	/*const */double dmin = Dmin;
	const int RunUp_ = RunUp;
