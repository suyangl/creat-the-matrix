#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//my part starts from LINE 138.

class point {
public:
	float x;
	float y;
};
/*---------------------------------------------------------------*/
class obstacle {
public:
	int n_sommets; //nombre de sommets
	vector<point> sommets;


	//point* sommets=new point[n_sommet];
	//void create_from_stack(stack<point>,int);


	ostream& operator<<(ostream& flux) {
		flux << "nombre de sommets de l'obstacle est" << n_sommets << "qui sont :" << endl;
		for (int i = 0; i<n_sommets; i++) { flux << "point num " << i << " : " << "x = " << sommets[i].x << ", y = " << sommets[i].y << endl; }
		return(flux);
	}



}; //fin classe obstacle


   /**************************************************************************************************/


float min(float x, float y) {
	if (x<y) return(x); else return(y);
}

float max(float x, float y) {
	if (x>y) return(x); else return(y);
}




float* equation_droite(float xa, float ya, float xb, float yb) { //passant par deux points A et B, sous la forme ax+by+c=0

	float* E = new float[3];
	E[0] = yb - ya; //coefficiant a
	E[1] = xa - xb; //coefficiant b
	E[2] = ya*xb - xa*yb; //coefficiant c
	return(E);
}

bool point_in_segment(point A, point B, point C) { //retourne true si C dans [AB], false sinon
	return((C.x <= max(A.x, B.x)) && (C.x >= min(A.x, B.x)) && (C.y <= max(A.y, B.y)) && (C.y >= min(A.y, B.y)));


}

bool intersection_segments(float xa1, float ya1, float xb1, float yb1, float xa2, float ya2, float xb2, float yb2) { //[A1,B1] et [A2,B2]
	float* E1 = equation_droite(xa1, ya1, xb1, yb1);
	float* E2 = equation_droite(xa2, ya2, xb2, yb2);

	if ((E1[0] * E2[1] - E2[0] * E1[1] == 0) || (E1[1] * E2[0] - E2[1] * E1[0] == 0)) {
		/*if( (x<=max(xa1,xb1)) && (x>=min(xa1,xb1)) && (y<=max(ya1,yb1)) && (y>=min(ya1,yb1)) || )*/ return(false);
	}
	else {
		float x = (E1[1] * E2[2] - E2[1] * E1[2]) / (E1[0] * E2[1] - E2[0] * E1[1]); // coordonn¨¦es pt d'intersection
		float y = (E2[2] * E1[0] - E2[0] * E1[2]) / (E1[1] * E2[0] - E2[1] * E1[0]); // coordonn¨¦es pt d'intersection
		if ((x <= max(xa1, xb1)) && (x >= min(xa1, xb1)) && (y <= max(ya1, yb1)) && (y >= min(ya1, yb1)) && (x <= max(xa2, xb2)) && (x >= min(xa2, xb2)) && (y <= max(ya2, yb2)) && (y >= min(ya2, yb2))) {

			if (((x == xa1) && (y == ya1)) || ((x == xa2) && (y == ya2)) || ((x == xb1) && (y == yb1)) || ((x == xb2) && (y == yb2))) return(false); else return(true);
		}
		else return(false);

	}

}//fin intersection_segments



void lecture_obstacles(char* path, vector<obstacle>& V_obs, int& N) {
	fstream ff(path, ios::out | ios::in);
	//ff.open(path,ios::out | ios::in);
	char c;
	char k;
	point temp_p;
	string temp = "";
	int k_temp = 0; //nombre de points d'obstacle
	obstacle temp_obs;
	while (ff >> noskipws >> c) {
		if ((c != ',') && (c != ';') && (c != '\n')) { temp = temp + c; }
		else if (c == ',') { temp_p.x = stof(temp); temp.clear(); }
		else if (c == ';') { temp_p.y = stof(temp); temp.clear(); temp_obs.sommets.push_back(temp_p); k_temp++; /*stock nouvelle point dans une struc*/ }
		else { N++; temp_obs.n_sommets = k_temp; /*modification du tableau d'obstacles*/ V_obs.push_back(temp_obs); k_temp = 0; temp_obs.sommets.clear(); }






	} //end while

} //end lecture

  /*
  void affichage_obstacles(vector<obstacle> V_obs,int N){
  for(int i=0;i<N;i++){
  cout<<V_obs[i];
  }//fin boucle for

  }

  */















//my part starts from here


