
/***********************************************/
/*     Bibliotecas necessarias ao programa     */
/***********************************************/
#include <math.h>
/***********************************************/


/*******************************************************************/
/*     Variaveis usadas para a definicao do brilho e contraste     */
/*******************************************************************/
float material_shininess[] = {50.0};
float material_diffuse[] = {0.8,0.8,0.8,1.0};
float material_specular[] = {0.8,0.8,0.8,1.0};
/*******************************************************************/


/***********************************************************/
/*     Variaveis usadas para a construçao dos objectos     */
/***********************************************************/
GLdouble alpha = (60.0 * 2.0 * PI) / 360.0;
GLdouble raio_triangulo = (TAB_SIZE / 12) / sin(alpha);
GLdouble aresta = sqrt(pow((raio_triangulo * sin(alpha * 2)),2.0) 
		+ pow((raio_triangulo - (raio_triangulo * cos(alpha * 2))),2.0));
/***********************************************************/


/******************************************************************************/
/*     Variaveis usadas para definir a posiçao das piramides no tabuleiro     */
/******************************************************************************/
GLdouble linhas_pretas[6] = {- (3 * (raio_triangulo + (raio_triangulo * cos(alpha)))) + raio_triangulo,
	- (2 * (raio_triangulo + (raio_triangulo * cos(alpha)))) + raio_triangulo,
	- (raio_triangulo + (raio_triangulo * cos(alpha))) + raio_triangulo,
	raio_triangulo,
	(raio_triangulo + (raio_triangulo * cos(alpha))) + raio_triangulo,
	(2 * (raio_triangulo + (raio_triangulo * cos(alpha)))) + raio_triangulo};

GLdouble linhas_brancas[6] = {- (2 * (raio_triangulo + (raio_triangulo * cos(alpha)))) - raio_triangulo,
	- (raio_triangulo + (raio_triangulo * cos(alpha))) - raio_triangulo,
	- raio_triangulo,
	(raio_triangulo + (raio_triangulo * cos(alpha))) - raio_triangulo,
	(2 * (raio_triangulo + (raio_triangulo * cos(alpha)))) - raio_triangulo,
	(3 * (raio_triangulo + (raio_triangulo * cos(alpha)))) - raio_triangulo};

GLdouble colunas[11] = {-(5 * ((TAB_SIZE / 2) / 6)),-(4 * ((TAB_SIZE / 2) / 6)),
	-(3 * ((TAB_SIZE / 2) / 6)),-(2 * ((TAB_SIZE / 2) / 6)),-((TAB_SIZE / 2) / 6),0,
	(TAB_SIZE / 2) / 6,2 * ((TAB_SIZE / 2) / 6),3 * ((TAB_SIZE / 2) / 6),
	4 * ((TAB_SIZE / 2) / 6),5 * ((TAB_SIZE / 2) / 6)};

float pos_piramides[12][3];
float pos_iniciais_piramides[12][3] = 
{	{ colunas[2],0.0,linhas_brancas[5]},
	{ colunas[3],0.0, linhas_pretas[5]},
	{ colunas[7],0.0, linhas_pretas[5]},
	{ colunas[8],0.0,linhas_brancas[5]},
	{colunas[10],0.0,linhas_brancas[3]},
	{colunas[10],0.0, linhas_pretas[2]},
	{ colunas[8],0.0, linhas_pretas[0]},
	{ colunas[7],0.0,linhas_brancas[0]},
	{ colunas[3],0.0,linhas_brancas[0]},
	{ colunas[2],0.0, linhas_pretas[0]},
	{ colunas[0],0.0, linhas_pretas[2]},
	{ colunas[0],0.0,linhas_brancas[3]}	};
/******************************************************************************/


/**************************************************************************/
/*     Inicializaçao das posicoes iniciais das piramides no tabuleiro     */
/**************************************************************************/
void initialize_pos_iniciais_piramides(float altura_piramide)
{
	int i;

	for(i = 0; i < 12; i++)
		pos_iniciais_piramides[i][1] = 0.1 + (altura_piramide / 2);
}
/**************************************************************************/


