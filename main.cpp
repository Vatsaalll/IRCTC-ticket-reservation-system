#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>

class Coach {
public:
    char type; 
    int seats; 
    int availableSeats; 

    Coach(char type, int seats) : type(type), seats(seats), availableSeats(seats) {}

    
    char getType() const {
        return type;
    }

    
    int getSeats() const {
        return seats;
    }

    
    int getAvailableSeats() const {
        return availableSeats;
    }

    
    void updateAvailableSeats(int numSeats) {
        availableSeats -= numSeats;
    }
};

class Train {
public:
    std::string number; // Train number
    std::pair<std::string, std::string> route; // Train route (from, to)
    int distance; // Distance of the train route
    std::vector<Coach> coaches; // Coaches in the train

    Train(std::string number, std::pair<std::string, std::string> route, int distance, std::vector<Coach> coaches)
        : number(number), route(route), distance(distance), coaches(coaches) {}

    // Get the train number
    std::string getNumber() const {
        return number;
    }

    // Get the train route
    std::pair<std::string, std::string> getRoute() const {
        return route;
    }

    // Get the distance of the train route
    int getDistance() const {
        return distance;
    }

    // Get the coaches in the train
    std::vector<Coach> getCoaches() const {
        return coaches;
    }
};