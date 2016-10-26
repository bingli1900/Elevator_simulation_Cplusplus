// stdafx.cpp : source file that includes just the standard includes
// elevator_test.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

using elevator_51044_final::Direction;
using elevator_51044_final::FLOOR_NUM;

void 
elevator_51044_final::Statistics::processing(){
	average_waiting_time = (double)total_waiting_time / num_people;
	for (int i = 0; i < FLOOR_NUM; i++)
		for (int j = 0; j < FLOOR_NUM; j++)
			average_waiting_time_matrix[i][j] = (double)waiting_matrix[i][j] / num_people_matrix[i][j];
}

void
elevator_51044_final::Statistics::printResult() const{
	std::cout << "********Elevator Testing Result Summary**********\n" << std::endl;
	std::cout << "\t# of people\t\t\t" << num_people << std::endl;
	std::cout << std::endl;
	std::cout << "\ttotal time\t\t\t" << total_waiting_time << std::endl;
	std::cout << std::endl;
	std::cout << "\taverage waiting time\t\t" << average_waiting_time << std::endl;
	std::cout << "*************************************************" << std::endl;
	//std::cout << "\tprinting waiting time matrix:" << std::endl;
	//print_matrix((double *)average_waiting_time_matrix);
	//std::cout << "*************************************************" << std::endl;
}


void 
elevator_51044_final::Algorithm_SingleElevator_Simple::processing()
{
	//naive meaning of "up" and "down" boolean variables here would be
	//if there is need for the elevator to go up or go down, which depends on series of 
	//inquery like, whether there are people going up(or down) in the elevator, whether there
	// are buttons pressed from higher floors
	//(and further it's "going-up" button or "going-down" button)
	bool up = false, down = false;
	int up_loc = -1, down_loc = -1;
	int lower_bound, upper_bound;

	if (!elevator_full){
		lower_bound = current_floor;
		upper_bound = current_floor;
	}
	else{
		//if the elevator is full when loading at a particular floor, it has to 
		//leave this floor even though the button of this floor is still on(which means there are
		//still people waiting on this floor, and they have to take the next cart or next round)
		lower_bound = current_floor + 1;
		upper_bound = current_floor - 1;
	}

	for (int ii = current_floor; ii < FLOOR_NUM; ii++){
		//if the current state is "going up", then it will respond to tasks of "going up"
		//for example, if elevator is at 5, and people in it are going to 10th floor,
		//then it will ignore the person who is waiting to get down from 8th floor to 2nd floor
		if (ii >= lower_bound)
			//somebody is waiting to go up from a higher floor, then go get him
			up = up || bfloor_up[ii];
		//somebody already in the elevator is going up, then go up
		up = up || belevator[ii];
		//respond to the "nearest" task
		if (up && up_loc == -1) up_loc = ii;
	}
	if (!up){
		//if nobody is waiting for going up, then respond to tasks "going down" from higher levels
		//otherwise, ignore "going-down" requests from higher floor for this moment.
		for (int ii = current_floor; ii >= 0; ii--){
			if (ii > current_floor)
				//tasks of people going down from a higher floor
				up = up || bfloor_down[ii];
			if (up && up_loc == -1) up_loc = ii;
		}
	}

	//take care of downside task responding order, symmetric to the code above, just skip the comments 
	for (int ii = current_floor; ii >= 0; ii--){
		if (ii <= upper_bound)
			down = down || bfloor_down[ii];
		down = down || belevator[ii];
		if (down && down_loc == -1) down_loc = ii;
	}
	if (!down){
		for (int ii = current_floor; ii >= 0; ii--){
			if (ii < current_floor)
				down = down || bfloor_up[ii];
			if (down && down_loc == -1) down_loc = ii;
		}
	}

	//codes prior to this taking care of assigning "up" and "down"
	//codes below are 
	if (direction == UP) {     //previous direction is UP
		//if there is still tasks aligned with its previous direction, then continue on this direction
		if (up){
			nextstop = up_loc;
		}
		//if not and othe direction has tasks, then go towards the other direction
		else if (down){
			direction = DOWN;
			nextstop = down_loc;//
		}
		//otherwise the elevator would be free, fall into sleep
		else
			direction = SLEEP;
	}
	else if (direction == DOWN){
		//previous direction is DOWN, the same logic with "direction is UP" case
		if (down){
			nextstop = down_loc;
		}
		else if (up){
			direction = UP;
			nextstop = up_loc;// (int)std::fmin(down_loc, 0)
		}
		else
			direction = SLEEP;
	}
	else{
		//if the elevator was sleeping previously, then check if there is new task.
		//there is task above this floor
		if (up){
			direction = UP;
			nextstop = up_loc;
		}
		//there is task below this floor
		else if (down){
			direction = DOWN;
			nextstop = down_loc;
		}
		//otherwise still sleep
		else
			nextstop = current_floor;
	}
}

vector<int> 
elevator_51044_final::Algorithm_SingleElevator_Simple::nextStop(){
	//return the next destination info to update elevator
	//since there might be multiple elevators, we use a vector to pass this result
	vector<int> ret;
	ret.push_back(nextstop);
	std::cout << "next stop of this elevator is at floor: " << nextstop << std::endl;
	return ret;
}

vector<Direction>
elevator_51044_final::Algorithm_SingleElevator_Simple::upOrDown(){
	//return the direction info to update elevator
	vector<Direction> ret;
	std::string str_direction = direction == UP ? "up" : direction == DOWN ? "down" : "sleep";
	std::cout << "next state/direction of this elevator is: " << str_direction << std::endl;
	ret.push_back(direction);
	return ret;
}

