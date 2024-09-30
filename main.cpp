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

class Booking {
public:
    int pnr; 
    std::string from; 
    std::string to; 
    std::string date; 
    char coachType; 
    int numPassengers; 
    int fare; 
    std::vector<std::pair<std::string, int>> seats; // Seats booked

    Booking(int pnr, std::string from, std::string to, std::string date, char coachType, int numPassengers, int fare)
        : pnr(pnr), from(from), to(to), date(date), coachType(coachType), numPassengers(numPassengers), fare(fare) {}

    // Get the PNR number
    int getPNR() const {
        return pnr;
    }

    // Get the from station
    std::string getFrom() const {
        return from;
    }

    // Get the to station
    std::string getTo() const {
        return to;
    }

    // Get the date of travel
    std::string getDate() const {
        return date;
    }

    // Get the coach type
    char getCoachType() const {
        return coachType;
    }

    // Get the number of passengers
    int getNumPassengers() const {
        return numPassengers;
    }

    // Get the fare for the booking
    int getFare() const {
        return fare;
    }

    // Get the seats booked
    std::vector<std::pair<std::string, int>> getSeats() const {
        return seats;
    }
};

class BookingSystem {
public:
    std::vector<Train> trains; // Trains in the system
    std::map<int, Booking> bookings; // Bookings in the system
    int pnrCounter; // PNR counter

    BookingSystem(const std::vector<Train>& trains) : trains(trains), pnrCounter(100000000) {}

    // Book tickets for a given route, date, class/coach type, and number of passengers
    std::pair<int, int> bookTickets(const std::string& fromStation, const std::string& toStation, const std::string& date, char coachType, int numPassengers) {
        for (auto& train : trains) {
                        if (train.getRoute() == std::make_pair(fromStation, toStation)) {
                int totalSeats = 0;
                for (const auto& coach : train.getCoaches()) {
                    if (coach.getType() == coachType) {
                        totalSeats += coach.getAvailableSeats();
                    }
                }
                if (totalSeats >= numPassengers) {
                    pnrCounter++;
                    int fare = train.getDistance() * getFareRate(coachType) * numPassengers;
                    updateSeatAvailability(train, coachType, numPassengers);
                    bookings[pnrCounter] = Booking(pnrCounter, fromStation, toStation, date, coachType, numPassengers, fare);
                    return {pnrCounter, fare};
                }
            }
        }
        return {-1, 0}; // No Seats Available
    }

    // Get booking details using PNR
    Booking getBookingDetails(int pnr) {
        if (bookings.find(pnr) != bookings.end()) {
            return bookings[pnr];
        }
        throw std::invalid_argument("Invalid PNR");
    }

    // Generate a report of all bookings
    void generateReport() {
        for (const auto& booking : bookings) {
            std::cout << "PNR: " << booking.second.getPNR() << ", Date: " << booking.second.getDate() << ", Train: " << booking.second.getFrom() << " to " << booking.second.getTo() << ", Fare: " << booking.second.getFare() << "\n";
        }
    }

    // Handle waitlist and cancellations
    void handleWaitlistAndCancellations() {
        // Implement logic to handle waitlist and cancellations
    }

private:
    // Get the fare rate for a given coach type
    int getFareRate(char coachType) {
        std::map<char, int> rates = {{'S', 1}, {'B', 2}, {'A', 3}, {'H', 4}};
        return rates[coachType];
    }

    // Update the seat availability for a given train and coach type
    void updateSeatAvailability(Train& train, char coachType, int numPassengers) {
        for (auto& coach : train.getCoaches()) {
            if (coach.getType() == coachType) {
                if (coach.getAvailableSeats() >= numPassengers) {
                    coach.updateAvailableSeats(numPassengers);
                    break;
                } else {
                    numPassengers -= coach.getAvailableSeats();
                    coach.updateAvailableSeats(coach.getAvailableSeats());
                }
            }
        }
    }
};

int main() {
    // Create trains
    std::vector<Coach> coaches1 = {Coach('S', 72), Coach('B', 72), Coach('A', 48), Coach('H', 24)};
    Train train1("17726", std::make_pair("Rajkot", "Mumbai"), 750, coaches1);

    std::vector<Coach> coaches2 = {Coach('S', 15), Coach('B', 20), Coach('A', 36), Coach('H', 48)};
    Train train2("37392", std::make_pair("Ahmedabad", "Surat"), 300, coaches2);

    std::vector<Train> trains = {train1, train2};

    // Create booking system
    BookingSystem bookingSystem(trains);

    // Book tickets
    auto result = bookingSystem.bookTickets("Rajkot", "Mumbai", "2023-03-15", 'S', 6);
    if (result.first != -1) {
        std::cout << "PNR: " << result.first << " Fare: " << result.second << "\n";
    } else {
        std::cout << "No Seats Available\n";
    }

    // Generate report
    bookingSystem.generateReport();

    return 0;
}