#include <bits/stdc++.h>
#include <thread>
#include <fstream>
#include <ctime>
#include <climits>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
#include <random>
#include <atomic>
#include <mutex>
#include <sys/types.h>
using namespace std;

void testCS(int n,int id, int k, int l1, int l2);

string printTime(time_t curtime);
ifstream input("inp-params.txt");							//file stream
ofstream out("backoff.txt");
double avgtimetaken=0;
std::vector<string> p;	
atomic <bool> state = ATOMIC_FLAG_INIT;
sem_t mtx;

class Backoff{
	public:
		int minDelay, maxDelay;
		int limit;
		int random;
		Backoff(int min, int max) {
			minDelay = min;
			maxDelay = max;
			limit = minDelay;
			random = rand()%100;
	}
	void backoff(){
		int delay = random++;
		limit = min(maxDelay, 2 * limit);
		usleep(delay);
	}
	friend class Backofflock;
};

class Backofflock{
	public:
		int Min_Delay=0;
		int Max_Delay=500;
		void lock(){
			Backoff bo(Min_Delay,Max_Delay);
			while(true){
				while(state){};
				if(!state.exchange(true)){
					return;
				}
				else{
					bo.backoff();
				}
			}
		}
		void unlock(){
			state = false;
		}
};
Backofflock lockobj;

int main(){
	sem_init(&mtx,0,1);
	int n,k,l1,l2;
	input>>n;
	input>>k;
	input>>l1;
	input>>l2;
	std::thread threads[n];
	atomic <bool> state = ATOMIC_FLAG_INIT;
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
		lockobj.lock();
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
		lockobj.unlock();
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