bool directly_reachable(point A, point B, obstacle& obs)	//Si l'obstacle obs bloque le segment AB
{
	for (int i = 0; i < obs.n_sommets - 1; i++)
	{
		if (intersection_segments(A.x, A.y, B.x, B.y, obs.sommets[i].x, obs.sommets[i].y, obs.sommets[i + 1].x, obs.sommets[i + 1].y))
			return false;
	}
	if (intersection_segments(A.x, A.y, B.x, B.y, obs.sommets[obs.n_sommets - 1].x, obs.sommets[obs.n_sommets - 1].y, obs.sommets[0].x, obs.sommets[0].y))
		return false;
	return true;
}

bool directly_reachable(point A, point B, vector<obstacle>& V_obs, int N) //Si les obstacles bloquent le segment AB 
{
	for (int i = 0; i < N; i++)
	{
		if (!directly_reachable(A, B, V_obs[i]))
			return false;
	}
	return true;

}

#define INFINITY 1e10;
struct vertax_mapping	//Pour lier le sommet dans la matrice de distance et le sommet de quelle obstacle
{
	int obstacle_number;
	int point_number;
};

class distance_matrix	//Enregistrer l'information de distance
{
public:
	double** matrix;	//La matrice de distance
	int nombre_sommet;	//le nombre de sommet
	vector<vertax_mapping> vertice;	//Pour lier le sommet dans la matrice de distance et le sommet de quelle obstacle

	distance_matrix(vector<obstacle>& V_obs, int N);	//Construire la matrice
	void display();
	void display_vertax_mapping();
	~distance_matrix();
};


distance_matrix::distance_matrix(vector<obstacle>& V_obs, int N)
{
	nombre_sommet = 0;
	for (int i = 0; i < N; i++)	//Compter les sommets
	{
		nombre_sommet += V_obs[i].n_sommets;
	}
	matrix = new double*[nombre_sommet];	//Creer l'assembl¨¦e de 2D
	for (int i = 0; i < nombre_sommet; i++)
	{
		matrix[i] = new double[nombre_sommet];
	}
	int obstacle_count = 0;
	int point_count = 0;
	vertax_mapping temp_vertax;
	for (int i = 0; i < nombre_sommet; )	//Construire le lien entre les sommets de matrice et les sommets des obstacles
	{
		if (point_count < V_obs[obstacle_count].n_sommets)
		{
			temp_vertax.obstacle_number = obstacle_count;
			temp_vertax.point_number = point_count;
			vertice.push_back(temp_vertax);
			point_count++;
			i++;
		}
		else
		{
			obstacle_count++;
			point_count = 0;
		}
	}

	display_vertax_mapping();	//Seulement pour debugging

	double delta_x, delta_y;
	for (int i = 0; i < nombre_sommet; i++)	//Calculer la matrice
	{
		for (int j = 0; j < nombre_sommet; j++)
		{


			if (directly_reachable(V_obs[vertice[i].obstacle_number].sommets[vertice[i].point_number], V_obs[vertice[j].obstacle_number].sommets[vertice[j].point_number], V_obs, N))
				//Si les deux points peuvent li¨¦s sans traverser les bords des obstacles
			{
				delta_x = V_obs[vertice[i].obstacle_number].sommets[vertice[i].point_number].x - V_obs[vertice[j].obstacle_number].sommets[vertice[j].point_number].x;
				delta_y = V_obs[vertice[i].obstacle_number].sommets[vertice[i].point_number].y - V_obs[vertice[j].obstacle_number].sommets[vertice[j].point_number].y;
				matrix[i][j] = sqrt(delta_x*delta_x + delta_y*delta_y);
			}
			else
				matrix[i][j] = INFINITY + 1;
		}
	}
}

void distance_matrix::display()
{
	for (int i = 0; i < nombre_sommet; i++)
	{
		for (int j = 0; j < nombre_sommet; j++)
		{
			cout << matrix[i][j] << "\t";
		}
		cout << endl;
	}
}

void distance_matrix::display_vertax_mapping()
{
	for (int i = 0; i < nombre_sommet; i++)
	{
		cout << i << "\tobstacle" << vertice[i].obstacle_number << "\tpoint" << vertice[i].point_number << endl;
	}
}

distance_matrix::~distance_matrix() {
	for (int i = 0; i < nombre_sommet; i++)
	{
		delete[] matrix[i];
	}
	delete[] matrix;
}


