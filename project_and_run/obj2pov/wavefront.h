#include "matrixlib.h"

class Wavefront
{

  struct Face
  {
    int v[3];       	   
    int n[3];
    int mat;  // material index
    int facenum;
  };
  
  struct Material
  {
    char name[80];
    float r, g, b;       
  };

  vector<Vector> points;
  vector<Vector> normals;
  vector<Face> faces;
  vector<Material> materials;


  void loadfile(char *filename, char *mtlfilename, float scale)
  {
    char line[2048];
    int spaces = 0;          
    FILE *fp;

    Material mat;                       

    // read the materials    
    
    fp = fopen(mtlfilename, "r");
    if (!fp) {MessageBox(NULL, "could not open material file", "error", MB_OK); return;}
    while (fgets(line, 2048, fp))
      {          
	if (!strncmp(line, "newmtl ", 7)) sscanf(line+7, "%s", mat.name);
	
	if (!strncmp(line, "Ka ", 3)) 
	  {
	    sscanf(line+4, "%f %f %f", &mat.r, &mat.g, &mat.b);
	    materials.push_back(mat);
	  }
      }        	     
    fclose(fp); 
    

    int curmat = 0;

    // read the geometry

    fp = fopen(filename, "r");
    if (!fp) {MessageBox(NULL, "could not open file", "error", MB_OK); return;}
      
    while (fgets(line, 2048, fp))
      {          

	// vertex
	if (!strncmp(line, "v ", 2)) {
	  Vector v;
	  sscanf(line+2, "%f %f %f", &v[0], &v[1], &v[2]);
	  
	  v[0]*=scale;
	  v[1]*=scale;
	  v[2]*=scale;

	  points.push_back(v);	    
	}

	// normal
	if (!strncmp(line, "vn ", 2)) {
	  Vector n;
	  sscanf(line+2, "%f %f %f", &n[0], &n[1], &n[2]);
	  normals.push_back(n);	    
	}
	
	
	if (!strncmp(line, "usemtl ", 7)) {

	  char matname[80];
	  sscanf(line+7, "%s", matname);

	  for (int ctr = 0; ctr < materials.size(); ctr++) if (!strcmp(materials[ctr].name, matname)) curmat = ctr;
                       
	}

	// face	    
	if (!strncmp(line, "f ", 2)) {
	  Face f;

	  // this is just to read the texture coord indices since we're not using them
	  int junk;

	  if (strstr(line, "//"))   		   
	    sscanf(line+2, "%d//%d %d//%d %d//%d", &f.v[0],  &f.n[0], &f.v[1], &f.n[1], &f.v[2], &f.n[2]);
	  else	  sscanf(line+2, "%d/%d/%d %d/%d/%d %d/%d/%d", &f.v[0],  &junk, &f.n[0], &f.v[1],&junk, &f.n[1], &f.v[2], &junk, &f.n[2]);

	  // subtract 1 so it indexes properly into our vector object
	  f.v[0]--;
	  f.v[1]--;
	  f.v[2]--;
	  
	  f.n[0]--;
	  f.n[1]--;
	  f.n[2]--;	  
	  f.mat = curmat;
	  f.facenum = faces.size();
	  faces.push_back(f);	    
	}	    
          
      }        	     
    fclose(fp); 
    
    
  }

  // Octree code

  struct Node {
    Node* subnodes[8];

    float boxcenter[3];
    float boxhalfsize[3];

    vector<Face> triangles;
  };

  void buildoctree(Node *root, float halfwidth, float centerpt[3], vector<Face> triangles, float MAXNUMTRIS = 5, float MINHALFWIDTH = .1)
  {
    root->boxcenter[0] = centerpt[0];
    root->boxcenter[1] = centerpt[1];
    root->boxcenter[2] = centerpt[2];
    root->boxhalfsize[0] = halfwidth;
    root->boxhalfsize[1] = halfwidth;
    root->boxhalfsize[2] = halfwidth;

    if (triangles.size() < MAXNUMTRIS || halfwidth < MINHALFWIDTH)
      {
	// build a leaf node
	for (int ctr = 0; ctr < 8; ctr++) root->subnodes[ctr] = NULL;
	root->triangles = triangles;
	return; 
      }
    for (int nodectr = 0; nodectr < 8; nodectr++)
      {
	vector<Face> temptriangles;

	float plusorminus[3];

	if (nodectr & 1) plusorminus[0] = 1.0;
	else plusorminus[0] = -1.0;

	if (nodectr & 2) plusorminus[1] = 1.0;
	else plusorminus[1] = -1.0;

	if (nodectr & 4) plusorminus[2] = 1.0;
	else plusorminus[2] = -1.0;

	float boxcenter[3] =
	  {centerpt[0] + plusorminus[0]*halfwidth/2.0,
	   centerpt[1] + plusorminus[1]*halfwidth/2.0,
	   centerpt[2] + plusorminus[2]*halfwidth/2.0};

	float boxhalfsize[3] = {halfwidth/2.0, halfwidth/2.0, halfwidth/2.0};

	float triverts[3][3];

	for (int ctr = 0; ctr < triangles.size(); ctr++)
	  {
	    Face &f = triangles[ctr];

	    triverts[0][0] = points[f.v[0]][0];
	    triverts[0][1] = points[f.v[0]][1];
	    triverts[0][2] = points[f.v[0]][2];

	    triverts[1][0] = points[f.v[1]][0];
	    triverts[1][1] = points[f.v[1]][1];
	    triverts[1][2] = points[f.v[1]][2];

	    triverts[2][0] = points[f.v[2]][0];
	    triverts[2][1] = points[f.v[2]][1];
	    triverts[2][2] = points[f.v[2]][2];

	    int isect = triBoxOverlap(boxcenter, boxhalfsize, triverts);
	    if (isect) temptriangles.push_back(f);
	  }

	root->subnodes[nodectr] = new Node;

	buildoctree(root->subnodes[nodectr], halfwidth/2.0, boxcenter, temptriangles, MAXNUMTRIS, MINHALFWIDTH);
      }

  }

