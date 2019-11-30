#include <bits/stdc++.h>
#include <thread>
#include <fstream>
#include <atomic>
#include <ctime>
#define max_threads 40

using namespace std;


int* arr;						//integer array declared globally

void mergesort(int, int);
void merge(int, int);
ifstream input("inp_merge.txt");
atomic <int> thread_count(0);

/***************** mergesort ***************/
/*
	input : left index, right index of the array which is to be sorted

	divide the array into 2 parts.
	as the 2 parts of array can be sorted independently,
	we create 2 threads, 1 for the left subarray ,  1 for the right array 
	and spawn the threads with recursive mergesort function with appropriate parameters

*/

void mergesort(int left, int right)			
{
	if(left == right)
		return;

	
	//thread rightthread;

	if(thread_count >= max_threads)
	{
		mergesort(left, (left + right)/2);
		mergesort((left + right)/2 + 1, right);
	}
	else
	{
		thread leftthread;
		thread rightthread;
		thread_count += 2;
		leftthread = thread(mergesort, left, (left + right)/2);
		rightthread = thread(mergesort, (left + right)/2 + 1, right);
		leftthread.join();
		rightthread.join();
		thread_count -= 2;
	}
	merge(left, right);
	return;
}

/*
	input: 2 indices left and right

	as the 2 subarrays are sorted this method merges them together and gives output as a sorted array.
	this method is implemented by 2-pointer method

	keeping track of which index of the left and right subaray were inserted and consequently building the sorted array

*/

void merge(int left, int right)
{

	int lend = (left + right)/2, rstart = lend + 1;
	int* temp = new int[right - left + 1];

	int lmax = lend - left + 1;
	int rmax = right - rstart + 1;
	int current = 0, lindex = 0, rindex = 0;
	
	for(int i = 0; i < right - left + 1; i++)
	{
		if(lindex < lmax && rindex < rmax)
		{
			if(arr[left + lindex] < arr[rstart + rindex])
				temp[i] = arr[left + lindex++];
			else
				temp[i] = arr[rstart + rindex++];
		}
		else if(lindex < lmax)
		{
			temp[i] = arr[left + lindex++];
		}
		else if(rindex < rmax)
		{
			temp[i] = arr[rstart + rindex++];
		}
	}

	for(int i = left; i <= right; i++)
		arr[i] = temp[i - left];

	return;
}

int main()
{

	int n;
	cout<<"Enter number of elements in array"<<endl;
	cin>>n;
	arr = new int[n];
	srand(time(NULL));
	for(int i = 0; i < n; i++)
		arr[i] = rand()%1000 + 1;

	mergesort(0, n-1);

	cout << "Sorted array is : "<<endl; 

	for(int i = 0; i < n; i++)
		cout << arr[i] << " ";
	cout << endl;

	return 0;
}