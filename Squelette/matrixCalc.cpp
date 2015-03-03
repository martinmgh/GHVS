#include "matrixCalc.h"
extern int nb_bones;

glm::vec3 getScale(glm::vec3 ref1, glm::vec3 ref2, glm::vec3 mov1, glm::vec3 mov2){
	glm::vec3 ref = ref2 - ref1;
	glm::vec3 mov = mov2 - mov1;
	float s = sqrt(glm::dot(mov, mov)) / sqrt(glm::dot(ref, ref));

	glm::vec3 scalingVec = glm::vec3(s, s, s);
	/*scalingMat[0][0] = sX;
	scalingMat[1][1] = sY;
	scalingMat[2][2] = sZ;
	*/
	return scalingVec;
}

/* obtient l'angle de rotation */
float getRot(glm::vec3 ref1, glm::vec3 ref2, glm::vec3 mov1, glm::vec3 mov2){

	glm::vec3 ref = ref2 - ref1;
	glm::vec3 mov = mov2 - mov1;
	float normRef = sqrt(glm::dot(ref, ref));
	// printf("norme Ref : %f\n", normRef);

	float normMov = sqrt(glm::dot(mov, mov));
	// printf("norme Mov : %f\n", normMov);

	// printf("produit scalaire : %f\n", glm::dot(ref, mov));

	float theta = acos(glm::dot(ref, mov) / (normRef * normMov)); // toujours calculable

	return theta;
}

/* obtient le vecteur translation */
glm::vec3 getTrans(glm::vec3 ref, glm::vec3 mov){

	glm::vec3 translation = mov - ref;
	return translation;
}

/* calcule la normale */
glm::vec3 getNormal(glm::vec3 ref1, glm::vec3 ref2, glm::vec3 mov1, glm::vec3 mov2){
	glm::vec3 ref = ref2 - ref1;
	glm::vec3 mov = mov2 - mov1;
	glm::vec3 normal = glm::cross(ref, mov);
	float norm = sqrt(glm::dot(normal, normal));
	normal = normal / norm;
	return normal;
}


/* Calcule la matrice de transf. d'un bone */
glm::mat4 updateMatrix(glm::vec3 ref1, glm::vec3 ref2, glm::vec3 mov1, glm::vec3 mov2){
	float angl =0.0f;
	glm::vec3 transl;
	glm::mat4 res = glm::mat4(1.0f);
	glm::vec3 normal;
	glm::vec3 scale;

	angl = getRot(ref1, ref2, mov1, mov2);
	transl = getTrans(ref1, mov1);
	normal = getNormal(ref1, ref2, mov1, mov2);
	scale = getScale(ref1, ref2, mov1, mov2);

	//res = glm::scale(res, scale);
	//res = glm::rotate(res, -angl, normal);
	res = glm::translate(res, transl);
	return res;
}

/* Calcule la matrice de transformation de chaque bone et la range dans le tableau correspodant */
void updateData(glm::vec3 ** Bones, glm::mat4 * bone_matrices){
	int i;
	for (i = 0; i < nb_bones; i++){
		bone_matrices[i] = updateMatrix(Bones[i][0], Bones[i][1], Bones[i][2], Bones[i][3]);
		Bones[i][0] = Bones[i][2];
		Bones[i][1] = Bones[i][3];
	}
}

/* range dans le tableau des os les positions des os par d�faut du mod�le (pose au repos) */
void initData(glm::vec3 ** Bones, FILE* fichier){
	int i, j;
	for (i = 0; i < nb_bones; i++){
		fscanf(fichier, "%f %f %f", &Bones[i][0].x, &Bones[i][0].y, &Bones[i][0].z);
		fscanf(fichier, "%f %f %f", &Bones[i][1].x, &Bones[i][1].y, &Bones[i][1].z);
	}
	for (i = 0; i < nb_bones;i++)
		printf("Bone %d, frame 1. \n\told1 = (%f, %f, %f)\n\told2 = (%f, %f, %f)\n\n", i, Bones[i][0].x, Bones[i][0].y, Bones[i][0].z, Bones[i][1].x, Bones[i][1].y, Bones[i][1].z);
}

/* Lit les donn�es Kinect et les range dans le tableau de Bones(lui m�me tableau de vec3 */
void readData(FILE* fichier, glm::vec3 ** Bones){
	FILE* fichier3 = fopen("bones-ordonnesTestSam---.txt", "w");
	if (fichier3 == NULL){
		printf("Error opening\n");
		exit(1);
	}
	int i,j;
	for (i = 0; i < nb_bones; i++){
		fscanf(fichier, "%f %f %f", &Bones[i][2].x, &Bones[i][2].y, &Bones[i][2].z);
		fscanf(fichier, "%f %f %f", &Bones[i][3].x, &Bones[i][3].y, &Bones[i][3].z);
/*		if (i == (nb_bones -1)){
			for (j = 0; j < nb_bones; j++){
				fscanf(fichier, "%f %f %f", &Bones[j][2].x, &Bones[j][2].y, &Bones[j][2].z);
				fscanf(fichier, "%f %f %f", &Bones[j][3].x, &Bones[j][3].y, &Bones[j][3].z);
			}
		}
*/
	}
	for (i = 0; i < nb_bones; i++){
/*		Bones[i][0].x /= 2.0;
		Bones[i][1].x /= 2.0;
		Bones[i][0].y /= 1.6;
		Bones[i][1].y /= 1.6;
		Bones[i][0].z = (Bones[i][0].z - 2.0) / 2.0;
		Bones[i][1].z = (Bones[i][1].z - 2.0) / 2.0;
*/
		/*
		float intZ1 = Bones[i][2].z;
		float intZ2 = Bones[i][3].z;
		Bones[i][2].x /= 2.0;
		Bones[i][3].x /= 2.0;
		Bones[i][2].z = (Bones[i][2].y) / 1.6;
		Bones[i][3].z = (Bones[i][3].y) / 1.6;
		Bones[i][2].y = -(intZ1 - 2.0)/2.0;
		Bones[i][3].y = -(intZ2 -2.0)/2.0;
*/
//		printf("Bone %d, frame 1. \n\told1 = (%f, %f, %f)\n\told2 = (%f, %f, %f)\n\n", i, Bones[i][0].x, Bones[i][0].y, Bones[i][0].z, Bones[i][1].x, Bones[i][1].y, Bones[i][1].z);
		printf("Bone %d, frame 2. \n\tnew1 = (%f, %f, %f)\n\tnew2 = (%f, %f, %f)\n\n", i, Bones[i][2].x, Bones[i][2].y, Bones[i][2].z, Bones[i][3].x, Bones[i][3].y, Bones[i][3].z);
		//fprintf(fichier3, "%f %f %f\n%f %f %f\n", Bones[i][2].x, Bones[i][2].y, Bones[i][2].z, Bones[i][3].x, Bones[i][3].y, Bones[i][3].z);
	}
	fclose(fichier3);
}