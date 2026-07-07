#include <iostream>
#include <cmath>
#include <windows.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <iomanip> // For table formatting
#include <algorithm> // For transform
using namespace std;

// --- Colors for UI ---
const string RESET = "\033[0m";
const string RED = "\033[0;31m";
const string GREEN = "\033[0;32m";
const string YELLOW = "\033[0;33m";
const string BLUE = "\033[0;34m";
const string MAGENTA = "\033[0;35m";
const string CYAN = "\033[0;36m";
const string WHITE = "\033[0;37m";

const string BRIGHT_RED = "\033[1;31m";
const string BRIGHT_GREEN = "\033[1;32m";
const string BRIGHT_YELLOW = "\033[1;33m";
const string BRIGHT_BLUE = "\033[1;34m";
const string BRIGHT_MAGENTA = "\033[1;35m";
const string BRIGHT_CYAN = "\033[1;36m";
const string BRIGHT_WHITE = "\033[1;37m";

// --- Helper Functions ---
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// --- Authentication Class ---
class Authentication
{
protected:
    static string username;
    static string password;

public:
    // Simple loading animation
    void loadingBar()
    {
        cout << "\n  Loading";
        for (int i = 0; i < 6; i++)
        {
            cout << ".";
            Sleep(200); // Wait 200ms
        }
        cout << endl;
        system("cls"); // Clear screen
    }

    // Generate a random 4-digit OTP
    int generateOTP() {
        srand(time(0));
        return 1000 + rand() % 9000;
    }

    // Check if credentials exist
    bool credentialsExist() {
        ifstream file("admin.txt");
        return file.good();
    }

    // Save credentials to file
    void saveCredentials(string u, string p) {
        ofstream file("admin.txt");
        if (file.is_open()) {
            file << u << endl;
            file << p << endl;
            file.close();
        }
    }

    // Load credentials from file
    void loadCredentials() {
        ifstream file("admin.txt");
        if (file.is_open()) {
            getline(file, username);
            getline(file, password);
            file.close();
        }
    }

    // Display the welcome screen and handle Login/Signup choice
    bool welcomeScreen()
    {
        while (true) {
            cout << "\n\n\n";
            cout << BRIGHT_BLUE << "  ==============================" << RESET << endl;
            cout << BRIGHT_BLUE << "  | EMPLOYEE MANAGEMENT SYSTEM |" << RESET << endl;
            cout << BRIGHT_BLUE << "  ==============================" << RESET << endl << endl;
            cout << BRIGHT_CYAN << "  [1] LOGIN" << RESET << endl;
            cout << BRIGHT_CYAN << "  [2] SIGN UP (New Admin)" << RESET << endl;
            cout << BRIGHT_RED  << "  [0] EXIT" << RESET << endl;
            
            int choice;
            cout << "\n  Enter Choice: ";
            cin >> choice;
            cin.ignore(); // Clear buffer

            if (choice == 1) {
                if (credentialsExist()) {
                    loadCredentials();
                    return login();
                } else {
                    cout << BRIGHT_RED << "\n\t No admin account found! Please Sign Up first." << RESET << endl;
                    Sleep(2000);
                }
            }
            else if (choice == 2) {
                signUp();
                return login();
            }
            else if (choice == 0) {
                return false; // Exit
            }
            else {
                cout << BRIGHT_RED << "\n\t Invalid Choice!" << RESET << endl;
                Sleep(1000);
            }
        }
    }

    void signUp() {
        system("cls");
        cout << "\n\n\n";
        cout << BRIGHT_BLUE << "  ==============================" << RESET << endl;
        cout << BRIGHT_BLUE << "  | EMPLOYEE MANAGEMENT SYSTEM |" << RESET << endl;
        cout << BRIGHT_BLUE << "  ==============================" << RESET << endl << endl;
        cout << BRIGHT_MAGENTA << "  SIGN UP " << RESET << endl << endl;
        
        cout << "  CREATE USERNAME : ";
        getline(cin, username);
        cout << "  CREATE PASSWORD : ";
        getline(cin, password);
        
        saveCredentials(username, password);

        cout << BRIGHT_GREEN << "\n  Account Created Successfully! Please Wait..." << RESET << endl;
        loadingBar();
    }

