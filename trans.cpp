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
//new reset 
