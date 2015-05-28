#include "trans.h"

int main(int argc,char**argv){
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	srand((myid+1)*(unsigned int)time(NULL));
	ifstream parafin(parafile);
	parameter(parafin);
	
}