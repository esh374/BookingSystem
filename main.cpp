 #include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream> // Added for string stream #include <unordered_map>
#include <vector>
// Structure to represent a booking struct Booking {
std::string room; std::string time;
// Define the equality operator for Booking objects bool operator==(const Booking &other) const {
return room == other.room && time == other.time; }
};
// Function to display available rooms
void displayAvailableRooms(const std::vector<std::string> &rooms) {
std::cout << "Available Rooms:\n"; for (const auto &room : rooms) {
std::cout << room << "\n"; }
}
// Function to display available timings for a given room
void displayAvailableTimings(const std::vector<std::string> &timings) {
std::cout << "Available Timings:\n"; for (const auto &timing : timings) {
std::cout << timing << "\n"; }
}
// Function to display available rooms for a specific timing void displayAvailableRoomsForTiming(
const std::vector<std::string> &rooms,
const std::unordered_map<std::string, std::vector<std::string>>
&roomTimings,
const std::string &timing) {
std::cout << "Available Rooms for Timing " << timing << ":\n"; for (const auto &room : rooms) {

 if (std::find(roomTimings.at(room).begin(), roomTimings.at(room).end(), timing) != roomTimings.at(room).end()) {
std::cout << room << "\n"; }
} }
// Function to display booked rooms and timings
void displayBookings(const std::vector<Booking> &bookings) {
std::cout << "Booked Rooms and Timings:\n"; for (const auto &booking : bookings) {
std::cout << "Room: " << booking.room << ", Timing: " << booking.time << "\n";
} }
// Function to append a booking to a CSV file
void appendBookingToCSV(const Booking &booking, const std::string &userEmail) {
std::ofstream outFile("bookings.csv", std::ios::app); // Open file in append mode if (outFile.is_open()) {
// Append the new booking to the CSV file
outFile << userEmail << "," << booking.room << "," << booking.time << "\n"; outFile.close();
} else {
std::cerr << "Error opening the CSV file for writing.\n";
} }
// Function to append bookings to a CSV file
void appendBookingsToCSV(const std::vector<Booking> &bookings, const std::string &userEmail) {
for (const auto &booking : bookings) { appendBookingToCSV(booking, userEmail);
} }
// Function to read bookings from a CSV file and update the provided vector
void readBookingsFromCSV(std::vector<Booking> &bookings, const std::string &userEmail) {
bookings.clear(); // Clear the existing vector before updating it
std::ifstream inFile("bookings.csv"); if (inFile.is_open()) {
// Skip header line std::string header;

 std::getline(inFile, header);
// Read bookings from the CSV file std::string line;
while (std::getline(inFile, line)) {
std::istringstream iss(line);
std::string email, room, time; std::getline(std::getline(iss, email, ','), room, ','); std::getline(iss, time, ',');
if (email == userEmail) { bookings.push_back({room, time});
} }
inFile.close(); } else {
std::cout << "CSV file not found. Creating a new one.\n";
// You can choose to create a new CSV file here if needed. }
}
// Function to perform room booking void bookRoom(
std::vector<std::string> &rooms,
std::unordered_map<std::string, std::vector<std::string>> &roomTimings, std::vector<Booking> &bookings, bool byRoom, const std::string &userType, const std::string &userEmail) {
// Read existing bookings from CSV file readBookingsFromCSV(bookings, userEmail);
std::string selectedRoom, selectedTiming;
if (byRoom) {
// Display available rooms displayAvailableRooms(rooms);
// User selects a room
std::cout << "Select a room: "; std::getline(std::cin >> std::ws, selectedRoom);
// Check if the selected room is valid
if (std::find(rooms.begin(), rooms.end(), selectedRoom) == rooms.end()) {
std::cout << "Invalid room. Please try again.\n";

 return; }
// Display available timings for the selected room displayAvailableTimings(roomTimings[selectedRoom]);
// User selects a timing
std::cout << "Select a timing (e.g., 9:00 AM): "; std::getline(std::cin >> std::ws, selectedTiming);
} else {
// Display available timings displayAvailableTimings(
roomTimings.begin()
->second); // Display timings for the first room (assuming timings
// are the same for all rooms)
// User selects a timing
std::cout << "Select a timing (e.g., 9:00 AM): "; std::getline(std::cin >> std::ws, selectedTiming);
// Display available rooms for the selected timing displayAvailableRoomsForTiming(rooms, roomTimings, selectedTiming);
// User selects a room
std::cout << "Select a room: "; std::getline(std::cin >> std::ws, selectedRoom);
}
// Check if the selected room or timing is valid if ((byRoom &&
std::find(rooms.begin(), rooms.end(), selectedRoom) == rooms.end()) || (!byRoom &&
std::find(roomTimings.begin()->second.begin(), roomTimings.begin()->second.end(),
selectedTiming) == roomTimings.begin()->second.end())) {
std::cout << "Invalid " << (byRoom ? "room" : "timing") << ". Please try again.\n";
return; }
// Add the booking to the list bookings.push_back({selectedRoom, selectedTiming});
// Remove the booked timing from available timings for the selected room

 auto &timings = roomTimings[selectedRoom]; timings.erase(std::remove(timings.begin(), timings.end(), selectedTiming),
timings.end());
// Check if the room is booked for all timings, remove it from available rooms // if true
if (std::all_of(timings.begin(), timings.end(),
[&](const std::string &timing) {
return std::find_if(bookings.begin(), bookings.end(),
[&](const Booking &booking) {
return booking.room == selectedRoom &&
booking.time == timing; }) != bookings.end();
})) {
rooms.erase(std::remove(rooms.begin(), rooms.end(), selectedRoom),
rooms.end());
}
// Inform the user about the booking
std::cout << "Booking confirmed for Room: " << selectedRoom
<< " at Timing: " << selectedTiming << "\n";
// Save the new booking to CSV
appendBookingsToCSV(bookings, userEmail); }
// Function to cancel a booking // Function to cancel a booking void cancelBooking(
std::vector<std::string> &rooms,
std::unordered_map<std::string, std::vector<std::string>> &roomTimings, std::vector<Booking> &bookings, const std::string &userEmail) {
// Read existing bookings from CSV file readBookingsFromCSV(bookings, userEmail);
if (bookings.empty()) {
std::cout << "No bookings to cancel.\n"; return;
}
// Display booked rooms and timings displayBookings(bookings);

 // User selects a booking to cancel
std::string selectedRoom, selectedTiming;
std::cout << "Enter the room and timing for the booking to cancel (e.g., "
"Room1 9:00AM): ";
// Read the input with the modified format std::cin >> selectedRoom >> selectedTiming;
// Check if the booking exists
auto it = std::find(bookings.begin(), bookings.end(),
Booking{selectedRoom, selectedTiming}); if (it != bookings.end()) {
// Add the canceled timing back to available timings for the selected room roomTimings[selectedRoom].push_back(selectedTiming);
// Add the canceled room back to available rooms if it was booked for all timings if (std::all_of(
roomTimings[selectedRoom].begin(), roomTimings[selectedRoom].end(), [&](const std::string &timing) {
return std::find_if(bookings.begin(), bookings.end(), [&](const Booking &booking) {
return booking.room == selectedRoom && booking.time == timing;
}) != bookings.end();
})) { rooms.push_back(selectedRoom);
}
// Remove the canceled booking from the list bookings.erase(it);
// Inform the user about the cancellation
std::cout << "Booking canceled for Room: " << selectedRoom
<< " at Timing: " << selectedTiming << "\n";
// Update CSV file after cancellation
appendBookingsToCSV(bookings, userEmail); } else {
std::cout << "Booking not found.\n"; }
}
void resetBookings(

 std::vector<std::string> &rooms,
std::unordered_map<std::string, std::vector<std::string>> &roomTimings, std::vector<Booking> &bookings, const std::string &userEmail, int resetOption) {
// Read existing bookings from CSV file readBookingsFromCSV(bookings, userEmail);
if (bookings.empty()) {
std::cout << "No bookings to reset.\n"; return;
}
if (resetOption == 5) {
// Admin password for resetting bookings
std::string adminPassword;
std::cout << "Enter admin password to reset bookings: "; std::cin >> adminPassword;
// Check if the admin password is correct
if (adminPassword != "radhikaanannya123") {
std::cout << "Incorrect admin password. Unable to reset bookings.\n";
return; }
// Add all booked timings back to available timings for each room for (const auto &booking : bookings) {
roomTimings[booking.room].push_back(booking.time); }
// Clear the bookings list bookings.clear();
// Reset available rooms based on the initial state of room timings rooms.clear();
for (const auto &roomTimingPair : roomTimings) {
rooms.push_back(roomTimingPair.first); }
std::cout << "All bookings have been reset.\n"; } else {
std::cout << "Invalid reset option. Please try again.\n"; }

 // Update CSV file after resetting bookings, but only if option 5 is chosen if (resetOption == 5) {