/*****************************************************************/
/*     Inicializacao das posicoes das piramides no tabuleiro     */
/*****************************************************************/
void initialize_pos_piramides(void)
{
	int i, j;

	for(i = 0; i < 12; i++)
		for(j = 0; j < 3; j++)
			pos_piramides[i][j] = pos_iniciais_piramides[i][j];
}
/*****************************************************************/


/*********************************************************/
/*     Devolve a linha em que se encontra a piramide     */
/*********************************************************/
int get_line(GLdouble line)
{
	int i;

	for(i = 0; i < 6; i++)
	{
		if(abs(linhas_brancas[i] - line) < 0.5)
			return i;
		if(abs(linhas_pretas[i] - line) < 0.5)
			return (i + 10);
	}

	return -1;
}
/*********************************************************/


/**********************************************************/
/*     Devolve a coluna em que se encontra a piramide     */
/**********************************************************/
int get_column(GLdouble column)
{
	int i;

	for(i = 0; i < 11; i++)
		if((int) colunas[i] == (int) column)
			return i;

	return -1;
}
/**********************************************************/


/******************************************************************/
/*     Devolve a piramide que se encontra na posiçao indicada     */
/******************************************************************/
int get_piramide(int line, int line_type, int column)
{
	int i;

	double posX = colunas[column];
	double posZ;

	if(line_type)
		posZ = linhas_brancas[line];
	else
		posZ = linhas_pretas[line];

	for(i = 0; i < 12; i++)
		if((abs(pos_piramides[i][0] - posX) < 0.01) && (abs(pos_piramides[i][2] - posZ) < 0.01))
			return i;

	return -1;
}
/******************************************************************/


/******************************************************/
/*     Normaliza um vector pela unidade de medida     */
/******************************************************/
void normalise(GLdouble *vec)
{
	GLdouble length = 0.0;
	int i;
	
	for (i = 0; i < VLENGTH; i++)
		length += vec[i] * vec[i]; 

	length = (GLdouble) sqrt((double)length); 

	for (i = 0; i < VLENGTH; i++)
		vec[i] = vec[i] / length; 
}
/******************************************************/


/********************************************/
/*     Calcula a normal para 3 vectores     */
/********************************************/
void normal3V(GLdouble *vec1, GLdouble *vec2, GLdouble *vec3, GLdouble *normal)
{
	normal[0] = ((vec1[1] - vec2[1]) * (vec1[2] + vec2[2])) +
				((vec2[1] - vec3[1]) * (vec2[2] + vec3[2])) +
				((vec3[1] - vec1[1]) * (vec3[2] + vec1[2]));

	normal[1] = ((vec1[2] - vec2[2]) * (vec1[0] + vec2[0])) +
				((vec2[2] - vec3[2]) * (vec2[0] + vec3[0])) +
				((vec3[2] - vec1[2]) * (vec3[0] + vec1[0]));

	normal[2] = ((vec1[0] - vec2[0]) * (vec1[1] + vec2[1])) +
				((vec2[0] - vec3[0]) * (vec2[1] + vec3[1])) +
				((vec3[0] - vec1[0]) * (vec3[1] + vec1[1]));

	normalise(normal);
}
/********************************************/


/********************************************/
/*     Calcula a normal para 4 vectores     */
/********************************************/
void calculaNormal(GLdouble *vec1, GLdouble *vec2, GLdouble *vec3, GLdouble *vec4, GLdouble *normal)
{
	normal[0] = ((vec1[1] - vec2[1]) * (vec1[2] + vec2[2])) + 
				((vec2[1] - vec3[1]) * (vec2[2] + vec3[2])) +
				((vec3[1] - vec4[1]) * (vec3[2] + vec4[2])) +
				((vec4[1] - vec1[1]) * (vec4[2] + vec1[2]));

	normal[1] = ((vec1[2] - vec2[2]) * (vec1[0] + vec2[0])) + 
				((vec2[2] - vec3[2]) * (vec2[0] + vec3[0])) +
				((vec3[2] - vec4[2]) * (vec3[0] + vec4[0])) +
				((vec4[2] - vec1[2]) * (vec4[0] + vec1[0]));

	normal[2] = ((vec1[0] - vec2[0]) * (vec1[1] + vec2[1])) + 
				((vec2[0] - vec3[0]) * (vec2[1] + vec3[1])) +
				((vec3[0] - vec4[0]) * (vec3[1] + vec4[1])) +
				((vec4[0] - vec1[0]) * (vec4[1] + vec1[1]));

	normalise(normal);
}
/********************************************/