    // Login Logic
    bool login()
    {
        string u1, p1;
        int enteredOTP, generatedOTP;

        while (true) // Loop until correct login or user exits (handled by logic)
        {
            system("cls");
            cout << "\n\n\n";
            cout << BRIGHT_CYAN << "  ==============================" << RESET << endl;
            cout << BRIGHT_MAGENTA << "  |         LOGIN PAGE         |" << RESET << endl;
            cout << BRIGHT_CYAN << "  ==============================" << RESET << endl << endl;

            cout << "  USER NAME : ";
            getline(cin, u1);
            cout << "  PASSWORD  : ";
            getline(cin, p1);

            if (u1 == username && p1 == password)
            {
                // 1. Generate and Save OTP
                generatedOTP = generateOTP();
                ofstream otpFile("otp.txt");
                if (otpFile.is_open()) {
                    otpFile << generatedOTP;
                    otpFile.close();
                    cout << BRIGHT_YELLOW << "\n\t [INFO] OTP sent to 'otp.txt'. Check the file!" << RESET << endl;
                } else {
                    cout << BRIGHT_RED << "\n\t [ERROR] Could not create otp.txt" << RESET << endl;
                }

                // 2. Verify OTP
                cout << "\t ENTER OTP : ";
                cin >> enteredOTP;
                cin.ignore(); // Clear buffer

                if (enteredOTP == generatedOTP) {
                    cout << BRIGHT_GREEN << "\n\t Login Successful! Welcome." << RESET << endl;
                    Sleep(1000);
                    return true; // Success
                } else {
                    cout << BRIGHT_RED << "\n\t Invalid OTP!" << RESET << endl;
                    Sleep(1500);
                }
            }
            else
            {
                cout << BRIGHT_RED << "\n\t Invalid Username or Password!" << RESET << endl;
                Sleep(1500);
            }
        }
    }
};

// Initialize static members
string Authentication::username = "";
string Authentication::password = "";

// --- Employee Node Structure (Doubly Linked List) ---
class EmployeeNode
{
public:
    EmployeeNode *next;
    EmployeeNode *prev;
    
    // Data fields
    string firstname, lastname, cnic, department, contact;
    int empid;
    int salary;
};

// --- Employee Management Class ---
class EmployeeList
{
    EmployeeNode *head;
    EmployeeNode *tail;

public:
    int totalEmployees;

    EmployeeList()
    {
        head = nullptr;
        tail = nullptr;
        totalEmployees = 0;
        loadFromFile(); // Load data on startup
    }

    // Save all data to file
    void saveToFile() {
        ofstream outFile("employees.txt");
        if (!outFile) {
            cout << BRIGHT_RED << "[ERROR] Could not save to file!" << RESET << endl;
            return;
        }

        EmployeeNode *temp = head;
        while (temp != nullptr) {
            outFile << temp->empid << endl;
            outFile << temp->firstname << endl;
            outFile << temp->lastname << endl;
            outFile << temp->cnic << endl;
            outFile << temp->department << endl;
            outFile << temp->salary << endl;
            outFile << temp->contact << endl;
            temp = temp->next;
        }
        outFile.close();
    }

    // Load data from file
    void loadFromFile() {
        ifstream inFile("employees.txt");
        if (!inFile) return; // File doesn't exist yet, that's fine

        while (true) {
            EmployeeNode *newNode = new EmployeeNode;
            if (!(inFile >> newNode->empid)) {
                delete newNode;
                break;
            }
            inFile >> newNode->firstname;
            inFile >> newNode->lastname;
            inFile >> newNode->cnic;
            inFile.ignore(); // Skip newline
            getline(inFile, newNode->department);
            inFile >> newNode->salary;
            inFile >> newNode->contact;

            newNode->next = nullptr;
            newNode->prev = nullptr;

            // Insert at end (assuming file is sorted or order doesn't matter for loading)
            if (head == nullptr) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
            totalEmployees++;
        }
        inFile.close();
    }

