/*#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include "../encryptDecrypt/Cryption.hpp"

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    taskQueue.push(std::move(task));
    int pid=fork();
    if(pid<0){
        return false;
    }
    else if(pid>0){
        std::cout<<"entering parentb process"<<std::endl;
    }
    else{
        std::cout<<"entering the child process"<<std::endl;
        executeTasks();
        std::cout<<"exiting the cild process"<<std::endl;
    }
    return true;
}

void ProcessManagement::executeTasks() {
    while (!taskQueue.empty()) {
        std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());
        taskQueue.pop();
        std::cout << "Executing task: " << taskToExecute->toString() << std::endl;
        // Add a breakpoint here in VS Code
        //executeCryption(taskToExecute->toString());
        int childProcessToRun = fork();
        if (childProcessToRun == 0) {
            // Child process
            std::string taskStr = taskToExecute->toString();
            char* args[3];
            args[0] = strdup("./cryption");  // Use the correct path to your cryption executable
            args[1] = strdup(taskStr.c_str());
            args[2] = nullptr;
            execv("./cryption", args);  // Use the correct path to your cryption executable
            // If execv returns, there was an error
            std::cerr << "Error executing cryption" << std::endl;
            exit(1);
        } else if (childProcessToRun > 0) {
            // Parent process
            // Wait for the child process to complete
            int status;
            waitpid(childProcessToRun, &status, 0);
        } else {
            // Fork failed
            std::cerr << "Fork failed" << std::endl;
            exit(1);
        }
    }
}*/

#include <iostream>
#include "ProcessManagement.hpp"
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include "../encryptDecrypt/Cryption.hpp"

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    taskQueue.push(std::move(task));
    return true;
}

void ProcessManagement::executeTasks() {
    std::vector<pid_t> childPIDs;  // To store PIDs of all forked processes

    while (!taskQueue.empty()) {
        std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());
        taskQueue.pop();

        std::string taskStr = taskToExecute->toString();
        std::cout << "Forking for task: " << taskStr << std::endl;

        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            std::cout << "Child [" << getpid() << "] executing task: " << taskStr << std::endl;
            char* args[3];
            args[0] = strdup("./cryption");  // Path to cryption executable
            args[1] = strdup(taskStr.c_str()); // Task string as argument
            args[2] = nullptr;

            execv("./cryption", args);  // Replace current process with cryption
            // If execv fails
            std::cerr << "Child [" << getpid() << "] error: Failed to execute cryption" << std::endl;
            exit(1);
        } 
        else if (pid > 0) {
            // Parent process: store child PID
            childPIDs.push_back(pid);
        } 
        else {
            // Fork failed
            std::cerr << "Error: Failed to fork process" << std::endl;
        }
    }

    // Wait for all child processes to finish
    for (pid_t pid : childPIDs) {
        int status;
        waitpid(pid, &status, 0);
        std::cout << "Child [" << pid << "] finished with status " << status << std::endl;
    }
}
