#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <vector>

class point
{
public:
	double x;
	double y;
	double z;
	double color[3];
};

class index
{
public:
	int i[3];
};


void NormalizeSize(std::vector< point>& vertex)
{

	float radius = 3.0f;

	// Compute the bounding box
	float max[3], min[3];
	float size[3], mid[3];

	max[0] = vertex[0].x;
	max[1] = vertex[0].y;
	max[2] = vertex[0].z;

	min[0] = max[0];
	min[1] = max[1];
	min[2] = max[2];

	for ( int i = 1; i < vertex.size(); i++ )
	{
		if ( max[0] < vertex[i].x ) max[0] = vertex[i].x;
		if ( max[1] < vertex[i].y ) max[1] = vertex[i].y;
		if ( max[2] < vertex[i].z ) max[2] = vertex[i].z;
		if ( min[0] > vertex[i].x ) min[0] = vertex[i].x;
		if ( min[1] > vertex[i].y ) min[1] = vertex[i].y;
		if ( min[2] > vertex[i].z ) min[2] = vertex[i].z;
	}
	size[0] = max[0] - min[0];
	size[1] = max[1] - min[1];
	size[2] = max[2] - min[2];
	mid[0] = 0.5f*(max[0] + min[0]);
	mid[1] = 0.5f*(max[1] + min[1]);
	mid[2] = 0.5f*(max[2] + min[2]);

	float diagonal = (float)sqrt(size[0]*size[0] + size[1]*size[1] + size[2]*size[2]);
	float scale = radius/(0.5f*diagonal);

	for ( int i = 0; i < vertex.size(); i++ )
	{
		vertex[i].x = scale*(vertex[i].x - mid[0]);
		vertex[i].y = scale*(vertex[i].y - mid[1]);
		vertex[i].z = scale*(vertex[i].z - mid[2]);
	}
}


