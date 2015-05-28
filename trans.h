#include "transmit.h"
#include "mpi.h"
#include "string.h"
#include <stdlib.h>
#include <time.h>

char* parafile = "iparameter.txt";
int myid,numprocs;
subarea* myarea = NULL;
unordered_map<int,int>pid_slaveid;     //personid-slaveid
unordered_map<int,individual*> extraInv;   //personid-individual
unordered_map<int,vector<int> > slaveid_pid;  //slaveid-personid
ofstream ffout("result_trans.txt",ofstrean::app);

void initial_Subarea(){

}

void read_malloc_master(char* filename,char* filename_pi,char*filename_sp){
	ifstream fin(filename1);
	int slaveid,pid,age,num;
	while(fin>>slaveid>>pid>>age)
	{
		pid_slaveid[pid] = slaveid;
	}
	fin.close();
	ifstrean fin_pi(filename_pi);
	while(fin_pi>>pid>>age){
		individual* inv = new individual(pid,age,0);
		extraInv.insert(make_pair(pid,inv));
	}
	fin_pi.close();
	ifstream fin_sp(filename_sp);
	while(fin_sp>>slaveid>>num){
		vector<int> persons;
		for(int i=0;i<num;i++){
			fin>>pid;
			persons.insert(pid);
		}
		slaveid_pid.insert(make_pair(slaveid,persons));
	}
	fin_sp.close();
}

void read_malloc_slave(char*filename){
	
}