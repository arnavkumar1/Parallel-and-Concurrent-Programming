#include <bits/stdc++.h>
#include <thread>
#include <fstream>
#include <ctime>
#include <climits>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
#include <random>
#include <mutex>
#include <sys/types.h>

using namespace std;
ifstream input("inp-params.txt");							//file stream
ofstream out("Bakery-out.txt");
std::vector<string> p;						//log file
											
sem_t mtx;											//using mutex to write to log file
bool* flag;
long long* label;

double avgtimetaken=0;

void lock (int id,int n){									//function lock

    flag[id]=true;
    long long max = label[0];
    for (int j = 1; j < n; j ++) {
      if (label[j] > max) {
        max = label[j];
      }
    }
    label[id] = max + 1;
     for (int k=0;k<n;k++){
        	while((id!=k) &&flag[k] && ((label[k]<label[id]) || (label[id] == label[k] && k<id))){};
    }
 	
}
void unlock (int id){									//function unlock
   	flag[id]=false;
 }


string printTime(time_t curtime) {						//function to convert time to print format
  struct tm* ptime = localtime(&curtime);
  char currentTime[100];
  strftime(currentTime, 100, "%T", ptime);
  return currentTime;
}

void testCS(int n,int id, int k, int l1, int l2){				//Test CS function
	clock_t tim;
  	double const exp_dist_lambda_1 = l1;
  	double const exp_dist_lambda_2 = l2;

  	int seed = chrono::system_clock::now().time_since_epoch().count();

  	default_random_engine rg(seed);
  	exponential_distribution <double> dist_1(exp_dist_lambda_1);
  	exponential_distribution <double> dist_2(exp_dist_lambda_2);

  	double t1d = dist_1(rg);
  	double t2d = dist_2(rg);

  	int t1 = (int)t1d;
  	int t2 = (int)t2d;
  	
	for(int i=1;i<=k;i++){
		const time_t reqTime = time(0);
		string st=to_string(i)+"th CS request at ";
		st=st+printTime(reqTime);
		st=st+" by thread "+to_string(id);
		//out<<st<<endl;
		sem_wait(&mtx);
		p.push_back(st);
		sem_post(&mtx);
		tim=clock();
		lock(id,n);
		const time_t enterTime=time(0);
		st.clear();
		string st1=to_string(i)+"th Entry at ";
		st1=st1+printTime(enterTime);
		st1=st1+" by thread "+to_string(id);
		//out<<st1<<endl;
		sem_wait(&mtx);
		p.push_back(st1);
		sem_post(&mtx);
		tim=clock()-tim;
		avgtimetaken+=((double)tim)/CLOCKS_PER_SEC;
		sleep(t1);
		unlock(id);
		const time_t exitTime=time(0);
		st.clear();
		string st2=to_string(i)+"th Exit at ";
		st2=st2+printTime(exitTime);
		st2=st2+" by thread "+to_string(id);
		//out<<st<<endl;
		sem_wait(&mtx);
		p.push_back(st2);
		sem_post(&mtx);
		sleep(t2);
	}



}

int main(){
	sem_init(&mtx,0,1);
	int n,k,l1,l2;
	flag = new bool[n];
	label = new long long[n];
	for(int i=0;i<n;i++){
      flag[i] = false;
      label[i] = 0;
    }
	input>>n;
	input>>k;
	input>>l1;
	input>>l2;
	std::thread threads[n];
	for(int i=0; i<n; i++){
		threads[i]=std::thread (testCS,n,i+1,k,l1,l2);			
	}
	for (int i=0; i<n; i++)
	{
		threads[i].join();
	}
	for(vector<string>::iterator it = p.begin(); it != p.end(); ++it) {
    	out<<*it<<endl;
  	}
	avgtimetaken/=n;
	avgtimetaken/=k;
	cout<<avgtimetaken<<endl;
	return 0;
}