    // Add Employee (Sorted by ID)
    void addEmployee()
    {
        EmployeeNode *newNode = new EmployeeNode;
        
        // 1. Get Input from User
        cout << BRIGHT_CYAN << "\n  --- NEW EMPLOYEE ---" << RESET << endl;
        cout << "  ID         : "; cin >> newNode->empid;
        
        // Check for Duplicate ID
        EmployeeNode *temp = head;
        while (temp != nullptr) {
            if (temp->empid == newNode->empid) {
                cout << BRIGHT_RED << "  [ERROR] ID " << newNode->empid << " exists!" << RESET << endl;
                delete newNode;
                return;
            }
            temp = temp->next;
        }

        cout << "  First Name : "; cin >> newNode->firstname;
        cout << "  Last Name  : "; cin >> newNode->lastname;
        cout << "  CNIC       : "; cin >> newNode->cnic;
        cin.ignore(); // Clear buffer before getline
        cout << "  Department : "; getline(cin, newNode->department);
        cout << "  Salary     : "; cin >> newNode->salary;
        cout << "  Contact    : "; cin >> newNode->contact;

        newNode->next = nullptr;
        newNode->prev = nullptr;

        totalEmployees++;

        // 2. Insert into List (Sorted Order)
        
        // Case A: List is empty
        if (head == nullptr)
        {
            head = newNode;
            tail = newNode;
        }
        // Case B: Insert at Head (Smallest ID)
        else if (head->empid >= newNode->empid)
        {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        // Case C: Insert in Middle or End
        else {
            EmployeeNode *current = head;
            while (current->next != nullptr && current->next->empid < newNode->empid)
            {
                current = current->next;
            }

            newNode->next = current->next;
            if (current->next != nullptr)
            {
                current->next->prev = newNode;
            }
            newNode->prev = current;
            current->next = newNode;

            if (newNode->next == nullptr)
            {
                tail = newNode;
            }
        }
        
        saveToFile(); // Save changes
        cout << BRIGHT_GREEN << "\t [SUCCESS] Employee Added!" << RESET << endl;
    }

    // Helper to print table header
    void printTableHeader() {
        cout << BRIGHT_BLUE << "\n  +------+----------+----------+----------------+--------+----------+" << RESET << endl;
        cout << BRIGHT_BLUE << "  | " << RESET << left << setw(4) << "ID" 
             << BRIGHT_BLUE << " | " << RESET << setw(8) << "F.Name" 
             << BRIGHT_BLUE << " | " << RESET << setw(8) << "L.Name" 
             << BRIGHT_BLUE << " | " << RESET << setw(14) << "Dept" 
             << BRIGHT_BLUE << " | " << RESET << setw(6) << "Salary" 
             << BRIGHT_BLUE << " | " << RESET << setw(8) << "Contact" 
             << BRIGHT_BLUE << " |" << RESET << endl;
        cout << BRIGHT_BLUE << "  +------+----------+----------+----------------+--------+----------+" << RESET << endl;
    }

    // Helper to print a single row
    void printEmployeeRow(EmployeeNode* node) {
        cout << BRIGHT_BLUE << "  | " << RESET << left << setw(4) << node->empid 
             << BRIGHT_BLUE << " | " << RESET << setw(8) << node->firstname.substr(0,8) 
             << BRIGHT_BLUE << " | " << RESET << setw(8) << node->lastname.substr(0,8) 
             << BRIGHT_BLUE << " | " << RESET << setw(14) << node->department.substr(0,14) 
             << BRIGHT_BLUE << " | " << RESET << setw(6) << node->salary 
             << BRIGHT_BLUE << " | " << RESET << setw(8) << node->contact.substr(0,8) 
             << BRIGHT_BLUE << " |" << RESET << endl;
    }

    // Display All Employees
    void displayAll()
    {
        if (head == nullptr) {
            cout << BRIGHT_RED << "\n\t List is Empty!" << RESET << endl;
            return;
        }

        printTableHeader();
        EmployeeNode *temp = head;
        while (temp != nullptr)
        {
            printEmployeeRow(temp);
            temp = temp->next;
        }
        cout << BRIGHT_BLUE << "  +------+----------+----------+----------------+--------+----------+" << RESET << endl;
    }

    // Search by ID
    void searchById()
    {
        int id;
        cout << "\n\t Enter ID to Search: ";
        cin >> id;

        EmployeeNode *temp = head;
        while (temp != nullptr)
        {
            if (temp->empid == id)
            {
                cout << BRIGHT_GREEN << "\n\t [FOUND]" << RESET << endl;
                printTableHeader();
                printEmployeeRow(temp);
                cout << BRIGHT_BLUE << "  +--------+---------------+---------------+--------------------+----------+---------------+" << RESET << endl;
                return;
            }
            temp = temp->next;
        }
        cout << BRIGHT_RED << "\n\t Employee with ID " << id << " not found." << RESET << endl;
    }

    // Display by Department
    void displayByDepartment()
    {
        string dept;
        cout << "\n\t Enter Department Name: ";
        cin.ignore();
        getline(cin, dept);

        string searchDept = toLower(dept);
        bool found = false;
        EmployeeNode *temp = head;

        cout << BRIGHT_CYAN << "\n\t --- Department: " << dept << " ---" << RESET << endl;
        printTableHeader();

        while (temp != nullptr)
        {
            if (toLower(temp->department) == searchDept)
            {
                printEmployeeRow(temp);
                found = true;
            }
            temp = temp->next;
        }
        cout << BRIGHT_BLUE << "  +--------+---------------+---------------+--------------------+----------+---------------+" << RESET << endl;

        if (!found) cout << BRIGHT_RED << "\t No employees found in this department." << RESET << endl;
    }

    // Update Employee
    void updateEmployee()
    {
        int id;
        cout << "\n  ID to Update: ";
        cin >> id;

        EmployeeNode *temp = head;
        while (temp != nullptr)
        {
            if (temp->empid == id)
            {
                cout << BRIGHT_YELLOW << "\n  --- Updating ID " << id << " ---" << RESET << endl;
                cout << "  New F.Name : "; cin >> temp->firstname;
                cout << "  New L.Name : "; cin >> temp->lastname;
                cout << "  New Dept   : "; cin.ignore(); getline(cin, temp->department);
                cout << "  New Salary : "; cin >> temp->salary;
                cout << "  New Contact: "; cin >> temp->contact;
                
                saveToFile(); // Save changes
                cout << BRIGHT_GREEN << "\n\t [SUCCESS] Record Updated!" << RESET << endl;
                return;
            }
            temp = temp->next;
        }
        cout << BRIGHT_RED << "\n\t ID not found!" << RESET << endl;
    }

    // Delete Employee
    void deleteEmployee()
    {
        int id;
        cout << "\n\t Enter ID to Delete: ";
        cin >> id;

        EmployeeNode *temp = head;
        while (temp != nullptr)
        {
            if (temp->empid == id)
            {
                // Unlink the node
                if (temp->prev != nullptr) temp->prev->next = temp->next;
                if (temp->next != nullptr) temp->next->prev = temp->prev;
                
                if (temp == head) head = temp->next;
                if (temp == tail) tail = temp->prev;

                delete temp;
                totalEmployees--;
                
                saveToFile(); // Save changes
                cout << BRIGHT_GREEN << "\n\t [SUCCESS] Employee Deleted." << RESET << endl;
                return;
            }
            temp = temp->next;
        }
        cout << BRIGHT_RED << "\n\t ID not found!" << RESET << endl;
    }

    // Stats
    void showStats() {
        long double totalSalary = 0;
        double maxSalary = -1;
        
        EmployeeNode *temp = head;
        while (temp != nullptr) {
            totalSalary += temp->salary;
            if (temp->salary > maxSalary) maxSalary = temp->salary;
            temp = temp->next;
        }

        cout << "\n\t Total Employees : " << totalEmployees << endl;
        cout << "\t Total Salary    : " << totalSalary << endl;
        cout << "\t Highest Salary  : " << maxSalary << endl;
    }
};

int main()
{
    Authentication auth;
    EmployeeList empList;

    // 1. Sign Up & Login
    if (!auth.welcomeScreen()) {
        return 0; // User chose exit
    }

    // 2. Main Menu Loop
    int choice;
    do {
        system("cls");
        cout << BRIGHT_BLUE << "\n  ==============================" << RESET << endl;
        cout << BRIGHT_WHITE << "  |       MAIN MENU            |" << RESET << endl;
        cout << BRIGHT_BLUE << "  ==============================" << RESET << endl;
        cout << "  [1] INSERT EMPLOYEE" << endl;
        cout << "  [2] DISPLAY ALL" << endl;
        cout << "  [3] DISPLAY BY DEPT" << endl;
        cout << "  [4] SEARCH BY ID" << endl;
        cout << "  [5] UPDATE EMPLOYEE" << endl;
        cout << "  [6] DELETE EMPLOYEE" << endl;
        cout << "  [7] STATS" << endl;
        cout << BRIGHT_RED  << "  [0] EXIT" << RESET << endl;
        cout << BRIGHT_BLUE << "  ==============================" << RESET << endl;
        cout << "  CHOICE: ";
        cin >> choice;

        switch (choice)
        {
        case 1: empList.addEmployee(); break;
        case 2: empList.displayAll(); break;
        case 3: empList.displayByDepartment(); break;
        case 4: empList.searchById(); break;
        case 5: empList.updateEmployee(); break;
        case 6: empList.deleteEmployee(); break;
        case 7: empList.showStats(); break;
        case 0: 
            cout << "\n  Exiting..."; 
            auth.loadingBar();
            return 0;
        default: cout << BRIGHT_RED << "\n  Invalid Choice!" << RESET << endl;
        }
        
        cout << "\n  ";
        system("pause");

    } while (choice != 0);

    return 0;
}