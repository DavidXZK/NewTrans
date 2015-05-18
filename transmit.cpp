#incldue "transmit.h"


void individual::getDisease(double time,subarea*apt){
	double rand1 = (double)rand()/RAND_MAX;
	double rand2 = (double)rand()/RAND_MAX;
	double latency = (-mean_latency)*log(1-rand2);
	if(rand1 < in_rate){
		rand1 = (double)rand()/RAND_MAX;
		double pre_time = (-mean_infect_pre)*log(1 - rand1);
		rand2 = (double)rand()/RAND_MAX;
		double sym_time = (-mean_infect_sym)*log(1 - rand2);
		disease_state = 1;
		dis = new disease(time,latency,pre_time,sym_time,0,true);  //初始化disease
		apt->infected.insert(pid);   //加入感染者群体
	}
	else{
		rand1 = (double)rand()/RAND_MAX;
		double asym_time = (-mean_infect_asym)*log(1-rand1);
		disease_state = 1;
		dis = new disease(time,latency,0,0,asym_time,true);
		apt->infected.insert(pid);
	}
	
}

double getBeta(individual*ninv){

	map_iter iter = ninv->home.begin();
	while(iter != ninv->home.end()){
		
	}
}


void calInfect(unordered_map<int,individual*>& network,subarea*apt,individual*inv,double period){
	double ir = ((inv->age<=cutage)?Ager:1.0)*inv->infectious;   
	map_iter iter = network.begin();
	while(iter != network.end()){
		int id = iter->first;
		individual* ninv = iter->second;
		inv_iter inv_iter1 = (apt->individuals).find(id);
		if(inv_iter1 != apt->individuals.end()){
			ninv = inv_iter1->second;
		}
		else{
			cout<<"in calInfect missid = "<<id<<endl;
			exit(1);
		}
		/*we got all message about one neighbor.
		 *then we traversal the infectious neighbor of this one
		 * 
		 * */
		
	}

}

void Transmit(subarea*apt,individual*inv,double time){
	double latency = inv->dis->latency;
	double pre_time = inv->dis->pre_time;
	double sym_time = inv->dis->sym_time;
	double asym_time = inv->dis->asym_time;
	double infect_time = time - inv->dis->time_infected;
	if(inv->disease->is_sym){  //显性症状
		if(!inv->dis->is_pre_cal && (infect_time > latency)){
			CalInfect( );
		}
	}
	else{
	
	}
}

void transmit(subarea*apt,double time){
	map_iter 
}
