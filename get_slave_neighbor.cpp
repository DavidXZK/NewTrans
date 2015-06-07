#include <iostream>
#include <unordered_map>
#include <fstream>

using namespace std;
using namespace std::tr1;

typedef unordered_map<int,int>::iterator map_iter;

unordered_map<int,int> ps_map;
unordered_map<int,int> neighbor_map;
unordered_map<int,int> upload_map;
int main(int argc,char**argv){
	ifstream fin("slave");
	int slaveid,pid,age;
	while(fin>>slaveid>>pid>>age){
		ps_map[pid] = slaveid;
	}
	for(int i=1;i<82;i++){
		unordered_set<int> slave_set;
		map_iter iter = ps_map.begin();
		while(iter != ps_map.end()){
			if(iter->second == i){
				slave_set.insert(iter->first);
			}
			iter ++;
		}//while
		readwholenet(i,slave_set);
	}//for
	ofstream fout("neighbor_map.txt");
	map_iter iter1 = neighbor_map.begin();
	while(iter1 != neighbor_map.end()){
		fout<<iter1->first<<" "<<iter1->second<<endl;
		iter1 ++;
	}
	fout.close();
	ofstream fout1("upload_map.txt");
	map_iter iter2 = upload_map.begin();
	while(iter2 != upload_map.end()){
		fout<<iter2->first<<" "<<iter2->second<<endl;
		iter2 ++;
	}
	fout1.close();
}

void readnet(int i,const char* filename,unordered_set<int>& sset){
	int pid,degree,destid,destage;
	ifstream fin(filename);
	string line = "";
	while(fin>>pid>>degree){
		if(sset.find(pid) != sset.end()){
			for(int i=0;i<degree;i++){
				fin>>destid>>destage;
				if(sset.find(destid) == sset.end()){
					int sid = (ps_map.find(destid))->second;
					neighbor_map.insert(make_pair(destid,sid));  // not in and truely in
					upload_map.insert(make_pair(destid,i));
				}//if
			}//for
		}//if
		else
		{
			getline(fin,line);
		}
	}//while
	fin.close();
}

void readwholenet(int i,unordered_set<int>&sset){
	readnet(".//JTNet.txt",sset);
    readnet(".//XXNet.txt",sset);
    readnet(".//GZNet.txt",sset);
    readnet(".//PYNet.txt",sset);
    readnet(".//SQNet.txt",sset);
    readnet(".//TQNet.txt",sset);
}