void RGB2HSV( double *H, double *S, double *V, double R, double G, double B )
{
    double Z;
    double r,g,b;
    *V = max_color( R, G, B );
    Z  = min_color( R, G, B );
    if( *V != 0.0 )
        *S = ( *V - Z ) / *V;
    else
        *S = 0.0;
    if( ( *V - Z ) != 0 ){
        r = ( *V - R ) / ( *V - Z );
        g = ( *V - G ) / ( *V - Z );
        b = ( *V - B ) / ( *V - Z );
    }
    else{
        r = 0;
        g = 0;
        b = 0;
    }
    if( *V == R )
        *H = 60 * ( b - g );		// 60 = PI/3
    else if( *V == G )
        *H = 60 * ( 2 + r - b );
    else
        *H = 60 * ( 4 + g - r );
    if( *H < 0.0 )
        *H = *H + 360;
}
double max_color( double r, double g, double b )
{
    double ret;
    if( r > g ){
        if( r > b )
            ret = r;
        else
            ret = b;
    }
    else{
        if( g > b )
            ret = g;
        else
            ret = b;
    }
    return ret;
}
double min_color( double r, double g, double b )
{
    double ret;
    if( r < g ){
        if( r < b )
            ret = r;
        else
            ret = b;
    }
    else{
        if( g < b )
            ret = g;
        else
            ret = b;
    }
    return ret;
}
