class Coach {
public:
    char type; // Coach type (S, B, A, H)
    int seats; // Number of seats in the coach
    int availableSeats; // Number of available seats in the coach

    Coach(char type, int seats) : type(type), seats(seats), availableSeats(seats) {}

    // Get the coach type
    char getType() const {
        return type;
    }

    // Get the number of seats in the coach
    int getSeats() const {
        return seats;
    }

    // Get the number of available seats in the coach
    int getAvailableSeats() const {
        return availableSeats;
    }

    // Update the number of available seats in the coach
    void updateAvailableSeats(int numSeats) {
        availableSeats -= numSeats;
    }
};