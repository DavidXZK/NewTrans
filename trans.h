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
unordered_map<int,int> extra_pid_slaveid;  //pid-slaveid
ofstream ffout("result_trans.txt",ofstrean::app);
int slave_infect = 0;
vector<int> infect_num;
//***************************************************************
template<typename T>
void vector_to_array(T*array,vector<T>& vectors){
	for(int i=0;i<vectors.size();i++){
		array[i] = vectors[i];
	}
}

void initial_Subarea(){
	if(myid!=0){
        myarea->reset();//感染者状态置0，当前感染置0，disease对象置NULL，diseasestate置0，表示易感者
        for(int i = 0;i < first_infected.size();i ++){
        	myarea->individuals[first_infected.at(i)]->getDisease(0);
        }
    }
    else{
    	 infect_num.clear();
    	 infect_num.push_back(SEED_NUM);
    }

}

void readNet(string filenames,int flag,subarea*apt,unordered_set<int>& sset)
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
				if(sset.find(destid)!=sset.end()){
					individual*inv = new individual(destid,destage,0);
					apt->neighbor_to_update.insert(make_pair(destid,inv));
				}//if
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

void read_file_master(char* filename,char*filename_extra){
	/* pid_slaveid:所有信息
	   extra_pid_slaveid:外围邻居网络,pid-extra i 
	*/
	ifstream fin(filename);
	int slaveid,pid,age,num;
	while(fin>>slaveid>>pid>>age)
	{
		pid_slaveid[pid] = slaveid;
	}
	fin.close();

	ifstream fin_sp(filename_extra);
	while(fin_sp>>pid>>slaveid){
		extra_pid_slaveid[pid] = slaveid;
	}
	fin_sp.close();
}