  // test if an AABB and another AABB intersect
  int aabbaabb(float boxcenter[3], float boxhalfsize[3],
	       float boxcenter2[3], float boxhalfsize2[3]
	       )
  {
     
    float distx = fabs(boxcenter2[0] - boxcenter[0]);
    float radsumx = boxhalfsize2[0] + boxhalfsize[0];
    if (distx > radsumx) return 0;

    float disty = fabs(boxcenter2[1] - boxcenter[1]);
    float radsumy = boxhalfsize2[1] + boxhalfsize[1];
    if (disty > radsumy) return 0;

    float distz = fabs(boxcenter2[2] - boxcenter[2]);
    float radsumz = boxhalfsize2[2] + boxhalfsize[2];
    if (distz > radsumz) return 0;

    return 1;
  }

  // draw faces that intersect an AABB
  // this is only approximate - for it to be completely accurate,
  // if there's only a partial intersection with a leaf node, we 
  // should check each triangle with the AABB
  void getfaces(Node *root, float boxcenter[3], float boxhalfsize[3], char* scratchpad)
  {

    int isect = aabbaabb(boxcenter, boxhalfsize, root->boxcenter, root->boxhalfsize);
    if (!isect) return;

    // leaf
    if (!root->subnodes[0])
      {
	glEnable(GL_COLOR_MATERIAL);              
	vector<Face>::iterator i = root->triangles.begin();

	glBegin (GL_TRIANGLES);
	while (i != root->triangles.end())
	  {
	    Face &f = *i;
	    if (scratchpad[f.facenum]) {i++;continue;}
	    scratchpad[f.facenum]=1;
	    glColor3f(materials[f.mat].r, materials[f.mat].g, materials[f.mat].b);

	    glNormal3f (normals[f.n[0]][0], normals[f.n[0]][1], normals[f.n[0]][2]);
	    glVertex3f (points[f.v[0]][0], points[f.v[0]][1], points[f.v[0]][2]);

	    glNormal3f (normals[f.n[1]][0], normals[f.n[1]][1], normals[f.n[1]][2]);
	    glVertex3f (points[f.v[1]][0], points[f.v[1]][1], points[f.v[1]][2]);

	    glNormal3f (normals[f.n[2]][0], normals[f.n[2]][1], normals[f.n[2]][2]);
	    glVertex3f (points[f.v[2]][0], points[f.v[2]][1], points[f.v[2]][2]);
       	 
	    i++;      	 
	  }
	glEnd ();
	return;                      
      }
             
    for (int nodectr = 0; nodectr < 8; nodectr++)
      getfaces(root->subnodes[nodectr], boxcenter, boxhalfsize, scratchpad);


  }    

  void deleteoctree(Node *root)
  {
     
    // if there are leaves, delete them     
    if (root->subnodes[0])
      for (int nodectr = 0; nodectr < 8; nodectr++) deleteoctree(root->subnodes[nodectr]);

    delete root;
     
  }


 public:



  Node octroot;

  Wavefront(char *filename, char *mtlfilename, float scale, float halfwidth = 100, float MAXNUMTRIS = 5, float MINHALFWIDTH = .001)
    {
      loadfile(filename, mtlfilename, scale);
      float centerpt[3] = {0, 0, 0};
      buildoctree(&octroot, 100, centerpt, faces, MAXNUMTRIS, MINHALFWIDTH);      
    }


  void draw(void)
  {
    glEnable(GL_COLOR_MATERIAL);              
    vector<Face>::iterator i = faces.begin();
    
    glBegin (GL_TRIANGLES);
    while (i != faces.end())
      {
	Face &f = *i;
	glColor3f(materials[f.mat].r, materials[f.mat].g, materials[f.mat].b);

	glNormal3f (normals[f.n[0]][0], normals[f.n[0]][1], normals[f.n[0]][2]);
	glVertex3f (points[f.v[0]][0], points[f.v[0]][1], points[f.v[0]][2]);

	glNormal3f (normals[f.n[1]][0], normals[f.n[1]][1], normals[f.n[1]][2]);
	glVertex3f (points[f.v[1]][0], points[f.v[1]][1], points[f.v[1]][2]);

	glNormal3f (normals[f.n[2]][0], normals[f.n[2]][1], normals[f.n[2]][2]);
	glVertex3f (points[f.v[2]][0], points[f.v[2]][1], points[f.v[2]][2]);
       	 
	i++;      	 
      }
    glEnd ();
  }


  void culldraw(float centerpt[3], float halfsize[3])
  {
    char *scratchpad = new char[faces.size()];
    memset(scratchpad, 0, faces.size());     
    getfaces(&octroot, centerpt, halfsize, scratchpad);
    delete[] scratchpad;
  }

  ~Wavefront(void)
    {
      deleteoctree(&octroot);
    }

};
