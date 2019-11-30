#include <bits/stdc++.h>
#include <thread>
#include <fstream>
#include <ctime>
#include <climits>
#include <unistd.h>
#include <chrono>
#include <mutex>
#include <semaphore.h>
#include <sys/types.h>

using namespace std;

sem_t mtx;
sem_t mtv;				//semaphore declaration for concurrent writing to shared variables

ifstream input("inp-params.txt");
ofstream out("Primes-DAM.txt");				//File Stream
ofstream timeout("Times.txt");
long int increment();
bool isprime(int n);
void primeprint(long n);
std::vector<int> p;
int value = 1;	
		
long int increment()
{
	return value ++;		
}		

bool isprime(int n){				//function to check for prime
			if (n==1) return false;
			else if (n==2) return true;
			else
			for(int i = 2;i<=sqrt(n);i++){
				if(n%i==0){
					return false;
				}
			}
			return true;
}


void primeprint(long n){				//prime print function as given
			long i = 0;
			while(i<n){
				sem_wait(&mtx);						//using semaphore for thread saftey
				i = increment();
				sem_post(&mtx);
				if(isprime(i)){
					sem_wait(&mtv);
					p.push_back(i);
					sem_post(&mtv);
				}
			}
}

int main(){
	sem_init(&mtx, 0, 1);
	sem_init(&mtv, 0, 1);
	double timetaken=0;
	long n;
	int l,m;
	input>>l;
	input>>m;
	n=pow(10,l);
	std::thread threads[m];
	clock_t tim;
	tim=clock();
	for(int i=0;i<m;i++){
		threads[i]= std::thread(primeprint,n);
	}
	for (int i = 0; i < m; ++i)
	{
		threads[i].join();
	}
	tim=clock()-tim;
	timetaken+=((double)tim)/CLOCKS_PER_SEC;
	timeout<<timetaken<<"       "<<endl;
	sort(p.begin(),p.end());
	for(auto i : p){
		out<<i<<" ";
	}

	sem_destroy(&mtx);
	sem_destroy(&mtv);
	
	return 0;
}