void read_file_slave(char*filename,char*filename_np,char*filename_up){
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
	ifstream fin_np(filename_np);
	int slaveid,pid,age;
	while(fin_np>>slaveid>>pid)
	{
		if(slaveid == myid)
		{
			myarea->individual_to_upload.push_back(make_pair(0,myarea->individuals.find(pid)->second));
		}
	}
	fin_np.close();
	unordered_set<int> ids;
	ifstream fin_up(filename_up);
	while(fin_up>>slaveid>>pid){
		if(slaveid == myid){
			ids.insert(pid);
		}
	}
	fin_up.close();
	readNet(".//JTNet.txt",1,myarea,ids);
    readNet(".//XXNet.txt",2,myarea,ids);
    readNet(".//GZNet.txt",3,myarea,ids);
    readNet(".//PYNet.txt",4,myarea,ids);
    readNet(".//SQNet.txt",5,myarea,ids);
    readNet(".//TQNet.txt",6,myarea,ids);
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

void generateInfect(vector<vector<int> >& id){
	for(int i = 0;i < numprocs;i ++){
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
		if(it == pid_slaveid.end()){
			cout<<"can't find error"<<endl;
			exit(0);
		}
		int sindex = pid_slaveid[index];
		id[sindex].push_back(index);
	}//for
}//generateInfect

void send_to_master(int size){
	MPI_Send(&size,1,MPI_INT,0,98,MPI_COMM_WORLD);
    MPI_Send(ids,size,MPI_INT,0,97,MPI_COMM_WORLD);
    MPI_Send(sym,size,MPI_INT,0,96,MPI_COMM_WORLD);
    MPI_Send(variety_time,size*4,MPI_DOUBLE,0,95,MPI_COMM_WORLD);
}

void recv_from_slave(int* send_size){
	MPI_Status status,status1,status2,status3;
    int*size = new int[numprocs];
    for(int i = 1;i < numprocs;i ++){
        MPI_Recv(&size[i],1,MPI_INT,j,98,MPI_COMM_WORLD,&status);
        MPI_Recv(masterbuf1[i],size[i],MPI_INT,i,97,MPI_COMM_WORLD,&status1);
        MPI_Recv(masterbuf2[i],size[i],MPI_INT,i,96,MPI_COMM_WORLD,&status2);
        MPI_Recv(masterbuf3[i],size[i]*4,MPI_DOUBLE,i,95,MPI_COMM_WORLD,&status3);
    }//for
    vector<vector<int> > send_ids;
    vector<vector<int> > send_sym;
    vector<vector<double> > send_time;
    for(int i=0;i<numprocs;i++){
    	vector<int> a,b;
    	vector<double> c;
    	send_ids.push_back(a);
    	send_sym.push_back(b);
    	send_time.push_back(c);
    }
    for(int i = 1;i < numprocs;i ++){
    	for(int j = 0;j < size[i];j ++){
    		int ids = masterbuf1[i][j];
    		unordered_map<int,int>::iterator iter = extra_pid_slaveid.find(ids);  //pid-slaveid;
    		if(iter == extra_pid_slaveid.end()){
    			cout<<"can't find ids in ps map master"<<endl;
    			exit(-1);
    		}
    		slaveid = iter->second;
    		send_ids[slaveid].push_back(ids);
    		send_sym[slaveid].push_back(masterbuf2[i][j]);
    		int index = j*4;
    		send_time[slaveid].push_back(masterbuf3[i][index]);
    		send_time[slaveid].push_back(masterbuf3[i][index+1]);
    		send_time[slaveid].push_back(masterbuf3[i][index+2]);
    		send_time[slaveid].push_back(masterbuf3[i][index+3]);
    	}
    }//for
    for(int i=1;i<numprocs;i++){
    	send_size[i] = send_ids[i].size();
    	vector_to_array(masterbuf1[i],send_ids[i]);
    	vector_to_array(masterbuf2[i],send_sym[i]);
    	vector_to_array(masterbuf3[i],send_time[i]);
    }//for
	
}

void send_to_slave(int*size){
	for(int i=1;i<numprocs;i ++){
		MPI_Send(&size[i],1,MPI_INT,i,94,MPI_COMM_WORLD);
   		MPI_Send(ids,size,MPI_INT,i,93,MPI_COMM_WORLD);
    	MPI_Send(sym,size,MPI_INT,i,92,MPI_COMM_WORLD);
    	MPI_Send(variety_time,size*4,MPI_DOUBLE,i,91,MPI_COMM_WORLD);
	}

}

void recv_from_master(double time){
	int size = 0;
	MPI_Status status,status1,status2,status3;
	MPI_Recv(&size,1,MPI_INT,0,94,MPI_COMM_WORLD,&status);
    MPI_Recv(slavebuf1[i],size,MPI_INT,0,93,MPI_COMM_WORLD,&status1);
    MPI_Recv(slavebuf2[i],size,MPI_INT,0,92,MPI_COMM_WORLD,&status2);
    MPI_Recv(slavebuf3[i],size*4,MPI_DOUBLE,0,91,MPI_COMM_WORLD,&status3);
    for(int i = 0;i < size;i ++){
    	int pid = slavebuf1[i];
    	unordered_map<int,individual*>::iterator iter = myarea->neighbor_to_update.find(pid);
    	if(iter == myarea->neighbor_to_update.end()){
    		cout<<"can't find id in neighbor_to_update"<<endl;
    		exit(-1);
    	}
    	individual* inv = iter->second;
    	if(inv->disease_state != 0){
    		cout<<"dump disease_state in salve"<<endl;
    		exit(-1);
    	}
    	inv->disease_state = 1;
    	int index = i*4;
    	inv->dis = new disease(time,slavebuf3[index],slavebuf3[index+1],slavebuf3[index+2],slavebuf3[index+3],slavebuf2[i]);
    } // for size
    
}// recv_from_master

void send_data_to_master(int infect){
	/*more data to send*/
	int slave = infect;
	MPI_Send(&slave,1,MPI_INT,0,89,MPI_COMM_WORLD);
}

void recv_data_from_slave(int* si){
	/*add more data recved*/
	MPI_Status status;
	for(int i=1;i<numprocs;i++){
		MPI_Recv(&si[i],1,MPI_INT,i,89,MPI_COMM_WORLD,&status);
	}
}

void write_result_to_file(){
	ofstream fout("");
	for(int i=0;i<T;i++){
		fout<<infect_num[i]<<endl;
	}
	fout.close();
}