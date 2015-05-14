#include <iostream>
#include <vector>
#incldue <tr1/unordered_map>

using namespace std;
using namespace std::tr1;

class disease{
	public:
		double time_infected;
		double latency,pre_time,sym_time,asym_time;
		bool is_sym;
		bool is_pre_cal,is_sym_cal,is_asym_cal;
		Disease(double time,double latencyt,double pre_timet,double sym_timet,double asym_timet,bool sym){
			time_infected = time;
			latency = latencyt;pre_time = pre_timet;sym_time = sym_timet;asym_time = asym_timet;
			is_pre_cal = false;is_sym_cal = false;is_asym_cal = false;
			is_sym = sym;
		}
}
class individul{
	public:
		int pid,age,slaveid,disease_state;
		double susceptibility,infectious,in_rate;//感染概率
		bool is_treat,is_drug,is_immune;//治疗，服用抗病毒药物，注射疫苗
		unordered_map<int,int>home,school,work,friends,community,commute;
		disease* dis;
		Individual(int pids,int ages,int slaveids):pid(pids),age(ages),slaveid(slaveids){
			is_treat = false;
			is_drug = false;
			is_immune = false;
			in_rate = infectious_rate;   //全局变量
		}
		void getDisease(double,double,double,double,double,bool);//判断感染类型
		void freeDisease();
		void Dead();
		void Reset(){
			disease_state = 0;
			susceptibility = 1.0;
			infectious = 1.0
			is_treat = false;
			is_drug = false;
			is_immune = false;
		}		
};

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

class subarea{
	public:
		int aid;
		int drug_used,pdrug_used,vaccine_used;
		subarea(int id):aid(id){
			drug_used = 0;
			pdrug_used = 0;
			vaccine_used = 0;
		}
		unordered_map<int,individual*> individuals;
		vector<int> infected;
		void Reset();
};

void transmit(subarea*,individual*inv,double time);
void calInfect();
