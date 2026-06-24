#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include <filesystem>
using namespace std;

struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};

struct Node {
    Course course;
    Node *left;
    Node *right;

    Node() {
        left = nullptr;
        right = nullptr;
    }

    Node(const Course &course) : Node() {
        this->course = course;
    }
};

void displayMenu() {
    cout << "1. Load Courses." << endl;
    cout << "2. Print Course List." << endl;
    cout << "3. Print Course." << endl;
    cout << "9. Exit." << endl;
    cout << "What would you like to do?" << endl;
}

//opening file and populating lines vector
void populateLines(const string &fileName, vector<string> &lines) {
    cout << "Opening " << std::filesystem::absolute(fileName) << endl;
    //creating input file stream
    std::ifstream inputFile(fileName);
    //if the file opens successfully
    if (inputFile.is_open()) {
        //read lines and putting non-empty ones to the vector as is
        string line;
        while (getline(inputFile, line)) {
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
        //closing input file
        inputFile.close();
        //if opening the file fails
    } else {
        //notify user about the failure
        cout << "Failed to open file " << fileName << endl;
    }
}

class BinarySearchTree {
    //private methods declarations
private:
    Node *root;

    void addNode(Node *node, const Course &course);

    void inOrder(Node *node);

    static void deleteNode(const Node *node);
    //public methods declarations
public:
    BinarySearchTree();

    ~BinarySearchTree();

    void printCourses();

    void insertCourse(const Course &course);

    Course *searchCourse(const string &courseNumber) const;
};

BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

void BinarySearchTree::deleteNode(const Node *node) {
    if (node == nullptr) { return; }
    deleteNode(node->left);
    deleteNode(node->right);
    delete node;
}

BinarySearchTree::~BinarySearchTree() {
    deleteNode(root);
}

void BinarySearchTree::insertCourse(const Course &course) {
    //if the tree is empty
    if (root == nullptr) {
        //the course will be the root node
        root = new Node(course);
        //otherwise
    } else {
        //call method for adding node
        this->addNode(root, course);
    }
}

void BinarySearchTree::printCourses() {
    //call in order metod starting from the root
    this->inOrder(root);
}

Course *BinarySearchTree::searchCourse(const string &courseNumber) const {
    //set current node to the root
    Node *curNode = root;
    //while current node exists
    while (curNode != nullptr) {
        //if the course numbers match
        if (curNode->course.courseNumber == courseNumber) {
            //return the course
            return &curNode->course;
        }
        //if the searched course number is less than that of current node
        if (courseNumber.compare(curNode->course.courseNumber) < 0) {
            //traverse left
            curNode = curNode->left;
            //otherwise
        } else {
            //traverse right
            curNode = curNode->right;
        }
    }
    //return nullptr if the course was not found
    return nullptr;
}

void BinarySearchTree::addNode(Node *node, const Course &course) {
    //if the node course number is alphanumericly larger than that of the course to be inserted
    if (node->course.courseNumber.compare(course.courseNumber) > 0) {
        //if there is no node to the left
        if (node->left == nullptr) {
            //the course to be inserted goes to the left node
            node->left = new Node(course);
            //otherwise
        } else {
            //recurse down the left node
            this->addNode(node->left, course);
        }//if the node course number is alphanumericly smaller than that of the course to be inserted
    } else {
        //if there is no node to the right
        if (node->right == nullptr) {
            //the course to be inserted goes to the right node
            node->right = new Node(course);
            //otherwise
        } else {
            //recurse down the right node
            this->addNode(node->right, course);
        }
    }
}

void BinarySearchTree::inOrder(Node *node) {
    //if node exists
    if (node != nullptr) {
        //call in order on the left node
        inOrder(node->left);
        //print course number and name
        cout << node->course.courseNumber << ", " << node->course.courseName << endl;
        //call in order on the right node
        inOrder(node->right);
    }
}

vector<Course> refineCourses(vector<Course> courses) {
    //for each course
    for (Course course: courses) {
        vector<string> validPrerequisites;
        //for each prerequisite
        for (string prerequisite: course.prerequisites) {
            //if course number is present among course numbers of courses in courses vector
            auto it = ranges::find_if(
                courses, [&](const Course &course) { return course.courseNumber == prerequisite; });
            if (it != courses.end()) {
                //add the prerequisite to the valid prerequisites vector
                validPrerequisites.push_back(prerequisite);
                //otherwise notify user of an invalid prerequisite for the course
            } else {
                cout << "Invalid prerequisite " << prerequisite << " for course " << course.courseNumber << endl;
            }
        }
        //save valid prerequisites as the course's prerequisites
        course.prerequisites = validPrerequisites;
    }
    //return courses with refined prerequisites
    return courses;
}

//reading lines from csv into the vector, parsing them, creating courses
vector<Course> loadCourses(const string &fileName) {
    cout << "Loading courses" << endl;
    regex courseNumberPattern(R"(^[A-Z]{4}\d{3}$)");//pattern corresponding to course numbers like CSCI350
    vector<string> lines;
    vector<Course> courses;
    //read file and write non-empty lines to the vector
    populateLines(fileName, lines);
    //parse each line by commas, write tokens to the vector
    for (const string &line: lines) {
        stringstream lineStream(line);
        string token;
        vector<string> tokens;
        while (getline(lineStream, token, ',')) {
            tokens.push_back(token);
        }
        //if name or number is absent, notify user of invalid data
        if (tokens.size() < 2) {
            cout << "Invalid course data" << endl;
        }
        //if course number format is wrong, notify user
        if (!regex_match(tokens.at(0), courseNumberPattern)) {
            cout << "Invalid course number format " << tokens.at(0) << endl;
            //if the previous checkes are passed
        } else {
            //create a course
            Course course;
            //write first token as the course number
            course.courseNumber = tokens.at(0);
            //write second course as a course name
            course.courseName = tokens.at(1);
            //the following tokens, if present, must be saved as prerequisites
            if (tokens.size() > 2) {
                vector<string> prerequisites;
                for (int i = 2; i < tokens.size(); ++i) {
                    const string &prerequisite = tokens.at(i);
                    if (prerequisite.empty())
                        continue;
                    prerequisites.push_back(prerequisite);
                    course.prerequisites = prerequisites;
                }
            }
            //save the course in the courses vector
            courses.push_back(course);
        }
    }
    return courses;
}

void populateTree(const vector<Course> &courses, BinarySearchTree *coursesTree) {
    //for each course
    for (const Course &course: courses) {
        //call insert method
        coursesTree->insertCourse(course);
    }
}

//converts each character of a string to upper case
string toUpperCase(string str) {
    for (char &c: str) {
        c = toupper(c);
    }
    return str;
}

void printCourse(const Course &course) {
    //print course number and name
    cout << course.courseNumber << ", " << course.courseName << endl;
    cout << "Prerequisites: ";
    //if there are no prerequisites
    if (course.prerequisites.empty()) {
        //print "none"
        cout << "None";
        //otherwise
    } else {
        //print prerequisites separated by commas withou a comma at the end of the list
        std::string delimiter;//default value - empty string
        for (const auto &prerequisite: course.prerequisites) {
            cout << delimiter << prerequisite;
            delimiter = ", ";//delimiter set to comma and printed before the prerequisite in the next iteration
        }
    }
}

//entry point of the program
int main() {
    //BST instantiation
    BinarySearchTree *coursesTree = new BinarySearchTree();
    cout << "Welcome to the course planner." << endl;
    string input;
    //displaying the menu till the user enters 9(exit)
    while (input != "9") {
        displayMenu();
        cin >> input;
        //check for invalid option
        if (input != "1" && input != "2" && input != "3" && input != "9") {
            cout << input << " is not a valid option." << endl;
            continue;
        }
        //loading courses into BST
        if (input == "1") {
            cout << "Enter file name." << endl;
            string fileName;
            std::cin.ignore();
            std::getline(std::cin, fileName);
            //load courses data
            vector<Course> courses = loadCourses(fileName);
            //check that the prerequisites are valid
            vector<Course> refinedCourses = refineCourses(courses);
            //insert each course into the BST based on alphanumeric order
            populateTree(refinedCourses, coursesTree);
            continue;
        }
        if (input == "2") {
            cout << "Here is a sample schedule: " << endl;
            //print courses in alphanumeric order
            coursesTree->printCourses();
            cout << endl;
            continue;
        }
        //searching course by course number and printing it
        if (input == "3") {
            //prompting user for course number
            cout << "What course do you want to know about?" << endl;
            string courseNumber;
            regex courseNumberPattern(R"(^[a-zA-Z]{4}\d{3}$)");
            cin >> courseNumber;
            //converting user's input to upper case for correct search
            courseNumber = toUpperCase(courseNumber);
            //if user enters the course number in the wrong format
            if (!regex_match(courseNumber, courseNumberPattern)) {
                //notify the user about the wrong format
                cout << "Invalid course number " << courseNumber << endl;
                //otherwise
            } else {
                //serch for the course
                const Course *foundCourse = coursesTree->searchCourse(courseNumber);
                //if the course was not found
                if (foundCourse == nullptr) {
                    //notify the user
                    cout << "Invalid course " << courseNumber << endl;
                    //otherwise
                } else {
                    //print the course info
                    printCourse(*foundCourse);
                }
                cout << endl;
            }
        }
    }
    cout << "Thank you for using the course planner." << endl;
    return 0;
}
