#include "trans.h"

int main(int argc,char**argv){
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myid);
	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
	srand((myid+1)*(unsigned int)time(NULL));
	ifstream parafin(parafile);
	parameter(parafin);  // read parameter
	if(myid==0){
		read_file_master(spfilename,sp_filename);
		malloc_memory_master();
	}
	else{
		myarea = new subarea(myid);
		read_file_slave(spfilename,epfilename);
		malloc_memory_slave()
	}
	MPI_Barrier(MPI_COMM_WORLD);

	if(myid==0){
		vector<vector<int> > id;
		generateInfect(id);
		seedsize = new int[numprocs];
		randseed = new int*[numprocs];
		for(int i=1;i<numprocs;i++){
			randseed[i] = new int[id[i].size()];
			seedsize[i] = id[i].size();
			for(int j=0;j<id[i].size();j++){
				randseed[i][j] = id[i][j];
			}
		}
		for(int i=1;i<numprocs;i++){
			MPI_Send(&seedsize[i],1,MPI_INT,i,100,MPI_COMM_WORLD);
		}
		for(int i=1;i<numprocs;i++){
			MPI_Send(randseed[i],seedsize[i],MPI_INT,i,99,MPI_COMM_WORLD);
		}
	}
	else{
		MPI_Status status_a,status_b;
		int rec_size = 0;
		MPI_Recv(&rec_size,1,MPI_INT,0,100,MPI_COMM_WORLD,&status_a);
		int*slavebuf = new int[rec_size];// receive rand seed
		MPI_Recv(slavebuf,rec_size,MPI_INT,0,99,MPI_COMM_WORLD,&status_b);
		for(int i=0;i<rec_size;i++){
			myarea->first_infected.insert(slavebuf[i]);
		}
		delete []slavebuf;
	}//else
	MPI_Barrier(MPI_COMM_WORLD);

	for(int i = 0;i < n;i ++){
		initial_Subarea();
		for(int t=1;t<T;t++){
			if(myid!=0){
				vector<double> distime;
				vector<int> sym_or_not;
				vector<int> ids;
				for(int j = 0;j < individual_to_upload.size();j ++){
					pair<int,individual*> &temp = individual_to_upload[j];
					if(temp->first == 0 && (temp.second)->disease_state == 1){
						temp->first = 1;
						ids.push_back((temp.second)->pid);
						sym_or_not.push_back(((temp.second)->dis->is_sym)?1:0);
						distime.push_back((temp.second)->dis->lantency);
						distime.push_back((temp.second)->dis->pre_time);
						distime.push_back((temp.second)->dis->sym_time);
						distime.push_back((temp.second)->dis->asym_time);
					}//if
				}// for j individual_to_upload
				vector_to_array(infect_id,ids);
				vector_to_array(sym,sym_or_not);
				vector_to_array(variety_time,distime);
				send_to_master(ids.size());
				recv_from_master(t-1);
			}//if myid!=0
			else{
				int *send_size = new int[numprocs];
				recv_from_slave(send_size);
				send_to_slave(send_size);
			}

			if(myid != 0){// 传播过程
				unordered_map<int,individual*>::iterator iter = myarea->individuals.begin();
				while(iter != myarea->individuals.end()){
					transmit(t);
					iter ++;
				}//while
				iter = myarea->individuals.begin();
				while(iter != myarea->individuals.end()){ //统计感染人数
					if(iter->second->disease_state == 1){
						slave_infect += 1;
					}
					iter ++;
				}//while
			}//if
			//transfer infect num to master 

		}//for t
	}//for i


}

void initial_Subarea(){
	if(myid!=0){
        myarea->reset();//感染者状态置0，当前感染置0，disease对象置NULL，diseasestate置0，表示易感者
        for(int i = 0;i < first_infected.size();i ++){
        	myarea->individuals[first_infected.at(i)]->getDisease(0);
        }
    }
}

template<typename T>
void vector_to_array(T*array,vector<T>& vectors){
	for(int i=0;i<vectors.size();i++){
		array[i] = vectors[i];
	}
}

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

void send_data_to_master(){//infect num
	MPI_Send(&slave_infect,1,MPI_INT,i,90,MPI_COMM_WORLD);
}

void recv_data_from_slave(){
	int*si = new int[numprocs];
	MPI_Status status;
	for(int i=1;i<numprocs;i++){
		MPI_Recv(&si[i],1,MPI_INT,i,90,MPI_COMM_WORLD,&status);
	}
	int sum = 0;
	for(int i = 1;i < numprocs;i ++){
		sum += si[i];
	}
	infect_num.push_back(sum);
		
}


//new reset 