int main( int argc, char** argv)
{
	if ( argc < 4 )
	{
		return -1;
	}

	FILE* fp = fopen(argv[1], "r");
	if ( fp == NULL )
	{
		return -1;
	}

	std::vector< point> vert;
	std::vector< point> nrml;
	std::vector< index> ind;
	std::vector< index> nind;

	char buf[256];
	while(1)
	{
		if ( fgets(buf, 256, fp) == NULL )
		{
			break;
		}
		if ( buf[0] == 'v')
		{
			break;
		}
	}

	int vertex_color = 0;
	double x, y, z;
	while(1)
	{
		int color[3];
		int n = sscanf(buf, "v %lf %lf %lf %d %d %d", &x, &y, &z, color, color+1, color+2);
		if ( n != 6)
		{
			sscanf(buf, "v %lf %lf %lf", &x, &y, &z);
		}else
		{
			vertex_color = 1;
		}
		point p;
		p.x = x;
		p.y = y;
		p.z = z;
		if ( vertex_color )
		{
			p.color[0] = color[0]/255.0;
			p.color[1] = color[1]/255.0;
			p.color[2] = color[2]/255.0;
		}else
		{
			p.color[0] = -1;
			p.color[1] = -1;
			p.color[2] = -1;
		}
		vert.push_back(p);

		if ( fgets(buf, 256, fp) == NULL )
		{
			break;
		}
		if ( buf[0] != 'v' )
		{
			break;
		}
	}

	while(buf[0] != 'f')
	{
		if ( fgets(buf, 256, fp) == NULL )
		{
			break;
		}
		if ( buf[0] == 'f')
		{
			break;
		}
	}

	int id[3];
	while(1)
	{
		int n = sscanf(buf, "f %d %d %d", id, id+1, id+2);
		int dmy[3];
		if ( n != 3 )
		{
			n = sscanf(buf, "f %d//%d %d//%d %d//%d", id, dmy, id+1, dmy+1, id+2, dmy+2);
		}
		int dmy2[3];
		if ( n != 6 )
		{
			n = sscanf(buf, "f %d/%d/%d %d/%d/%d %d/%d/%d", id, dmy2, dmy, id+1, dmy2+1, dmy+1, id+2, dmy2+2, dmy+2);
		}
		index p;
		p.i[0] = id[0]-1;
		p.i[1] = id[1]-1;
		p.i[2] = id[2]-1;
		ind.push_back(p);

		if ( n != 3 )
		{
			p.i[0] = dmy[0]-1;
			p.i[1] = dmy[1]-1;
			p.i[2] = dmy[2]-1;
			nind.push_back(p);
		}

		if ( fgets(buf, 256, fp) == NULL )
		{
			break;
		}
		if ( buf[0] != 'f')
		{
			break;
		}
	}
	fclose( fp );

	//NormalizeSize(vert);

	fp = fopen(argv[2], "w");

	char* obj_id = argv[3];
	int sz = 0;
	if ( vertex_color == 0 )
	{
		fprintf(fp, "#declare Mesh_%s=\n", obj_id);
		fprintf(fp, "mesh2{\n");

		sz = vert.size();
		fprintf(fp, "  vertex_vectors{\n");
		fprintf(fp, "  %d,\n", sz);
		for ( int i = 0; i < sz-1; i++ )
		{
			fprintf(fp, "\t<%f, %f, %f>,\n", vert[i].x, vert[i].y, vert[i].z);
		}
		fprintf(fp, "\t<%f, %f, %f>\n", vert[sz-1].x, vert[sz-1].y, vert[sz-1].z);
		fprintf(fp, "   }\n\n");

		if ( nrml.size() )
		{
			sz = nrml.size();
			fprintf(fp, "  normal_vectors{\n");
			fprintf(fp, "  %d,\n", sz);
			for ( int i = 0; i < sz-1; i++ )
			{
				fprintf(fp, "\t<%f, %f, %f>,\n", nrml[i].x, nrml[i].y, nrml[i].z);
			}
			fprintf(fp, "\t<%f, %f, %f>\n", nrml[sz-1].x, nrml[sz-1].y, nrml[sz-1].z);
			fprintf(fp, "   }\n\n");
		}

		sz = ind.size();
		fprintf(fp, "  face_indices{\n");
		fprintf(fp, "  %d\n", sz);
		for ( int i = 0; i < sz-1; i++ )
		{
			fprintf(fp, "\t<%d, %d, %d>,\n", ind[i].i[0], ind[i].i[1], ind[i].i[2]);
		}
		fprintf(fp, "\t<%d, %d, %d>\n", ind[sz-1].i[0], ind[sz-1].i[1], ind[sz-1].i[2]);
		fprintf(fp, "   }\n");

		if ( nrml.size() )
		{
			sz = nind.size();
			fprintf(fp, "  normal_indices {\n");
			fprintf(fp, "  %d\n", sz);
			for ( int i = 0; i < sz-1; i++ )
			{
				fprintf(fp, "\t<%d, %d, %d>,\n", nind[i].i[0], nind[i].i[1], nind[i].i[2]);
			}
			fprintf(fp, "\t<%d, %d, %d>\n", nind[sz-1].i[0], nind[sz-1].i[1], nind[sz-1].i[2]);
			fprintf(fp, "   }\n");
		}

		fprintf(fp, "}\n\n");
	}

	if ( vertex_color )
	{
		fprintf(fp, "#declare Material_001_fsh=finish {\n");
		fprintf(fp, "         ambient 0.1  diffuse 0.86  specular 0.325 roughness 0.1\n");
		fprintf(fp, "}\n\n"); 


		fprintf(fp, "#declare Mesh_%s=\n", obj_id);
		fprintf(fp, "mesh2{\n");

		sz = vert.size();
		fprintf(fp, "  vertex_vectors{\n");
		fprintf(fp, "  %d,\n", sz);
		for ( int i = 0; i < sz-1; i++ )
		{
			fprintf(fp, "\t<%f, %f, %f>,\n", vert[i].x, vert[i].y, vert[i].z);
		}
		fprintf(fp, "\t<%f, %f, %f>\n", vert[sz-1].x, vert[sz-1].y, vert[sz-1].z);
		fprintf(fp, "   }\n\n");

		if ( nrml.size() )
		{
			sz = nrml.size();
			fprintf(fp, "  normal_vectors{\n");
			fprintf(fp, "  %d,\n", sz);
			for ( int i = 0; i < sz-1; i++ )
			{
				fprintf(fp, "\t<%f, %f, %f>,\n", nrml[i].x, nrml[i].y, nrml[i].z);
			}
			fprintf(fp, "\t<%f, %f, %f>\n", nrml[sz-1].x, nrml[sz-1].y, nrml[sz-1].z);
			fprintf(fp, "   }\n\n");
		}

		if ( vertex_color )
		{
			sz = vert.size();
			fprintf(fp, "  texture_list {\n");
			fprintf(fp, "  %d,\n", sz);
			for ( int i = 0; i < sz-1; i++ )
			{
				fprintf(fp, "\ttexture{pigment{rgb<%.3f,%.3f,%.3f>}finish{Material_001_fsh}},\n", vert[i].color[0], vert[i].color[1], vert[i].color[2]);
			}
		}
		fprintf(fp, "\ttexture{pigment{rgb<%.3f,%.3f,%.3f>}finish{Material_001_fsh}}\n", vert[sz-1].color[0], vert[sz-1].color[1], vert[sz-1].color[2]);
		fprintf(fp, "   }\n\n");


		sz = ind.size();
		fprintf(fp, "  face_indices{\n");
		fprintf(fp, "  %d\n", sz);
		for ( int i = 0; i < sz-1; i++ )
		{
			fprintf(fp, "\t<%d, %d, %d>,%d,%d,%d,\n", ind[i].i[0], ind[i].i[1], ind[i].i[2], ind[i].i[0], ind[i].i[1], ind[i].i[2]);
		}
		fprintf(fp, "\t<%d, %d, %d>,%d,%d,%d\n", ind[sz-1].i[0], ind[sz-1].i[1], ind[sz-1].i[2],ind[sz-1].i[0], ind[sz-1].i[1], ind[sz-1].i[2]);
		fprintf(fp, "   }\n");

		if ( nrml.size() )
		{
			sz = nind.size();
			fprintf(fp, "  normal_indices {\n");
			fprintf(fp, "  %d\n", sz);
			for ( int i = 0; i < sz-1; i++ )
			{
				fprintf(fp, "\t<%d, %d, %d>,\n", nind[i].i[0], nind[i].i[1], nind[i].i[2]);
			}
			fprintf(fp, "\t<%d, %d, %d>\n", nind[sz-1].i[0], nind[sz-1].i[1], nind[sz-1].i[2]);
			fprintf(fp, "   }\n");
		}

		fprintf(fp, "}\n\n");
	}
	fprintf(fp, "\n\n");
	fprintf(fp, "object {\n");
	fprintf(fp, "   Mesh_%s\n", obj_id);
	fprintf(fp,  "}\n\n");
	fclose(fp);

	return 0;
}
