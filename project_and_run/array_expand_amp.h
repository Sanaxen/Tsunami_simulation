// array_view配列データへのアクセスへの切り替え
#ifdef GPGPU_CPP_AMP

			extent<1> ext(IX*JY);
			array_view<const float,1> ww_(ext, w__);
			array_view<const float,1> mm_(ext, m__);
			array_view<const float,1> nn_(ext, n__);
			array_view<const float,1> ma_(ext, man__);
			array_view<const float,1> d2_(ext, d2__);
			array_view<const float,1> elv_(ext, elv__);
			array_view<const float,1> h_(ext, h__);
			array_view<const int,1> t_(ext, t__);
			array_view<const float,1> solitonFactor_(ext, solitonFactor);
			array_view< float,1> fm_(ext, fm__);
			array_view< float,1> fn_(ext, fn__);
			array_view<const float,1> rdx_(IX, rdx__);
			const float r_dy_ = r_dy;

#endif