elevator_51044_final::Person::Person()
{
	original_floor = 0;
	destination_floor = 0;
	goingup = false;
	in_time = 0;
	out_time = 0;
}

void 
elevator_51044_final::Person::setOutTime(int time){
	out_time = time;
}

elevator_51044_final::Person::Person(unsigned int i, unsigned int f, int time){
	original_floor = i;
	destination_floor = f;
	goingup = ((int)f - (int)i>0);
	in_time = time;
	out_time = 0;
}

void
elevator_51044_final::Floor::checkNewArrival(Person const &p, unsigned int const &time){
	//std::cout << p.getOrgFlr() << " " << p.getInTime() << std::endl;
	if (p.getInTime() == time && p.getOrgFlr() == floor_num){
		if (p.goingUp()){
			//std::cout << "one person getting in line going up at floor " << floor_num << std::endl;
			waiting_up.push(p);
			button_up = true;
		}
		else{
			//std::cout << "one person getting in line going down at floor " << floor_num << std::endl;
			waiting_down.push(p);
			button_down = true;
		}
	}
}

void 
elevator_51044_final::Elevator::get_in(Person const &p)
//person p get into the elevator
{
	if (vacancy > 0){
		vacancy -= 1;
		time_delayed += 1;
		buttons[p.getDstFlr()] = true;
		people.insert(std::pair<int, Person>(p.getDstFlr(), p));
		std::cout << "One person getting into the elevator(at floor" << "  " << p.getOrgFlr() << "going to floor" << " " << p.getDstFlr() << ")" << std::endl;
		std::cout << "now # of people in elevator is : " << 13 - vacancy << std::endl; //<< people.size() << " or "
	}
	else
		std::cout << "Elevator is full." << std::endl;
}


void 
elevator_51044_final::Elevator::update(vector<int> nextStop, vector<Direction> dirs, Floor floors[], Statistics &s, int time){

	direction = dirs[number];

	if (direction == SLEEP){
		std::cout << "elevator is waiting, no task right now." << std::endl;
	}
	else{
		if (nextStop[number] == thisfloor || time_delayed != 0){//
			//if time_since_stop == 0, then it just stopped, i.e. first cycle since stop
			if (time_delayed == 0){
				//getting off part
				int flag = 0;
				std::cout << "elevator " << " " << number << " " << "stopping at floor" << " " << thisfloor << std::endl;
				auto it = people.find(thisfloor);
				while (it != people.end()){
					std::cout << "# of people in elevator:" << " " << people.size() << std::endl;
					std::cout << "one person getting off the elevator(from floor" << " " << it->second.getOrgFlr() << ")" << std::endl;
					it->second.setOutTime(time);
					it->second.stat(s);
					people.erase(it);
					time_delayed += 1;
					it = people.find(thisfloor);
					flag = 1;
				}
				vacancy += time_delayed;
				if (vacancy > 0)
					full = false;

				//getting on part
				if (dirs[number] == UP && floors[thisfloor].button_up){
					while (!floors[thisfloor].waiting_up.empty() && vacancy > 0){
						std::cout << "here it comes" << std::endl;
						Person p = floors[thisfloor].waiting_up.front();
						floors[thisfloor].waiting_up.pop();
						get_in(p);
						flag = 1;
					}
					if (vacancy == 0)
						full = true;
					floors[thisfloor].update_up();
				}

				else if (dirs[number] == DOWN && floors[thisfloor].button_down){
					while (!floors[thisfloor].waiting_down.empty() && vacancy > 0){
						std::cout << "here it comes" << std::endl;
						Person p = floors[thisfloor].waiting_down.front();
						floors[thisfloor].waiting_down.pop();
						get_in(p);
						flag = 1;
					}
					if (vacancy == 0)
						full = true;
					floors[thisfloor].update_down();
				}

				//count the first person movement
				if (flag){
					time_delayed--;
					if (time_delayed == 0)
						buttons[thisfloor] = false;
				}
			}

			else{
				//time counting, count the rest of people's movement
				time_delayed--;
				//std::cout << time_delayed << "here" << std::endl;
				if (time_delayed == 0)
					buttons[thisfloor] = false;//this will change he result of stopHere through algorithm

				//and end the update queue cycle.
			}
		}

		else if (nextStop[number] > thisfloor){
			//will not stop at this floor
			thisfloor++;
			time_delayed = 0;
		}
		else{
			thisfloor--;
			time_delayed = 0;
		}
	}
}


void 
elevator_51044_final::QuanTimer::time_step(){
	//show the time at this step
	std::cout << "time is:" << time << std::endl;
	std::cout << "# of people in elevator:" << elevator.people.size() << std::endl;
	std::cout << "current floor:" << elevator.thisfloor << std::endl;
	//check new people arriving at this time step， if any, add them into corresponding queues
	for (int i = 0; i < FLOOR_NUM; i++){
		for (auto it = plist.begin(); it != plist.end(); it++){
			floors[i].checkNewArrival(*it, time);
		}
	}

	for (int i = 0; i < FLOOR_NUM; i++){
		//update button information, as input to algorithm class
		floor_up[i] = floors[i].check_up();
		floor_down[i] = floors[i].check_down();
		elevator_button[i] = elevator.buttons[i];
	}

	//first update the elevator states for next time slice
	alg.update(floor_up, floor_down, elevator_button, elevator.getDirection(), elevator.getFloor(), elevator.full);
	//processing procedure: including returning the algorithm results and
	//execute unloading and loading of each elevator to its corresponding(staying) floor.
	alg.processing();
	elevator.update(std::move(alg.nextStop()), std::move(alg.upOrDown()), floors, stat, time);
	time++;
	std::cout << std::endl;
}