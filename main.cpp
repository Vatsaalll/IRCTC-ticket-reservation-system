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

void bookTickets(string from, string to, string date, string coachType, int passengers) {
    // Find the train for the given route
    for (auto &train : trains) {
        if (train.stations.find(from) != train.stations.end() && train.stations.find(to) != train.stations.end()) {
            int distance = train.stations[to] - train.stations[from];
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
                cout << booking.pnr << " " << booking.fare << endl;
            } else {
                // Add to waitlist
                booking.waitlisted = true;
                booking.waitlistNumber = waitlists[train.number + "_" + date + "_" + coachType].size() + 1;
                waitlists[train.number + "_" + date + "_" + coachType].push(booking);
                bookings[booking.pnr] = booking;
                cout << booking.pnr << " " << booking.fare << " WL/" << booking.waitlistNumber << endl;
            }
            return;
        }
    }
    cout << "No Trains Available" << endl;
}

void retrieveBooking(int pnr) {
    if (bookings.find(pnr) != bookings.end()) {
        Booking booking = bookings[pnr];
        cout << booking.trainNumber << " " << booking.from << " " << booking.to << " " << booking.date << " " << booking.fare << " ";
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

void generateReport() {
    cout << "PNR, DATE, TRAIN, FROM, TO, FARE, SEATS" << endl;
    for (auto &entry : bookings) {
        Booking booking = entry.second;
        cout << booking.pnr << ", " << booking.date << ", " << booking.trainNumber << ", " << booking.from << ", " << booking.to << ", " << booking.fare << ", ";
        for (auto &seat : booking.seats) {
            cout << seat.first << "-" << seat.second << " ";
        }
        if (booking.waitlisted) {
            cout << "WL/" << booking.waitlistNumber;
        }
        cout << endl;
    }
}

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

int main() {
    // Example data
    Train train1 = {"17726", {{"Rajkot", 0}, {"Mumbai", 750}}, {{"S1", 72}, {"S2", 72}, {"B1", 72}, {"A1", 48}, {"H1", 24}}};
    Train train2 = {"37392", {{"Ahmedabad", 0}, {"Surat", 300}}, {{"S1", 15}, {"S2", 20}, {"S3", 50}, {"B1", 36}, {"B2", 48}}};
    trains.push_back(train1);
    trains.push_back(train2);

    // Example bookings
    bookTickets("Rajkot", "Mumbai", "2023-03-15", "SL", 6);
    bookTickets("Rajkot", "Mumbai", "2023-03-15", "1A", 24);
    bookTickets("Rajkot", "Mumbai", "2023-03-15", "1A", 1);
    bookTickets("Rajkot", "Mumbai", "2023-03-16", "1A", 10);
    bookTickets("Rajkot", "Chennai", "2023-03-16", "1A", 10);

    // Retrieve booking
    retrieveBooking(100000001);

    // Generate report
    generateReport();

    // Cancel booking
    cancelBooking(100000001);

    // Generate report after cancellation
    generateReport();

    return 0;
}
