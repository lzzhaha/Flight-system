/* 
 * CS61C Summer 2013
 * Name:
 * Login:
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flights.h"
#include "timeHM.h"

struct flightSys {
    // Place the members you think are necessary for the flightSys struct here.
	airport_t* airports;
};

struct airport {
    // Place the members you think are necessary for the airport struct here.
	char* name;
	flight_t* flights;
	airport_t* next; 
};

struct flight {
    // Place the members you think are necessary for the flight struct here.
	airport_t *destination;
	timeHM_t departure_time;
	timeHM_t arrival_time;
	int cost;
	flight_t* next;
};

/*
   This should be called if memory allocation failed.
 */
static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(EXIT_FAILURE);
}


/*
   Creates and initializes a flight system, which stores the flight schedules of several airports.
   Returns a pointer to the system created.
 */
flightSys_t* createSystem() {
    //Create an empty system at first time

	flightSys_t* s = (flightSys_t*)malloc(sizeof(flightSys_t));
	if(s == NULL){
		allocation_failed();
	}
	s->airports = NULL;
	return s;
}


/* Given a destination airport, a departure and arrival time, and a cost, return a pointer to new flight_t. Note that this pointer must be available to use even
   after this function returns. (What does this mean in terms of how this pointer should be instantiated)?
*/

flight_t* createFlight(airport_t* dest, timeHM_t dep, timeHM_t arr, int c) {
	flight_t* flt = (flight_t*)malloc(sizeof(flight_t));	 
   	flt->destination = dest;
	flt->departure_time = dep;
	flt->arrival_time = arr;
	flt->cost = c;
	flt->next = NULL;
	return flt;
}

/*
   Frees all memory associated with this system; that's all memory you dynamically allocated in your code.
 */
void deleteSystem(flightSys_t* s) {
    // Replace this line with your code
	airport_t* airps = s->airports;
	while(airps != NULL){
		flight_t* flt = airps->flights;
		while(flt != NULL){
			flight_t* temp_flt = flt;
			flt = flt->next;
			free(temp_flt);
		}
		airport_t* temp_airp = airps;
		airps = airps->next;
		free(temp_airp->name);
		free(temp_airp);
	}
	free(s);
}


/*
   Adds a airport with the given name to the system. You must copy the string and store it.
   Do not store "name" (the pointer) as the contents it point to may change.
 */
void addAirport(flightSys_t* s, char* name) {
    // Replace this line with your code
	airport_t *air = (airport_t*)malloc(sizeof(airport_t));
	
	if(air == NULL){
		allocation_failed();	
	}
	air->flights = NULL;
	air->name = (char*)calloc(strlen(name)+1,sizeof(char));
	strcpy(air->name,name);
	air->next = NULL;

	//If the current airport list is empty, add it directly
	if(s->airports == NULL){
		s->airports = air;
	}else{
		//Append the airport to the rear of the list
		airport_t *cur = s->airports;
		while(cur->next !=NULL){
			cur = cur->next;
		}
		cur->next = air;
	}
	
}


/*
   Returns a pointer to the airport with the given name.
   If the airport doesn't exist, return NULL.
 */
airport_t* getAirport(flightSys_t* s, char* name) {
	for(airport_t *cur = s->airports; cur!=NULL; cur = cur->next){
		if(strcmp(name,cur->name) == 0){
			return cur;
		}
	}
	return NULL;
}


/*
   Print each airport name in the order they were added through addAirport, one on each line.
   Make sure to end with a new line. You should compare your output with the correct output
   in flights.out to make sure your formatting is correct.
 */
void printAirports(flightSys_t* s){
	if(s->airports == NULL){
		return;
	}else{
		for(airport_t *cur = s->airports; cur!=NULL;cur = cur->next){
			printf("%s\n",cur->name);
		}
	}
}


/*
   Adds a flight to src's schedule, stating a flight will leave to dst at departure time and arrive at arrival time.
 */
void addFlight(airport_t* src, airport_t* dst, timeHM_t* departure, timeHM_t* arrival, int cost) {
	if(!(src && dst && departure && arrival)){
		exit(EXIT_FAILURE);
	}
	
	//New flight to be added
	flight_t *newFlt = createFlight(dst,*departure,*arrival,cost);
	if(src->flights == NULL){
		src->flights = newFlt;
	}else{
		flight_t *cur = src->flights;
		for(;cur->next != NULL;cur = cur->next);
		cur->next = newFlt;
	}		
}