appendBookingsToCSV(bookings, userEmail); }
}
// Function to reset all bookings void resetBookings(
std::vector<std::string> &rooms,
std::unordered_map<std::string, std::vector<std::string>> &roomTimings, std::vector<Booking> &bookings, const std::string &userEmail) {
// Read existing bookings from CSV file readBookingsFromCSV(bookings, userEmail);
if (bookings.empty()) {
std::cout << "No bookings to reset.\n"; return;
}
// Admin password for resetting bookings
std::string adminPassword;
std::cout << "Enter admin password to reset bookings: "; std::cin >> adminPassword;
// Check if the admin password is correct
if (adminPassword != "radhikaanannya123") {
std::cout << "Incorrect admin password. Unable to reset bookings.\n";
return; }
// Add all booked timings back to available timings for each room for (const auto &booking : bookings) {
roomTimings[booking.room].push_back(booking.time); }
// Clear the bookings list bookings.clear();
// Reset available rooms based on the initial state of room timings rooms.clear();
for (const auto &roomTimingPair : roomTimings) {
rooms.push_back(roomTimingPair.first);

 }
std::cout << "All bookings have been reset.\n";
// Update CSV file after resetting bookings
appendBookingsToCSV(bookings, userEmail); }
// Function to validate email format and determine user type
bool isValidEmail(const std::string &email, std::string &userType) {
// Use a regex pattern to validate the email format
std::regex studentPattern("^[a-zA-Z]+\\.[a-zA-Z]+@flame\\.edu\\.in$"); std::regex adminPattern("^[a-zA-Z]+\\.[a-zA-Z]+@admin\\.flame\\.edu\\.in$");
if (std::regex_match(email, studentPattern)) { userType = "Student";
return true;
} else if (std::regex_match(email, adminPattern)) { userType = "Admin";
return true;
} else {
userType = "Invalid"; return false;
} }
// ... (your existing code)
int main() {
std::cout << "If the user is a student, the mail domain should be 'flame.edu.in'.\n"; std::cout << "If the user is an admin, the mail domain should be 'admin.flame.edu.in'.\n"; std::cout << std::endl;
// Ask for the user's email ID std::string userEmail, userType; std::cout << "Enter your email ID: "; std::cin >> userEmail;
// Validate the email format and determine the user type if (!isValidEmail(userEmail, userType)) {
std::cout << "Invalid email.\n";
return 0; }
// Welcome message based on the user type

 if (userType == "Student") { std::cout << "Welcome Student ";
} else if (userType == "Admin") { std::cout << "Welcome Admin ";
}
// Display the user's first name
std::string firstName = userEmail.substr(0, userEmail.find('.')); std::cout << firstName << "\n";
// Initialize other variables
std::vector<std::string> availableRooms = {"Room1", "Room2", "Room3"}; std::unordered_map<std::string, std::vector<std::string>> roomTimings = {
{"Room1", {"9:00 AM", "2:00 PM", "6:00 PM"}}, {"Room2", {"9:00 AM", "2:00 PM", "6:00 PM"}}, {"Room3", {"9:00 AM", "2:00 PM", "6:00 PM"}},
};
std::vector<Booking> bookings;
readBookingsFromCSV(bookings, userEmail);
int option; do {
// Display the menu based on the user type if (userType == "Admin") {
std::cout << "\n1. Book by Room\n2. Book by Timing\n3. Cancel " "Booking\n4. Exit\n5. Reset Bookings\n";
} else {
std::cout << "\n1. Book by Room\n2. Book by Timing\n3. Cancel "
"Booking\n4. Exit\n";
}
std::cout << "Select an option: "; std::cin >> option;
switch (option) { case 1:
bookRoom(availableRooms, roomTimings, bookings, true, userType, userEmail);
break; case 2:
bookRoom(availableRooms, roomTimings, bookings, false, userType, userEmail);
break; case 3:
cancelBooking(availableRooms, roomTimings, bookings, userEmail);

 break; case 4:
std::cout << "Exiting the program.\n";
break; case 5:
if (userType == "Admin") {
resetBookings(availableRooms, roomTimings, bookings, userEmail, option); // After resetting bookings, update the 'bookings' vector readBookingsFromCSV(bookings, userEmail);
} else {
std::cout << "Invalid option. Please try again.\n";
}
break; default:
std::cout << "Invalid option. Please try again.\n";
break; }
} while (option != 4);
return 0; }
