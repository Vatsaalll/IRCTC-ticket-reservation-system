#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <queue>

using namespace std;

struct Train {
    string number;
    map<string, int> stations; // station name and distance
    map<string, int> coaches;  // coach type and seats
};

struct Booking {
    int pnr;
    string trainNumber;
    string from;
    string to;
    string date;
    string coachType;
    int passengers;
    int fare;
    vector<pair<string, int>> seats; // coach and seat number
    bool waitlisted;
    int waitlistNumber;
};

vector<Train> trains;
map<int, Booking> bookings;
map<string, queue<Booking>> waitlists; // key: trainNumber_date_coachType
int nextPNR = 100000001;

// Function to book tickets
void bookTickets(string trainNumber, string from, string to, string date, string coachType, int passengers) {
    // Find the selected train
    for (auto &train : trains) {
        if (train.number == trainNumber) {
            // Ensure both from and to stations exist in the train's route and from station comes before to station
            auto fromIt = train.stations.find(from);
            auto toIt = train.stations.find(to);
            
            if (fromIt != train.stations.end() && toIt != train.stations.end() && fromIt->second < toIt->second) {
                int distance = toIt->second - fromIt->second;  // Calculate the distance
                int farePerKm = 0;
                if (coachType == "SL") farePerKm = 1;
                else if (coachType == "3A") farePerKm = 2;
                else if (coachType == "2A") farePerKm = 3;
                else if (coachType == "1A") farePerKm = 4;

                int totalFare = distance * farePerKm * passengers;

                // Check seat availability and book
                int seatsAvailable = 0;
                for (auto &coach : train.coaches) {
                    if (coach.first.substr(0, 1) == coachType.substr(0, 1)) {
                        seatsAvailable += coach.second;
                    }
                }

                Booking booking;
                booking.pnr = nextPNR++;
                booking.trainNumber = train.number;
                booking.from = from;
                booking.to = to;
                booking.date = date;
                booking.coachType = coachType;
                booking.passengers = passengers;
                booking.fare = totalFare;
                booking.waitlisted = false;
                booking.waitlistNumber = 0;

                if (seatsAvailable >= passengers) {
                    // Allocate seats
                    for (auto &coach : train.coaches) {
                        if (coach.first.substr(0, 1) == coachType.substr(0, 1)) {
                            int seatsToBook = min(passengers, coach.second);
                            for (int i = 1; i <= seatsToBook; ++i) {
                                booking.seats.push_back({coach.first, i});
                            }
                            coach.second -= seatsToBook;
                            passengers -= seatsToBook;
                            if (passengers == 0) break;
                        }
                    }
                    bookings[booking.pnr] = booking;
                    cout << "Booking confirmed! PNR: " << booking.pnr << ", Fare: " << booking.fare << endl;
                } else {
                    // Add to waitlist
                    booking.waitlisted = true;
                    booking.waitlistNumber = waitlists[train.number + "_" + date + "_" + coachType].size() + 1;
                    waitlists[train.number + "_" + date + "_" + coachType].push(booking);
                    bookings[booking.pnr] = booking;
                    cout << "Booking waitlisted! PNR: " << booking.pnr << ", Fare: " << booking.fare << ", WL/" << booking.waitlistNumber << endl;
                }
                return;
            } else {
                cout << "The specified route (from: " << from << " to: " << to << ") does not exist in the selected train." << endl;
                return;
            }
        }
    }
    cout << "No Trains Available for the specified route." << endl;
}

// Function to retrieve a booking using PNR
void retrieveBooking(int pnr) {
    if (bookings.find(pnr) != bookings.end()) {
        Booking booking = bookings[pnr];
        cout << "Train: " << booking.trainNumber << ", From: " << booking.from << ", To: " << booking.to
             << ", Date: " << booking.date << ", Fare: " << booking.fare << ", Seats: ";
        for (auto &seat : booking.seats) {
            cout << seat.first << "-" << seat.second << " ";
        }
        if (booking.waitlisted) {
            cout << "WL/" << booking.waitlistNumber;
        }
        cout << endl;
    } else {
        cout << "Invalid PNR" << endl;
    }
}