/**********************************************/
/*     Representaçao do objecto triangulo     */
/**********************************************/
void triangulo(GLdouble raio, GLdouble *centro, GLint dir)
{
	GLdouble v[3][3];
	GLdouble normal[VLENGTH];

	GLdouble alpha_aux = (120.0 * 2.0 * PI) / 360.0;

	if(!dir)
	{
		v[0][0] = centro[0];
		v[0][1] = centro[1];
		v[0][2] = centro[2] - raio;

		v[1][0] = centro[0] - (raio * sin(alpha_aux));
		v[1][1] = centro[1];
		v[1][2] = centro[2] - raio * cos(alpha_aux);

		v[2][0] = centro[0] + raio * sin(alpha_aux);
		v[2][1] = centro[1];
		v[2][2] = centro[2] - raio * cos(alpha_aux);
	}
	else
	{
		v[0][0] = centro[0];
		v[0][1] = centro[1];
		v[0][2] = centro[2] + raio;

		v[1][0] = centro[0] + raio * sin(alpha_aux);
		v[1][1] = centro[1];
		v[1][2] = centro[2] + raio * cos(alpha_aux);

		v[2][0] = centro[0] - (raio * sin(alpha_aux));
		v[2][1] = centro[1];
		v[2][2] = centro[2] + raio * cos(alpha_aux);
	}

	normal3V(v[0],v[1],v[2],normal);
	glBegin(GL_TRIANGLES);
		glNormal3dv(normal);
		glTexCoord2f(0.5,0.0); glVertex3dv(v[0]);
		glTexCoord2f(0.0,1.0); glVertex3dv(v[1]);
		glTexCoord2f(1.0,1.0); glVertex3dv(v[2]);
	glEnd();
}
/**********************************************/


