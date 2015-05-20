#include <iostream>
#include <vector>
#incldue <tr1/unordered_map>

using namespace std;
using namespace std::tr1;

typedef unordered_map<int,individual*>::iterator map_iter;

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
		int pid,age,slaveid,disease_state;  //disease_state = 0,1,2,3
		double susceptibility,infectious,in_rate;//感染概率
		bool is_treat,is_drug,is_immune;//治疗，服用抗病毒药物，注射疫苗
		unordered_map<int,individual*>home,school,work,friends,community,commute;
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
		unordered_map<int,individual*> extraNeighbor;
		vector<int> infected;
		void Reset();
};

void transmit(subarea*,individual*inv,double time);
void calInfect(subarea*,individual*,);
