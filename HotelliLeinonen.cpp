// Jere Leinonen, 3p

#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <iomanip>  // Lis‰tty, jotta voidaan k‰ytt‰‰ setw ja setprecision
#include <string>

using namespace std;

// Rakenteet
struct Reservation {
    int reservationNumber;
    string customerName;
    int roomNumber;
    int nights;
};

// Aliohjelmat
int GenerateRandomRoomCount(mt19937& gen);
int GenerateRandomPrice(mt19937& gen);
int GenerateReservationNumber(mt19937& gen);
void PrintRoomList(const vector<int>& roomsAvailable);
int GetValidRoomTypeAndNumber(const vector<int>& roomsAvailable, int roomType);
bool IsValidRoomNumber(int roomNumber, const vector<int>& roomsAvailable);
void ReserveRoom(int roomNumber, vector<int>& roomsAvailable, int pricePerNight, int nights);
vector<double> Discount(double price);
int ErrorChecker(int answer);
void GetCustomerName(string& name);
void SearchReservationByNumber(int reservationNumber, const vector<Reservation>& reservations);
void SearchReservationByName(const string& customerName, const vector<Reservation>& reservations);

// P‰‰ohjelma
int main() {
    random_device rd;
    mt19937 gen(rd());  // Luodaan satunnaislukugeneraattori

    // Arvotaan huoneiden m‰‰r‰ ja hinta
    int totalRooms = GenerateRandomRoomCount(gen);
    int pricePerNight = GenerateRandomPrice(gen);

    // Alustetaan huoneiden saatavuus (1 = vapaa, 0 = varattu)
    vector<int> roomsAvailable(totalRooms, 1);

    vector<Reservation> reservations;  // Varausten tallentamiseen

    cout << "Hotellissa on " << totalRooms << " huonetta, ja hinta on " << pricePerNight << " Ä / yˆ." << endl;

    bool continueBooking = true;
    while (continueBooking) {
        cout << "\nValitse toiminto:" << endl;
        cout << "1. Varaa huone" << endl;
        cout << "2. Hae varaus varausnumerolla" << endl;
        cout << "3. Hae varaus varaajan nimell‰" << endl;
        cout << "4. Poistu" << endl;

        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            string name;
            GetCustomerName(name);  // Kysyt‰‰n varaajan nimi
            int roomType;
            cout << "Valitse huonetyyppi: [1] Yhden hengen huone, [2] Kahden hengen huone: ";
            while (cin.fail() || !(cin >> roomType) || (roomType != 1 && roomType != 2)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Virheellinen valinta. Valitse 1 tai 2: ";
            }

            int roomNumber = GetValidRoomTypeAndNumber(roomsAvailable, roomType);  // Kysyt‰‰n huonenumero
            int nights;
            cout << "Kuinka monta yˆt‰ (1-31)? ";
            while (cin.fail() || !(cin >> nights) || nights < 1 || nights > 31) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Anna kelvollinen luku (1-31): ";
            }

            int reservationNumber = GenerateReservationNumber(gen);  // Arvotaan varausnumero

            // Luo uusi varaus ja lis‰‰ se varauksiin
            Reservation newReservation = { reservationNumber, name, roomNumber, nights };
            reservations.push_back(newReservation);
            ReserveRoom(roomNumber, roomsAvailable, pricePerNight, nights);  // Varaa huone
            cout << "Varaus onnistui! Varausnumero: " << reservationNumber << endl;
            break;
        }
        case 2: {
            int reservationNumber;
            cout << "Anna varausnumero: ";
            cin >> reservationNumber;
            SearchReservationByNumber(reservationNumber, reservations);  // Hae varausnumerolla
            break;
        }
        case 3: {
            string customerName;
            cout << "Anna varaajan nimi: ";
            cin.ignore();  // Poistetaan mahdollinen j‰‰nnˆs syˆtteest‰
            getline(cin, customerName);
            SearchReservationByName(customerName, reservations);  // Hae varaajan nimell‰
            break;
        }
        case 4: {
            continueBooking = false;
            cout << "Kiitos! Ohjelma suljetaan." << endl;
            break;
        }
        default:
            cout << "Virheellinen valinta. Yrit‰ uudelleen." << endl;
            break;
        }
    }

    return 0;
}

// Funktio generoi satunnaisen huoneiden m‰‰r‰n v‰lill‰ 30-70
int GenerateRandomRoomCount(mt19937& gen) {
    uniform_int_distribution<> dis(40, 300);  // Huoneiden m‰‰r‰ v‰lill‰ 40-300
    return dis(gen);  // Arpoo huoneiden m‰‰r‰n
}

// Funktio generoi satunnaisen huonehinnan v‰lill‰ 80-100 euroa
int GenerateRandomPrice(mt19937& gen) {
    uniform_int_distribution<> dis(80, 100);  // Huonehinta v‰lill‰ 80-100 Ä
    return dis(gen);  // Arpoo huonehinnan
}