/*
   Prints the schedule of flights of the given airport.

   Prints the airport name on the first line, then prints a schedule entry on each 
   line that follows, with the format: "destination_name departure_time arrival_time $cost_of_flight".

   You should use printTime (look in timeHM.h) to print times, and the order should be the same as 
   the order they were added in through addFlight. Make sure to end with a new line.
   You should compare your output with the correct output in flights.out to make sure your formatting is correct.
*/
void printSchedule(airport_t* s) {
	printf("%s\n", s->name);
	flight_t* cur = s->flights;
	while (cur != NULL){
		printf("%s\t", (cur->destination)->name);
		printTime(&(cur->departure_time));
		printf("\t");
		printTime(&(cur->arrival_time));
		printf("\t$%d\n", cur->cost);

		cur = cur->next;
	}
}


/*
   Given a src and dst airport, and the time now, finds the next flight to take based on the following rules:
   1) Finds the cheapest flight from src to dst that departs after now.
   2) If there are multiple cheapest flights, take the one that arrives the earliest.

   If a flight is found, you should store the flight's departure time, arrival time, and cost in departure, arrival, 
   and cost params and return true. Otherwise, return false. 

   Please use the function isAfter() from time.h when comparing two timeHM_t objects.
 */
bool getNextFlight(airport_t* src, airport_t* dst, timeHM_t* now, timeHM_t* departure, timeHM_t* arrival, int* cost) {
	flight_t* search = src->flights;
	timeHM_t earliest;
	int cheapest;
	bool found = false;

	while (search != NULL && found == false){ //find the first matched flight
		if (!strcmp(search->destination->name,dst->name)){ //check if the destination matches
			if(isAfter(&(search->departure_time), now)){ //check if the time is good
				//update local variables
				earliest = search->arrival_time;
				cheapest = search->cost;
				found = true;
				
				//update return values
				*departure = search->departure_time;
				*arrival = search->arrival_time;
				*cost = search->cost;
			}
		}

		search = search->next;
	}

	while(search != NULL){ //find the most ideal one
		if (!strcmp(search->destination->name,dst->name)){
			if(isAfter(&(search->departure_time), now)){ //check if the time is after now
				if((search->cost) < cheapest){ //check if it's the cheapest
					//update local variables
					earliest = search->arrival_time;
					cheapest = search->cost;
					
				
					//update return values
					*departure = search->departure_time;
					*arrival = search->arrival_time;
					*cost = search->cost;
				}
				else if((search->cost) == cheapest && isAfter(&earliest, &(search->arrival_time))){ 
					//if the price is equal, find the  flight with earliest arrival time
					
					//update local variables
					earliest = search->arrival_time;
					
					//update return values
					*departure = search->departure_time;
					*arrival = search->arrival_time;
				}
			}
		}
		search = search->next;
	}

	return found;
}



/* Given a list of flight_t pointers (flight_list) and a list of destination airport names (airport_name_list), first confirm that it is indeed possible to take these sequences of flights,
   (i.e. be sure that the i+1th flight departs after or at the same time as the ith flight arrives) (HINT: use the isAfter and isEqual functions).
   Then confirm that the list of destination airport names match the actual destination airport names of the provided flight_t struct's.
   sz tells you the number of flights and destination airport names to consider. Be sure to extensively test for errors (i.e. if you encounter NULL's for any values that you might expect to
   be non-NULL, return -1).

   Return from this function the total cost of taking these sequence of flights. If it is impossible to take these sequence of flights, if the list of destination airport names
   doesn't match the actual destination airport names provided in the flight_t struct's, or if you run into any errors mentioned previously or any other errors, return -1.
*/
int validateFlightPath(flight_t** flight_list, char** airport_name_list, int sz) {
	if((!(flight_list && airport_name_list)) ||(sz < 0)){
		return -1;
	}	

	int total_cost = 0;
	timeHM_t pre_dep, pre_arr, cur_dep, cur_arr;
	pre_dep.hours = pre_arr.hours = -1;

	for(int i=0; i<sz; i++){
		if(flight_list[i] == NULL || airport_name_list[i] == NULL){
			return -1;
		}
		cur_dep = flight_list[i]->departure_time;
		cur_arr = flight_list[i]->arrival_time;
		if(isEqual(&cur_dep,&cur_arr)||isAfter(&cur_dep,&cur_arr)){
			return -1;
		}

		if(!((isEqual(&cur_dep,&pre_arr))||(isAfter(&cur_dep,&pre_arr)))){
			return -1;
		}

		pre_arr = cur_arr;
		pre_dep = cur_dep;

		if(strcmp(flight_list[i]->destination->name,airport_name_list[i])){
			return -1;
		}

		total_cost += flight_list[i]->cost;
	}

	return total_cost;
}
