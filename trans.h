#include "transmit.h"
#include "mpi.h"
#include "string.h"
#include <stdlib.h>
#include <time.h>

char* parafile = "iparameter.txt";
char* spfilename = ".//slave_pop.txt";
char* sp_filename = "";
int myid,numprocs;
int*seedsize;
int**randseed;
int* infect_id;
int* sym;
double *variety_time;
int**masterbuf1;
int**masterbuf2;
double**masterbuf3;
int*slavebuf1;
int*slavebuf2;
double*slavebuf3;
subarea* myarea = NULL;
unordered_map<int,int>pid_slaveid;     //personid-slaveid
unordered_map<int,individual*> extraInv;   //personid-individual
unordered_map<int,int> extra_pid_slaveid;  //slaveid-personid
ofstream ffout("result_trans.txt",ofstrean::app);
int slave_infect = 0;
vector<int> infect_num;
void initial_Subarea(){
	if(myid!=0){
        myarea->reset();//感染者状态置0，当前感染置0，disease对象置NULL，diseasestate置0，表示易感者
        set_iter iter = myarea->first_infected.begin();
        set_iter iter1 = myarea->first_infected.end();
        while(iter!=iter1){
            myarea->individuals[*iter]->getDisease(0);//初始时间为0,add to infected_now
            iter ++;
        }
    }
}
void readNet(string filenames,int flag,subarea*apt)
{
    string filename = filenames;
	ifstream fin(filename.c_str());
	int pid,degree,destid,destage;
	string line;
	while(fin>>pid>>degree)
	{
        unordered_map<int,Individual*>::iterator it = apt->Individuals.find(pid);
		if(it!=apt->Individuals.end())
		{
			for(int i =0;i<degree;i++)
			{
				fin>>destid>>destage;
                switch(flag)
                {
                    case 1:
                        it->second->home[destid] = destage;
                        break;
                    case 2:
                        it->second->school[destid] = destage;
                        break;
                    case 3:
                        it->second->work[destid] = destage;
                        break;
                    case 4:
                        it->second->friends[destid] = destage;
                        break;
                    case 5:
                        it->second->community[destid] = destage;
                        break;
                    case 6:
                        it->second->commute[destid] = destage;
                        break;
                    default:
                        cout<<"readNet default fault"<<endl;
                        break;
                }//switch	
			}
		}
		else
		{
			getline(fin,line);
		}
	}//while
	fin.close();
}//readNet

void read_file_master(char* filename,char*filename_sp){
	/* pid_slaveid:所有信息
	   extra_pid_slaveid:外围邻居网络 
	*/
	ifstream fin(filename);
	int slaveid,pid,age,num;
	while(fin>>slaveid>>pid>>age)
	{
		pid_slaveid[pid] = slaveid;
	}
	fin.close();
	ifstream fin_sp(filename_sp);
	while(fin_sp>>slaveid>>pid>>age){
		extra_pid_slaveid[pid] = slaveid;
	}
	fin_sp.close();
}

void malloc_memory_master(){
	masterbuf1 = new int*[numprocs];
	masterbuf2 = new int*[numprocs];
	masterbuf3 = new double*[numprocs];
	for(int i=1;i<numprocs;i++){
		masterbuf1[i] = new int[MAX_SLAVE];
		masterbuf2[i] = new int[MAX_SLAVE];
		masterbuf3[i] = new double[MAX_SLAVE];
	}
}//malloc_memory_master

void malloc_memory_slave(){
	infect_id = new int[MAX_NEIGHBOR];
	sym = new int[MAX_NEIGHBOR];
	variety_time = new double[MAX_NEIGHBOR];
	slavebuf1 = new int[MAX_NEIGHBOR];
	slavebuf2 = new int[MAX_NEIGHBOR];
	slavebuf3 = new int[MAX_NEIGHBOR];
}

void memory_release(){
	if(myid==0){
		for(int i=1;i<numprocs;i++){
			delete []masterbuf1[i];
			delete []masterbuf2[i];
			delete []masterbuf3[i];
		}
		delete []masterbuf1;
		delete []masterbuf2;
		delete []masterbuf3;
	}//if
	else{
		delete infect_id;
		delete sym;
		delete variety_time;
		delete slavebuf1;
		delete slavebuf2;
		delete slavebuf3;
	}//else
}//memory_release()

void read_file_slave(char*filename,char*filename_ep){
	ifstream fin(filename);
	int slaveid,pid,age;
	while(fin>>slaveid>>pid>>age)
	{
		if(slaveid == myid)
		{
			myarea->individuals[pid] = new individual(pid,age,slaveid);
		}
	}
	fin.close();
	ifstream fin_ep(filename_ep);
	int slaveid,pid,age;
	while(fin_ep>>slaveid>>pid>>age)
	{
		if(slaveid == myid)
		{
			myarea->updateNeighbor[pid] = new individual(pid,age,0);
		}
	}
	fin_ep.close();
	readNet(".//JTNet.txt",1,myarea);
    readNet(".//XXNet.txt",2,myarea);
    readNet(".//GZNet.txt",3,myarea);
    readNet(".//PYNet.txt",4,myarea);
    readNet(".//SQNet.txt",5,myarea);
    readNet(".//TQNet.txt",6,myarea);
}

void generateInfect(vector<vector<int> >& id){
	for(int i=0;i<numprocs;i++){
		vector<int> which;
		id.push_back(which);
	}
	unordered_set<int> rnum;
	int pid_size = pid_slaveid.size();
	for(int i= 0; i < SEED_NUM; i ++){
		int index = 0;
		while(1){
			index = 1+(int)((double)rand()/((double)RAND_MAX+1.0)*(double)pid_size);
			if(rnum.find(index)!=rnum.end()){
				continue;
			}
			else{
				rnum.insert(index);
				break;
			}
		}//while
		unordered_map<int,int>::iterator it = pid_slaveid.find(index);
		if(it==pid_slaveid.end()){
			cout<<"can't find error"<<endl;
			exit(0);
		}
		int sindex = pid_slaveid[index];
		id[sindex].push_back(index);
	}//for
}

