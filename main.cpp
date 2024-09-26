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