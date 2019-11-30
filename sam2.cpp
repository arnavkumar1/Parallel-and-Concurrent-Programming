#include <bits/stdc++.h>
#include <thread>
#include <fstream>
#include <ctime>
#include <climits>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
#include <sys/types.h>

using namespace std;

ifstream input("inp-params.txt");
ofstream out("Primes-SAM2.txt");				//File Stream
ofstream timeout("Times.txt");
sem_t mtx;
std::vector<int> p;

void prime(vector<int> a){									//function to check for prime
	for(int i=0; i<a.size(); i++){
		int isprime=1;
		for(int j=2; j<=sqrt(a[i]); j++){
			if(a[i]%j==0){
				isprime=0;
				break;
			}
		}
		string s="";
		if(isprime){
			sem_wait(&mtx);
			p.push_back(a[i]);
			sem_post(&mtx);
		}
	}
}

int main(){
	double timetaken=0;
	sem_init(&mtx, 0, 1);
	int l,m,n;
	input>>l;
	input>>m;
	n=pow(10,l);
	vector<vector<int>> a(m);
	for(int i=2; i<=n; i++){
		if(i!=2){
			if(i%2!=0){			
				a[i%m].push_back(i);					//insert numbers in given format to each thread
			}
		}	
	}
	std::thread threads[m];
	clock_t tim;
	tim=clock();
	for(int i=0; i<m; i++){
		threads[i]=std::thread (prime,a[i]);
	}
	tim=clock()-tim;
	timetaken+=((double)tim)/CLOCKS_PER_SEC;
	timeout<<timetaken<<"       "<<endl;
	for (int i=0; i<m; i++)
	{
		threads[i].join();
	}
	sort(p.begin(),p.end());
	for(auto i : p){
		out<< i <<" ";
	}
	sem_destroy(&mtx);
	return 0;
}