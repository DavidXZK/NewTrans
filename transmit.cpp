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


void calInfect(){
	

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
