#incldue "transmit.h"


void individual::getDisease(double time){
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
		//apt->infected.insert(pid);   //加入感染者群体
	}
	else{
		rand1 = (double)rand()/RAND_MAX;
		double asym_time = (-mean_infect_asym)*log(1-rand1);
		disease_state = 1;
		dis = new disease(time,latency,0,0,asym_time,true);
		//apt->infected.insert(pid);
	}
}

void subarea::reset(){
	drug_used = 0;
	pdrug_used = 0;
	vaccine_used = 0;
	recovery_num = 0;
	dead_num = 0;
	map_iter iter1 = individuals.begin();
	map_iter iter2 = individuals.end();
	while(iter1 !=iter2){
		if(iter1->second->disease_state != 0){
			iter1->second->reset();
		}
		iter1 ++;
	}
	iter1 = neighbor_to_update.begin();
	iter2 = neighbor_to_update.end();
	while(iter1 !=iter2){
		if(iter1->second->disease_state != 0){
			iter1->second->reset();
		}
		iter1 ++;
	}//while
	for(int i = 0;i < individual_to_upload.size();i ++){
		individual_to_upload[i].first = 0;
	}
}
void parameter(ifstream& fin){
    fin>>n>>T>>cutage>>SEED_NUM;
    fin>>LAMDA>>infectious_rate>>death_rate;
    fin>>mean_latency>>mean_infectious_pre>>mean_infectious_sym>>mean_infectious_asym;
    fin>>Age_infectious_rate>>Age_susceptible_rate;
    fin>>Infectious_rate_pre>>Infectious_rate_sym>>Infectious_rate_asym;
    fin>>school_intensity>>home_intensity>>work_intensity>>community_intensity;
    fin>>friends_intensity>>commute_intensity;
    fin>>treat_infectious>>susceptible_prevention>>infectious_prevention;
    fin>>ill_rate_prevention>>ill_rate_treat>>infectious_treat;
    fin>>get_treat_rate>>get_immune_rate;
	fin>>school_decrease>>work_decrease;
}

double calbeta(unordered_map<int,int>& network,double intensity,double time,subarea*apt){  //cal beta of one network
	unordered_map<int,int>::iterator iter = network.begin();
	unordered_map<int,individual*>::iterator iter1;
	double sum = 0;
	while(iter != network.end()){
		int pid = iter->first;
		individual* inv = NULL;
		iter1 = apt->individuals.find(pid);
		if(iter1 != apt->individuals.end()){   //in inner network
			inv = iter1->second;
		}
		else{
			iter1 = apt->neighbor_to_update.find(pid);
			if(iter1 != apt->neighbor_to_update.end()){
				inv = iter1->second;
			}
		}
		double tsum = 0;
		if(inv->dis->disease_state == 1){
			double latency = inv->dis->lantency;
			double time_pre = inv->dis->infect_pre_time;
			double time_sym = inv->dis->infect_sym_time;
			double time_asym = inv->dis->infect_asym_time;
			if(inv->disease->sym_or_not){      //sym 显式感染
				double sym_time = time - inv->dis->time_infected;
				if(sym_time>=latency&&(sym_time<=latency + time_pre)){   //前驱期
					tsum = Infectious_rate_pre*LAMDA*intensity*((inv->age>=cutage)?1.0:Age_infectious_rate)*inv->infectious;
				}
				else if(sym_time>latency + time_pre){      //显性症状期
					tsum = Infectious_rate_sym*LAMDA*intensity*((inv->age>=cutage)?1.0:Age_infectious_rate)*inv->infectious;
				}
			}
			else{      //asym
				double asym_time = time - inv->dis->time_infected;
				if(asym_time >= latency){
					tsum = Infectious_rate_asym*LAMDA*intensity*((inv->age>=cutage)?1.0:Age_infectious_rate)*inv->infectious;
				}//if
			}//else
		}// if
		sum += tsum;
		iter ++;
	}//while
	return sum;
}//calBeta

pair<double,int> beta(individual* inv,double time,subarea* apt){    // tranverse every network 
	double sum = 0;
	int num = 0;
	sum += calbeta(inv->home,home_intensity,time,apt); 
	sum += calbeta(inv->school,school_intensity,time,apt);
	sum += calbeta(inv->friends,friends_intensity,time,apt);
	sum += calbeta(inv->work,work_intensity,time,apt);
	sum += calbeta(inv->community,community_intensity,time,apt);
	sum += calbeta(inv->commute,commute_intensity,time,apt);
	double age_rs = (inv->age>=cutage)?1.0:Age_susceptible_rate;
	age_rs *= inv->susceptibility;
	sum *= age_rs;
	num = inv->home.size() + inv->school.size() + inv->friends.size() + inv->work.size();
	num += inv->community.size() + inv->commute.size(); 
	return pair<double,int> result(sum,num);
}

void transmit(double time,subarea*apt){
	/*遍历某一区域中的所有个体，除去死亡以及康复的，
	其他的分类处理*/
	map_iter iter = apt->individuals.begin();
	while(iter != apt->individuals.end()){
		individual* inv = iter->second;
		int disease_state = inv->disease_state;
		if(disease_state==0){
			pair<double,int> beta = beta(inv,time);
			//blabla 计算,是否感染
			double r = 1 - exp(-beta.first/beta.second);
			double rand = (double)rand()/RAND_MAX;
			if(r<=rand){
				//infected
				inv->getDisease(time);
			}  
		}
		if(disease_state==1){   //infected
			double delta_time = time - inv->dis->time_infected;   //感染时间
			double sum_sym_time = inv->dis->lantency + inv->dis->pre_time + inv->dis->sym_time;
			double sum_asym_time = inv->dis->lantency + inv->dis->asym_time;
			if(inv->disease->is_sym){    //显性症状
				if(delta_time >= sum_sym_time){
					double random = (double)rand()/RAND_MAX;
					double dr = death_rate;
					if(random<=dr){
						inv->dead();
					}
					else{
						inv->freeDisease();
					}//if-else
				}//if
			}
			else{
				if(delta_time >= sum_asym_time){
					inv->freeDisease();
				}//if
			}
		}// infected
		iter ++;
	}
}
