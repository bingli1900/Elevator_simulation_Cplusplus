// elevator_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
using namespace std;
using namespace elevator_51044_final;

int main()
{

#if 1					    //this chunk of code is used to run a demonstration example
							//which is designed to cover all extreme cases, like "full elevator"/"sleeping elevator"/"people waiting at multiple floors"....

	Person p0(0, 15, 0);    //person 0 getting to floor 0 at time 0 waiting the elevator, going to floor 15
	Person p1(3, 1, 10);	//person 1 getting to floor 3 at time 10 waiting the elevator, going to floor 1
	Person p2(6, 10, 3);	//person 2 getting to floor 6 at time 3 waiting the elevator, going to floor 10
	Person p3(8, 0, 20);	//person 3 getting to floor 8 at time 20 waiting the elevator, going to floor 0
	Person p4(12, 15, 6);	//person 4 getting to floor 12 at time 6 waiting the elevator, going to floor 15
	Person p5(5, 10, 60);	//person 5 getting to floor 5 at time 60 waiting the elevator, going to floor 10

	QuanTimer T;
							//generate task, i.e. waiting queue
	T.insert_person(p0);
	T.insert_person(p1);
	for (int i = 0; i < 15;i++)  //suppose a group of 15 come with the same data as person 2
		T.insert_person(p2);
	for (int i = 0; i < 15; i++)  //suppose a group of 15 come with the same data as person 3
		T.insert_person(p3);
	T.insert_person(p4);
	T.insert_person(p5);

	for (int tt = 0; tt < 120; tt++)
	    T.time_step();

	T.stat.processing();
	T.stat.printResult();

#else						//this chunk of code is used to test statistically with a huge task input
							//to assess the design of a particular algorithm


#endif

	int a;
	cin >> a;
	return 0;
}