// Function to cancel a booking using PNR
void cancelBooking(int pnr) {
    if (bookings.find(pnr) != bookings.end()) {
        Booking booking = bookings[pnr];
        if (booking.waitlisted) {
            // Remove from waitlist
            queue<Booking> &waitlist = waitlists[booking.trainNumber + "_" + booking.date + "_" + booking.coachType];
            queue<Booking> newWaitlist;
            while (!waitlist.empty()) {
                Booking b = waitlist.front();
                waitlist.pop();
                if (b.pnr != pnr) {
                    newWaitlist.push(b);
                }
            }
            waitlists[booking.trainNumber + "_" + booking.date + "_" + booking.coachType] = newWaitlist;
        } else {
            // Free up seats
            for (auto &seat : booking.seats) {
                for (auto &train : trains) {
                    if (train.number == booking.trainNumber) {
                        train.coaches[seat.first]++;
                    }
                }
            }
            // Confirm waitlisted bookings if possible
            queue<Booking> &waitlist = waitlists[booking.trainNumber + "_" + booking.date + "_" + booking.coachType];
            while (!waitlist.empty()) {
                Booking b = waitlist.front();
                waitlist.pop();
                int seatsAvailable = 0;
                for (auto &train : trains) {
                    if (train.number == b.trainNumber) {
                        for (auto &coach : train.coaches) {
                            if (coach.first.substr(0, 1) == b.coachType.substr(0, 1)) {
                                seatsAvailable += coach.second;
                            }
                        }
                    }
                }
                if (seatsAvailable >= b.passengers) {
                    // Allocate seats
                    for (auto &train : trains) {
                        if (train.number == b.trainNumber) {
                            for (auto &coach : train.coaches) {
                                if (coach.first.substr(0, 1) == b.coachType.substr(0, 1)) {
                                    int seatsToBook = min(b.passengers, coach.second);
                                    for (int i = 1; i <= seatsToBook; ++i) {
                                        b.seats.push_back({coach.first, i});
                                    }
                                    coach.second -= seatsToBook;
                                    b.passengers -= seatsToBook;
                                    if (b.passengers == 0) break;
                                }
                            }
                        }
                    }
                    b.waitlisted = false;
                    b.waitlistNumber = 0;
                    bookings[b.pnr] = b;
                    cout << "Booking confirmed for PNR " << b.pnr << endl;
                } else {
                    waitlist.push(b);
                    break;
                }
            }
        }
        bookings.erase(pnr);
        cout << "Booking cancelled for PNR " << pnr << endl;
    } else {
        cout << "Invalid PNR" << endl;
    }
}

// Function to generate a report of all bookings
void generateReport() {
    cout << "PNR, Date, Train, From, To, Fare, Seats" << endl;
    for (auto &entry : bookings) {
        Booking booking = entry.second;
        cout << booking.pnr << ", " << booking.date << ", " << booking.trainNumber << ", " << booking.from
             << ", " << booking.to << ", " << booking.fare << ", ";
        for (auto &seat : booking.seats) {
            cout << seat.first << "-" << seat.second << " ";
        }
        if (booking.waitlisted) {
            cout << "WL/" << booking.waitlistNumber;
        }
        cout << endl;
    }
}

// Function to display all available trains
void displayTrains() {
    cout << "Available Trains: \n";
    for (const auto &train : trains) {
        cout << "Train Number: " << train.number << " - Stations: ";
        for (const auto &station : train.stations) {
            cout << station.first << "(" << station.second << "km) ";
        }
        cout << "- Coaches: ";
        for (const auto &coach : train.coaches) {
            cout << coach.first << "(" << coach.second << " seats) ";
        }
        cout << endl;
    }
}

// Function to handle user actions
void userInterface() {
    int choice;
    do {
        cout << "\n1. Display Trains\n2. Book Ticket\n3. Retrieve Booking\n4. Cancel Booking\n5. Generate Report\n6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                displayTrains();
                break;
            case 2: {
                string trainNumber, from, to, date, coachType;
                int passengers;
                cout << "Enter Train Number: ";
                cin >> trainNumber;
                cout << "Enter From Station: ";
                cin >> from;
                cout << "Enter To Station: ";
                cin >> to;
                cout << "Enter Date (yyyy-mm-dd): ";
                cin >> date;
                cout << "Enter Coach Type (SL/3A/2A/1A): ";
                cin >> coachType;
                cout << "Enter Number of Passengers: ";
                cin >> passengers;
                bookTickets(trainNumber, from, to, date, coachType, passengers);
                break;
            }
            case 3: {
                int pnr;
                cout << "Enter PNR: ";
                cin >> pnr;
                retrieveBooking(pnr);
                break;
            }
            case 4: {
                int pnr;
                cout << "Enter PNR: ";
                cin >> pnr;
                cancelBooking(pnr);
                break;
            }
            case 5:
                generateReport();
                break;
            case 6:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);
}

int main() {
    // Initialize train data
    Train train1 = {"17726", {{"Rajkot", 0}, {"Mumbai", 750}}, {{"S1", 72}, {"S2", 72}, {"B1", 72}, {"A1", 48}, {"H1", 24}}};
    Train train2 = {"37392", {{"Ahmedabad", 0}, {"Surat", 300}}, {{"S1", 15}, {"S2", 20}, {"S3", 50}, {"B1", 36}, {"B2", 48}}};
    Train train3 = {"29772", {{"Vadodara", 0}, {"Indore", 450}}, {{"S1", 60}, {"B1", 36}, {"A1", 48}}};
    Train train4 = {"12933", {{"Delhi", 0}, {"Jaipur", 250}, {"Ahmedabad", 600}}, {{"S1", 72}, {"B1", 50}, {"A1", 24}}};
    Train train5 = {"12016", {{"Mumbai", 0}, {"Pune", 150}}, {{"S1", 72}, {"S2", 60}, {"B1", 48}}};
    Train train6 = {"12628", {{"Chennai", 0}, {"Bangalore", 350}}, {{"S1", 72}, {"S2", 70}, {"A1", 48}}};
    Train train7 = {"19001", {{"Surat", 0}, {"Mumbai", 300}}, {{"S1", 15}, {"S2", 20}, {"B1", 48}}};

    // Add all trains to the list
    trains = {train1, train2, train3, train4, train5, train6, train7};

    // Run the user interface
    userInterface();

    return 0;
}
