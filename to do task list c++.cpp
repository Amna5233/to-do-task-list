#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// Base Task class
class Task {
protected:
    string description;
    string dueDate;
    bool isComplete;

public:
    Task(const string& desc, const string& date)
        : description(desc), dueDate(date), isComplete(false) {}

    virtual ~Task() {}

    void markComplete() {
        isComplete = true;
    }

    virtual void display() const {
        cout << description << " [Due: " << dueDate << "] ["
             << (isComplete ? "Complete" : "Incomplete") << "]" << endl;
    }

    virtual string saveFormat() const {
        return description + "," + dueDate + "," + (isComplete ? "1" : "0");
    }

    virtual void loadFormat(const string& desc, const string& date, const string& status, const string& priority = "") {
        description = desc;
        dueDate = date;
        isComplete = (status == "1");
    }

    string getDueDate() const {
        return dueDate;
    }

    bool isTaskComplete() const {
        return isComplete;
    }
};

// Derived PriorityTask class
class PriorityTask : public Task {
private:
    int priorityLevel;

public:
    PriorityTask(const string& desc, const string& date, int priority)
        : Task(desc, date), priorityLevel(priority) {}

    void display() const override {
        cout << description << " [Priority: " << priorityLevel
             << "] [Due: " << dueDate << "] ["
             << (isComplete ? "Complete" : "Incomplete") << "]" << endl;
    }

    string saveFormat() const override {
        return description + "," + dueDate + "," + (isComplete ? "1" : "0") + "," + to_string(priorityLevel);
    }

    void loadFormat(const string& desc, const string& date, const string& status, const string& priority = "") override {
        Task::loadFormat(desc, date, status);
        if (!priority.empty()) {
            priorityLevel = stoi(priority);
        }
    }
};

// TaskList class to manage tasks
class TaskList {
private:
    vector<Task*> loadedTasks;
    vector<Task*> newTasks;
    string filename;

public:
    TaskList(const string& fname) : filename(fname) {
        loadTasks();
    }

    ~TaskList() {
        saveTasks();
        for (Task* task : loadedTasks) {
            delete task;
        }
        for (Task* task : newTasks) {
            delete task;
        }
    }

    void addTask(Task* task) {
        newTasks.push_back(task);
        saveTasks();
    }

    void displayTasks() const {
        if (newTasks.empty()) {
            cout << "No new tasks set\n";
            return;
        }
        for (size_t i = 0; i < newTasks.size(); ++i) {
            cout << (i + 1) << ". ";
            newTasks[i]->display();
        }
    }

    void markTaskComplete(int index) {
        if (index >= 0 && static_cast<size_t>(index) < newTasks.size()) {
            newTasks[index]->markComplete();
            saveTasks();
        } else {
            cout << "Invalid task index\n";
        }
    }

    void saveTasks() const {
        ofstream outfile(filename);
        if (!outfile) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        for (Task* task : loadedTasks) {
            outfile << task->saveFormat() << endl;
        }
        for (Task* task : newTasks) {
            outfile << task->saveFormat() << endl;
        }
        outfile.close();
    }

    void loadTasks() {
        ifstream infile(filename);
        if (!infile) {
            ofstream outfile(filename); // Create the file if it doesn't exist
            if (!outfile) {
                cerr << "Error creating file: " << filename << endl;
            }
            return;
        }

        string line;
        while (getline(infile, line)) {
            stringstream ss(line);
            string desc, date, status, priority;
            getline(ss, desc, ',');
            getline(ss, date, ',');
            getline(ss, status, ',');

            Task* task = nullptr;
            if (getline(ss, priority, ',')) {
                task = new PriorityTask(desc, date, stoi(priority));
                task->loadFormat(desc, date, status, priority);
            } else {
                task = new Task(desc, date);
                task->loadFormat(desc, date, status);
            }
            loadedTasks.push_back(task);
        }
        infile.close();
    }

    void remindIncompleteTasks() const {
        for (size_t i = 0; i < newTasks.size(); ++i) {
            if (!newTasks[i]->isTaskComplete()) {
                cout << "Reminder: Task " << (i + 1) << " is incomplete: ";
                newTasks[i]->display();
            }
        }
    }

    void notifyDueTasks(const string& currentDate) const {
        bool tasksDue = false;
        for (size_t i = 0; i < newTasks.size(); ++i) {
            if (newTasks[i]->getDueDate() == currentDate) {
                tasksDue = true;
                cout << "Notification: Task " << (i + 1) << " is due today: ";
                newTasks[i]->display();
            }
        }
        if (!tasksDue) {
            cout << "No tasks are due today." << endl;
        }
    }
};

void showMenu() {
    cout << "\n1. Add Standard Task\n";
    cout << "2. Add Priority Task\n";
    cout << "3. View New Tasks\n";
    cout << "4. Mark Task Complete\n";
    cout << "5. Remind Incomplete Tasks\n";
    cout << "6. Notify Due Tasks\n";
    cout << "7. Exit\n";
    cout << "Choose an option: ";
}

int main() {
    TaskList taskList("tasks.txt");

    int choice;
    string description, dueDate, currentDate;
    int priority, taskIndex;

    while (true) {
        showMenu();
        cin >> choice;
        cin.ignore();  // To consume the newline character left in the buffer

        switch (choice) {
        case 1:
            cout << "Enter task description: ";
            getline(cin, description);
            cout << "Enter due date (YYYY-MM-DD): ";
            getline(cin, dueDate);
            taskList.addTask(new Task(description, dueDate));
            break;
        case 2:
            cout << "Enter task description: ";
            getline(cin, description);
            cout << "Enter due date (YYYY-MM-DD): ";
            getline(cin, dueDate);
            cout << "Enter priority level: ";
            cin >> priority;
            cin.ignore();  // To consume the newline character left in the buffer
            taskList.addTask(new PriorityTask(description, dueDate, priority));
            break;
        case 3:
            taskList.displayTasks();
            break;
        case 4:
            cout << "Enter task number to mark complete: ";
            cin >> taskIndex;
            taskList.markTaskComplete(taskIndex - 1);
            break;
        case 5:
            taskList.remindIncompleteTasks();
            break;
        case 6:
            cout << "Enter current date (YYYY-MM-DD): ";
            getline(cin, currentDate);
            taskList.notifyDueTasks(currentDate);
            break;
        case 7:
            return 0;
        default:
            cout << "Invalid option. Please try again.\n";
        }
    }

    return 0;
}