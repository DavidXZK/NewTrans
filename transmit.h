#include <iostream>
#include <vector>
#incldue <tr1/unordered_map>

using namespace std;
using namespace std::tr1;

typedef unordered_map<int,individual*>::iterator map_iter;
const int MAX_SLAVE = 500000;
const int MAX_NEIGHBOR = 100000;
int n,T,cutage,SEED_NUM;
double LAMDA,infectious_rate,death_rate;
double mean_latency,mean_infectious_pre,mean_infectious_sym,mean_infectious_asym;
double Age_infectious_rate,Age_susceptible_rate;
double Infectious_rate_pre,Infectious_rate_sym,Infectious_rate_asym;
double school_intensity,home_intensity,work_intensity;
double community_intensity,friends_intensity,commute_intensity;
double treat_infectious,susceptible_prevention,infectious_prevention;
double ill_rate_prevention,ill_rate_treat,infectious_treat;
double get_treat_rate,get_immune_rate;
double school_decrease,work_decrease;

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
		unordered_map<int,int>home,school,work,friends,community,commute;
		disease* dis;
		Individual(int pids,int ages,int slaveids):pid(pids),age(ages),slaveid(slaveids){
			is_treat = false;
			is_drug = false;
			is_immune = false;
			in_rate = infectious_rate;   //全局变量
		}
		void getDisease(double time);//判断感染类型
		void freeDisease(){
			disease_state = 2;
			delete dis;
			dis = NULL;
		}
		void dead(){
			disease_state = 3;
			delete dis;
			dis = NULL;
		}
		void reset(){
			disease_state = 0;
			susceptibility = 1.0;
			infectious = 1.0
			is_treat = false;
			is_drug = false;
			is_immune = false;
			delete dis;
			dis = NULL;
		}		
};

class subarea{
	public:
		int aid;
		int drug_used,pdrug_used,vaccine_used;
		int recover_num,dead_num;
		subarea(int id):aid(id){
			drug_used = 0;
			pdrug_used = 0;
			vaccine_used = 0;
		}
		unordered_map<int,individual*> individuals;  //区域人口
		unordered_map<int,individual*> neighbor_to_update;  //网络邻居
		vector<pair<int,individual*> > individual_to_upload;   //上传个体信息
		vector<int> first_infected;
		void reset();
};

void getParameter(ifstream& fin);
void transmit(subarea*,double);
pair<double,int> beta(individual*,double);
double calBeta(unordered_map<int,individual*>& network,double intensity,double time);