// Funktio generoi satunnaisen varausnumeron
int GenerateReservationNumber(mt19937& gen) {
    uniform_int_distribution<> dis(10000, 99999);  // Varausnumero v‰lill‰ 10000-99999
    return dis(gen);  // Arvotaan varausnumeron
}

// Tulostetaan huoneet ja niiden saatavuus
void PrintRoomList(const vector<int>& roomsAvailable) {
    cout << "\nVapaat huoneet: ";
    for (int i = 0; i < roomsAvailable.size(); ++i) {
        if (roomsAvailable[i] == 1) {  // Tarkistetaan, onko huone vapaa
            cout << setw(3) << i + 1 << " ";  // Huoneet numeroidaan alkaen 1:st‰
        }
    }
    cout << endl;
}

// Tarkistaa onko huonenumero validi (vapaan huoneen numero)
bool IsValidRoomNumber(int roomNumber, const vector<int>& roomsAvailable) {
    return roomNumber >= 1 && roomNumber <= roomsAvailable.size() && roomsAvailable[roomNumber - 1] == 1;
}

// Pyydet‰‰n ja validoidaan huonenumero
int GetValidRoomTypeAndNumber(const vector<int>& roomsAvailable, int roomType) {
    int roomNumber;
    int roomTypeCount = 0;
    for (int i = 0; i < roomsAvailable.size(); ++i) {
        if (roomType == 1 && i < roomsAvailable.size() / 2) {  // Yhden hengen huoneet
            roomTypeCount++;
        }
        else if (roomType == 2 && i >= roomsAvailable.size() / 2) {  // Kahden hengen huoneet
            roomTypeCount++;
        }
    }

    cout << "Valitse huone numero (" << roomTypeCount << " vaihtoehtoa): ";
    while (cin.fail() || !(cin >> roomNumber) || !IsValidRoomNumber(roomNumber, roomsAvailable)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Virheellinen huonenumero. Valitse vapaa huone: ";
    }
    return roomNumber;
}

// Varausfunktio
void ReserveRoom(int roomNumber, vector<int>& roomsAvailable, int pricePerNight, int nights) {
    roomsAvailable[roomNumber - 1] = 0;  // Merkit‰‰n huone varatuksi
    int totalCost = pricePerNight * nights;

    // Sovelletaan alennusta
    vector<double> costWithDiscount = Discount(totalCost);

    cout << "\nVaraus onnistui! Huone numero " << roomNumber << " on varattu " << nights << " yˆksi." << endl;
    cout << "Alennus: " << fixed << setprecision(1) << costWithDiscount[1] << "%" << endl;
    cout << "Kokonaiskustannus: " << fixed << setprecision(2) << costWithDiscount[0] << " Ä." << endl;
}

// Alennusfunktio
vector<double> Discount(double price) {
    // Arvotaan alennus 0%, 10% tai 20%
    double randomnumber = (rand() % 3) / 10.0;
    double discount = 1 - randomnumber;
    double discountPercentage = randomnumber * 100;
    return { price * discount, discountPercentage };
}

// Tarkistetaan syˆtteen oikeellisuus (vain 1 tai 2)
int ErrorChecker(int answer) {
    while (cin.fail() || (answer != 1 && answer != 2)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Vastaa 1 tai 2: ";
        cin >> answer;
    }
    return answer;
}

// Varaajan nimen kysyminen
void GetCustomerName(string& name) {
    cout << "Anna varaajan nimi: ";
    cin.ignore();  // Poistaa mahdollisen j‰‰nnˆsmerkin edellisest‰ syˆtteest‰
    getline(cin, name);  // Lukee koko nimen
}

// Varausnumerolla varauksen haku
void SearchReservationByNumber(int reservationNumber, const vector<Reservation>& reservations) {
    for (const auto& res : reservations) {
        if (res.reservationNumber == reservationNumber) {
            cout << "Varaus lˆytyi: " << endl;
            cout << "Nimi: " << res.customerName << ", Huone: " << res.roomNumber << ", Yˆt: " << res.nights << endl;
            return;
        }
    }
    cout << "Varauksen numeroa ei lˆytynyt." << endl;
}

// Varaajan nimell‰ varauksen haku
void SearchReservationByName(const string& customerName, const vector<Reservation>& reservations) {
    for (const auto& res : reservations) {
        if (res.customerName == customerName) {
            cout << "Varaus lˆytyi: " << endl;
            cout << "Nimi: " << res.customerName << ", Huone: " << res.roomNumber << ", Yˆt: " << res.nights << endl;
            return;
        }
    }
    cout << "Varauksen nime‰ ei lˆytynyt." << endl;
}