/**********************************************/
/*     Representacao do objecto tabuleiro     */
/**********************************************/
void tabuleiro(GLdouble dimx, GLdouble dimy, GLdouble dimz)
{
	GLdouble dx = dimx / 2,
			 dy = dimy / 2,
			 dz = dimz / 2;

	GLdouble v1[3] =  {-(dx / 2) - 0.447, dy,- dz - 0.894};
	GLdouble v2[3] =  {       - dx - 1.0, dy,           0};
	GLdouble v3[3] =  {-(dx / 2) - 0.447, dy,  dz + 0.894};
	GLdouble v4[3] =  {   dx / 2 + 0.447, dy,  dz + 0.894};
	GLdouble v5[3] =  {         dx + 1.0, dy,           0};
	GLdouble v6[3] =  {   dx / 2 + 0.447, dy,- dz - 0.894};
	GLdouble v7[3] =  {-(dx / 2) - 0.447,-dy,- dz - 0.894};
	GLdouble v8[3] =  {       - dx - 1.0,-dy,           0};
	GLdouble v9[3] =  {-(dx / 2) - 0.447,-dy,  dz + 0.894};
	GLdouble v10[3] = {   dx / 2 + 0.447,-dy,  dz + 0.894};
	GLdouble v11[3] = {         dx + 1.0,-dy,           0};
	GLdouble v12[3] = {   dx / 2 + 0.447,-dy,- dz - 0.894};
	GLdouble v13[3] = {        -(dx / 2), dy,        - dz};
	GLdouble v14[3] = {             - dx, dy,           0};
	GLdouble v15[3] = {        -(dx / 2), dy,          dz};
	GLdouble v16[3] = {           dx / 2, dy,          dz};
	GLdouble v17[3] = {               dx, dy,           0};
	GLdouble v18[3] = {           dx / 2, dy,        - dz};
	GLdouble v19[3] = {        -(dx / 2),-dy,        - dz};
	GLdouble v20[3] = {             - dx,-dy,           0};
	GLdouble v21[3] = {        -(dx / 2),-dy,          dz};
	GLdouble v22[3] = {           dx / 2,-dy,          dz};
	GLdouble v23[3] = {               dx,-dy,           0};
	GLdouble v24[3] = {           dx / 2,-dy,        - dz};

	int i, j, aux, dir;
	GLdouble centro_aux[3];

	GLdouble normal[VLENGTH];

	//**************************************************
	// Parte inferior
	//**************************************************

		material_diffuse[0] = 0.0;
		material_diffuse[1] = 0.0;
		material_diffuse[2] = 0.0;

		glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

		glBegin(GL_POLYGON);
			glVertex3dv(v7);
			glVertex3dv(v8);
			glVertex3dv(v9);
			glVertex3dv(v10);
			glVertex3dv(v11);
			glVertex3dv(v12);
		glEnd();

	//**************************************************


	//**************************************************
	// Parte lateral exterior
	//**************************************************

		material_diffuse[0] = 0.0;
		material_diffuse[1] = 0.0;
		material_diffuse[2] = 0.0;

		glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

		calculaNormal(v1,v7,v8,v2,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(v1);
			glVertex3dv(v7);
			glVertex3dv(v8);
			glVertex3dv(v2);
		glEnd();

		calculaNormal(v2,v8,v9,v3,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(v2);
			glVertex3dv(v8);
			glVertex3dv(v9);
			glVertex3dv(v3);
		glEnd();

		calculaNormal(v3,v9,v10,v4,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(v3);
			glVertex3dv(v9);
			glVertex3dv(v10);
			glVertex3dv(v4);
		glEnd();

		calculaNormal(v4,v10,v11,v5,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(v4);
			glVertex3dv(v10);
			glVertex3dv(v11);
			glVertex3dv(v5);
		glEnd();

		calculaNormal(v5,v11,v12,v6,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(v5);
			glVertex3dv(v11);
			glVertex3dv(v12);
			glVertex3dv(v6);
		glEnd();

		calculaNormal(v6,v12,v7,v1,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glVertex3dv(v6);
			glVertex3dv(v12);
			glVertex3dv(v7);
			glVertex3dv(v1);
		glEnd();

	//**************************************************


	//**************************************************
	// Parte de cima da borda
	//**************************************************

		material_diffuse[0] = 1.0;
		material_diffuse[1] = 0.0;
		material_diffuse[2] = 0.0;

		glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

		calculaNormal(v1,v2,v14,v13,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,0.0); glVertex3dv(v1);
			glTexCoord2f(1.0,0.0); glVertex3dv(v2);
			glTexCoord2f(1.0,1.0); glVertex3dv(v14);
			glTexCoord2f(0.0,1.0); glVertex3dv(v13);
		glEnd();

		calculaNormal(v2,v3,v15,v14,normal);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0,0.0); glVertex3dv(v2);
			glTexCoord2f(1.0,0.0); glVertex3dv(v3);
			glTexCoord2f(1.0,1.0); glVertex3dv(v15);
			glTexCoord2f(0.0,1.0); glVertex3dv(v14);
		glEnd();

		calculaNormal(v3,v4,v16,v15,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,0.0); glVertex3dv(v3);
			glTexCoord2f(1.0,0.0); glVertex3dv(v4);
			glTexCoord2f(1.0,1.0); glVertex3dv(v16);
			glTexCoord2f(0.0,1.0); glVertex3dv(v15);
		glEnd();

		calculaNormal(v4,v5,v17,v16,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,0.0); glVertex3dv(v4);
			glTexCoord2f(1.0,0.0); glVertex3dv(v5);
			glTexCoord2f(1.0,1.0); glVertex3dv(v17);
			glTexCoord2f(0.0,1.0); glVertex3dv(v16);
		glEnd();

		calculaNormal(v5,v6,v18,v17,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,0.0); glVertex3dv(v5);
			glTexCoord2f(1.0,0.0); glVertex3dv(v6);
			glTexCoord2f(1.0,1.0); glVertex3dv(v18);
			glTexCoord2f(0.0,1.0); glVertex3dv(v17);
		glEnd();

		calculaNormal(v6,v1,v13,v18,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,0.0); glVertex3dv(v6);
			glTexCoord2f(1.0,0.0); glVertex3dv(v1);
			glTexCoord2f(1.0,1.0); glVertex3dv(v13);
			glTexCoord2f(0.0,1.0); glVertex3dv(v18);
		glEnd();

	//**************************************************


	//**************************************************
	// Parte lateral interior
	//**************************************************

		material_diffuse[0] = 1.0;
		material_diffuse[1] = 0.0;
		material_diffuse[2] = 0.0;

		glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
		glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

		calculaNormal(v13,v19,v20,v14,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,1.0); glVertex3dv(v13);
			glTexCoord2f(0.0,0.0); glVertex3dv(v19);
			glTexCoord2f(1.0,0.0); glVertex3dv(v20);
			glTexCoord2f(1.0,1.0); glVertex3dv(v14);
		glEnd();

		calculaNormal(v14,v20,v21,v15,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,1.0); glVertex3dv(v14);
			glTexCoord2f(0.0,0.0); glVertex3dv(v20);
			glTexCoord2f(1.0,0.0); glVertex3dv(v21);
			glTexCoord2f(1.0,1.0); glVertex3dv(v15);
		glEnd();

		calculaNormal(v15,v21,v22,v16,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,1.0); glVertex3dv(v15);
			glTexCoord2f(0.0,0.0); glVertex3dv(v21);
			glTexCoord2f(1.0,0.0); glVertex3dv(v22);
			glTexCoord2f(1.0,1.0); glVertex3dv(v16);
		glEnd();

		calculaNormal(v16,v22,v23,v17,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,1.0); glVertex3dv(v16);
			glTexCoord2f(0.0,0.0); glVertex3dv(v22);
			glTexCoord2f(1.0,0.0); glVertex3dv(v23);
			glTexCoord2f(1.0,1.0); glVertex3dv(v17);
		glEnd();

		calculaNormal(v17,v23,v24,v18,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,1.0); glVertex3dv(v17);
			glTexCoord2f(0.0,0.0); glVertex3dv(v23);
			glTexCoord2f(1.0,0.0); glVertex3dv(v24);
			glTexCoord2f(1.0,1.0); glVertex3dv(v18);
		glEnd();

		calculaNormal(v18,v24,v19,v13,normal);
		glBegin(GL_POLYGON);
			glNormal3dv(normal);
			glTexCoord2f(0.0,1.0); glVertex3dv(v18);
			glTexCoord2f(0.0,0.0); glVertex3dv(v24);
			glTexCoord2f(1.0,0.0); glVertex3dv(v19);
			glTexCoord2f(1.0,1.0); glVertex3dv(v13);
		glEnd();

	//**************************************************


	//**************************************************
	// Triangulos
	//**************************************************
	for(i = 0; i < 6; i++)
	{
		if((i == 0) || (i == 5))
			aux = 2;

		if((i == 1) || (i == 4))
			aux = 1;

		if((i == 2) || (i == 3))
			aux = 0;

		if(i < 3)
			dir = 0;
		else
			dir = 1;

		for(j = aux; j < 11 - aux; j++)
		{
			centro_aux[0] = colunas[j];
			centro_aux[1] = 0.1;

			if(dir)
				centro_aux[2] = linhas_brancas[i];
			else
				centro_aux[2] = linhas_pretas[i];

			if(dir)
			{
				material_diffuse[0] = 1.0;
				material_diffuse[1] = 1.0;
				material_diffuse[2] = 1.0;
			}
			else
			{
				material_diffuse[0] = 0.2;
				material_diffuse[1] = 0.2;
				material_diffuse[2] = 0.2;
			}

			glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
			glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

			triangulo(raio_triangulo,centro_aux,dir);

			if(dir)
				dir = 0;
			else
				dir = 1;
		}
	}
	//**************************************************

}
/**********************************************/


/*********************************************/
/*     Representacao do objecto piramide     */
/*********************************************/
void piramide(GLdouble raio, GLint tipo)
{
	GLdouble alpha1 = (120.0 * 2.0 * PI) / 360.0;
	GLdouble alpha2 = (240.0 * 2.0 * PI) / 360.0;

	GLdouble normal[VLENGTH];
	GLdouble altura = sqrt(pow(aresta,2.0) - pow(raio,2.0));

	initialize_pos_iniciais_piramides(altura);
	initialize_pos_piramides();

	GLdouble v1[3] = {               0.0,   altura / 2,                  0.0};
	GLdouble v2[3] = {              raio,-(altura / 2),                  0.0};
	GLdouble v3[3] = {raio * cos(alpha1),-(altura / 2),-(raio * sin(alpha1))};
	GLdouble v4[3] = {raio * cos(alpha2),-(altura / 2),-(raio * sin(alpha2))};

	material_diffuse[0] = 1.0;
	material_diffuse[1] = 1.0;
	material_diffuse[2] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	normal3V(v1,v2,v3,normal);
	glBegin(GL_TRIANGLES);
		glNormal3dv(normal);
		glTexCoord2f(0.0,1.0); glVertex3dv(v1);
		glTexCoord2f(0.0,0.0); glVertex3dv(v2);
		glTexCoord2f(1.0,0.0); glVertex3dv(v3);
	glEnd();

	normal3V(v1,v3,v4,normal);
	glBegin(GL_TRIANGLES);
		glNormal3dv(normal);
		glTexCoord2f(0.0,1.0); glVertex3dv(v1);
		glTexCoord2f(0.0,0.0); glVertex3dv(v3);
		glTexCoord2f(1.0,0.0); glVertex3dv(v4);
	glEnd();

	normal3V(v1,v4,v2,normal);
	glBegin(GL_TRIANGLES);
		glNormal3dv(normal);
		glTexCoord2f(0.0,1.0); glVertex3dv(v1);
		glTexCoord2f(0.0,0.0); glVertex3dv(v4);
		glTexCoord2f(1.0,0.0); glVertex3dv(v2);
	glEnd();

	normal3V(v2,v3,v4,normal);
	glBegin(GL_TRIANGLES);
		glNormal3dv(normal);
		glTexCoord2f(0.0,1.0); glVertex3dv(v2);
		glTexCoord2f(0.0,0.0); glVertex3dv(v3);
		glTexCoord2f(1.0,0.0); glVertex3dv(v4);
	glEnd();
}
/*********************************************/


/*********************************************/
/*     Representacao do objecto marcador     */
/*********************************************/
void marcador(GLdouble comprimento, GLdouble altura, GLdouble largura)
{
	GLdouble dx = comprimento / 2.0,
			 dy = altura / 2.0,
			 dz = largura / 2.0;

	GLdouble normal[VLENGTH];

	GLdouble v1[3] = {-dx,-dy, dz};
	GLdouble v2[3] = { dx,-dy, dz};
	GLdouble v3[3] = { dx, dy, dz};
	GLdouble v4[3] = {-dx, dy, dz};
	GLdouble v5[3] = {-dx,-dy,-dz};
	GLdouble v6[3] = { dx,-dy,-dz};
	GLdouble v7[3] = { dx, dy,-dz};
	GLdouble v8[3] = {-dx, dy,-dz};

	material_diffuse[0] = 1.0;
	material_diffuse[1] = 1.0;
	material_diffuse[2] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	// Face frente
	calculaNormal(v1,v2,v3,v4,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v1);
		glTexCoord2f(1.0,0.0); glVertex3dv(v2);
		glTexCoord2f(1.0,1.0); glVertex3dv(v3);
		glTexCoord2f(0.0,1.0); glVertex3dv(v4);
	glEnd();

	material_diffuse[0] = 0.0;
	material_diffuse[1] = 0.0;
	material_diffuse[2] = 0.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	// Face tras
	calculaNormal(v5,v6,v7,v8,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(v5);
		glVertex3dv(v6);
		glVertex3dv(v7);
		glVertex3dv(v8);
	glEnd();

	// Face lado direito
	calculaNormal(v2,v6,v7,v3,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(v2);
		glVertex3dv(v6);
		glVertex3dv(v7);
		glVertex3dv(v3);
	glEnd();

	// Face cima
	calculaNormal(v3,v7,v8,v4,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(v3);
		glVertex3dv(v7);
		glVertex3dv(v8);
		glVertex3dv(v4);
	glEnd();

	// Face lado esquerdo
	calculaNormal(v4,v8,v5,v1,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(v4);
		glVertex3dv(v8);
		glVertex3dv(v5);
		glVertex3dv(v1);
	glEnd();

	// Face baixo
	calculaNormal(v1,v2,v6,v5,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glVertex3dv(v1);
		glVertex3dv(v2);
		glVertex3dv(v6);
		glVertex3dv(v5);
	glEnd();
}
/*********************************************/


/*****************************************/
/*     Representacao do objecto cubo     */
/*****************************************/
void cubo(GLdouble aresta)
{
	GLdouble dx = aresta / 2.0;
	GLdouble dy = aresta / 2.0;
	GLdouble dz = aresta / 2.0;

	GLdouble normal[VLENGTH];

	GLdouble v1[3] = {-dx,-dy, dz};
	GLdouble v2[3] = { dx,-dy, dz};
	GLdouble v3[3] = { dx, dy, dz};
	GLdouble v4[3] = {-dx, dy, dz};
	GLdouble v5[3] = {-dx,-dy,-dz};
	GLdouble v6[3] = { dx,-dy,-dz};
	GLdouble v7[3] = { dx, dy,-dz};
	GLdouble v8[3] = {-dx, dy,-dz};

	material_diffuse[0] = 1.0;
	material_diffuse[1] = 1.0;
	material_diffuse[2] = 1.0;

	glMaterialfv(GL_FRONT,GL_SHININESS,material_shininess);
	glMaterialfv(GL_FRONT,GL_SPECULAR,material_specular);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,material_diffuse);

	// Face frente
	calculaNormal(v1,v2,v3,v4,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v1);
		glTexCoord2f(1.0,0.0); glVertex3dv(v2);
		glTexCoord2f(1.0,1.0); glVertex3dv(v3);
		glTexCoord2f(0.0,1.0); glVertex3dv(v4);
	glEnd();

	// Face tras
	calculaNormal(v5,v6,v7,v8,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v5);
		glTexCoord2f(1.0,0.0); glVertex3dv(v6);
		glTexCoord2f(1.0,1.0); glVertex3dv(v7);
		glTexCoord2f(0.0,1.0); glVertex3dv(v8);
	glEnd();

	// Face lado direito
	calculaNormal(v2,v6,v7,v3,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v2);
		glTexCoord2f(1.0,0.0); glVertex3dv(v6);
		glTexCoord2f(1.0,1.0); glVertex3dv(v7);
		glTexCoord2f(0.0,1.0); glVertex3dv(v3);
	glEnd();

	// Face cima
	calculaNormal(v7,v8,v4,v3,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v3);
		glTexCoord2f(1.0,0.0); glVertex3dv(v7);
		glTexCoord2f(1.0,1.0); glVertex3dv(v8);
		glTexCoord2f(0.0,1.0); glVertex3dv(v4);
	glEnd();

	// Face lado esquerdo
	calculaNormal(v8,v5,v1,v4,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v4);
		glTexCoord2f(1.0,0.0); glVertex3dv(v8);
		glTexCoord2f(1.0,1.0); glVertex3dv(v5);
		glTexCoord2f(0.0,1.0); glVertex3dv(v1);
	glEnd();

	// Face baixo
	calculaNormal(v1,v2,v6,v5,normal);
	glBegin(GL_POLYGON);
		glNormal3dv(normal);
		glTexCoord2f(0.0,0.0); glVertex3dv(v1);
		glTexCoord2f(1.0,0.0); glVertex3dv(v2);
		glTexCoord2f(1.0,1.0); glVertex3dv(v6);
		glTexCoord2f(0.0,1.0); glVertex3dv(v5);
	glEnd();
}
/*****************************************/
