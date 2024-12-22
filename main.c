#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Task structure
typedef struct Task {
    char operation[20]; // operation like create, append, delete
    char filename[50];
    char data[100]; // data for append operation
} Task;

// Global task queue
Task taskQueue[10];
int taskCount = 0;

// Simulated memory
long long totalMemory = 1024;  // Total memory in bytes
long long usedMemory = 0;
long long freeMemory = 1024;

// Tree-based file system node structure
typedef struct FileNode {
    char filename[50];
    int size;  // size in bytes
    char data[100];  // data for the file
    struct FileNode *left;  // Pointer to the left child (for simplicity, binary tree)
    struct FileNode *right;  // Pointer to the right child
} FileNode;

FileNode *root = NULL;  // Root of the file tree

// Function declarations
void createFile();
void appendToFile();
void deleteFile();
void displayDirectoryTree();
void displayMemoryUsage();
void processTasks();
void addTask(char *operation, char *filename, char *data);
void displayFiles();
void displayFilesRecursive(FileNode *node);
void writeFile(char *filename, char *data);
void searchFile();
void displayDirectoryTreeRecursive(FileNode *node);

// Task addition
void addTask(char *operation, char *filename, char *data) {
    strcpy(taskQueue[taskCount].operation, operation);
    strcpy(taskQueue[taskCount].filename, filename);
    if (data != NULL) {
        strcpy(taskQueue[taskCount].data, data);
    }
    taskCount++;
}

// Helper function to create a new file node
FileNode* createFileNode(char *filename, int size, char *data) {
    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));
    strcpy(newNode->filename, filename);
    newNode->size = size;
    strcpy(newNode->data, data);
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Insert a file into the tree (simplified, binary insertion)
void insertFileNode(FileNode **node, char *filename, int size, char *data) {
    if (*node == NULL) {
        *node = createFileNode(filename, size, data);
    } else {
        if (strcmp(filename, (*node)->filename) < 0) {
            insertFileNode(&((*node)->left), filename, size, data);
        } else {
            insertFileNode(&((*node)->right), filename, size, data);
        }
    }
}

// Create a file
void createFile() {
    char filename[50];
    int size;
    printf("Enter file name: ");
    scanf("%s", filename);
    printf("Enter memory required (in bytes): ");
    scanf("%d", &size);

    if (usedMemory + size <= totalMemory) {
        // Insert the file into the tree
        insertFileNode(&root, filename, size, "");

        usedMemory += size;
        freeMemory = totalMemory - usedMemory;
        printf("File '%s' created successfully.\n", filename);

        // Add the create file task
        addTask("create", filename, NULL);
    } else {
        printf("Not enough memory to create file.\n");
    }
}

// Append data to a file
void appendToFile() {
    char filename[50];
    char data[100];
    FileNode *current = root;

    printf("Enter file name to append to: ");
    scanf("%s", filename);

    // Search for the file in the tree
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            break;
        } else if (strcmp(filename, current->filename) < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    if (current != NULL) {
        printf("Enter data to append: ");
        getchar();  // Consume any leftover newline character from scanf
        fgets(data, sizeof(data), stdin);

        // Append the data to the file
        writeFile(filename, data);

        // Add the append task
        addTask("append", filename, data);
        printf("Data appended to file '%s'.\n", filename);
    } else {
        printf("File '%s' not found.\n", filename);
    }
}

// Delete a file
void deleteFile() {
    char filename[50];
    printf("Enter file name to delete: ");
    scanf("%s", filename);

    FileNode *parent = NULL;
    FileNode *current = root;

    // Search for the file in the tree
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            break;
        } else if (strcmp(filename, current->filename) < 0) {
            parent = current;
            current = current->left;
        } else {
            parent = current;
            current = current->right;
        }
    }

    if (current != NULL) {
        // File found, remove it from the tree
        if (current->left == NULL && current->right == NULL) {
            // No children, just remove the node
            if (parent == NULL) {
                root = NULL;  // Tree is empty
            } else if (parent->left == current) {
                parent->left = NULL;
            } else {
                parent->right = NULL;
            }
            free(current);
        } else if (current->left == NULL || current->right == NULL) {
            // One child
            FileNode *child = (current->left != NULL) ? current->left : current->right;
            if (parent == NULL) {
                root = child;  // Replace root node
            } else if (parent->left == current) {
                parent->left = child;
            } else {
                parent->right = child;
            }
            free(current);
        } else {
            // Two children (this is simplified, we would normally replace with the in-order successor or predecessor)
            FileNode *inorderSuccessor = current->right;
            while (inorderSuccessor->left != NULL) {
                inorderSuccessor = inorderSuccessor->left;
            }
            strcpy(current->filename, inorderSuccessor->filename);
            current->size = inorderSuccessor->size;
            strcpy(current->data, inorderSuccessor->data);
            current->right = inorderSuccessor->right;  // Bypass the successor
            free(inorderSuccessor);
        }

        usedMemory -= current->size;
        freeMemory = totalMemory - usedMemory;

        // Add the delete task
        addTask("delete", filename, NULL);

        printf("File '%s' deleted successfully.\n", filename);
    } else {
        printf("File '%s' not found.\n", filename);
    }
}

// Display the directory tree (modified to show all files without the hierarchical structure)
void displayDirectoryTree() {
    printf("All Files in the Directory:\n");
    displayFiles();
}

// Display files with contents
void displayFiles() {
    printf("Files in the directory:\n");
    displayFilesRecursive(root);
}

// Recursive function to display files
void displayFilesRecursive(FileNode *node) {
    if (node == NULL) {
        return;
    }
    displayFilesRecursive(node->left);
    printf("  File: %s [Size: %d bytes] - Content: %s\n", node->filename, node->size, node->data);
    displayFilesRecursive(node->right);
}

// Function to write data to a file (for create/append operations)
void writeFile(char *filename, char *data) {
    // For simulation, just append the data to the file node in the tree
    FileNode *current = root;
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            strcat(current->data, data);
            break;
        } else if (strcmp(filename, current->filename) < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
}

// Function to display memory usage
void displayMemoryUsage() {
    printf("Memory Usage:\n");
    printf("Total Memory: %lld bytes\n", totalMemory);
    printf("Used Memory: %lld bytes\n", usedMemory);
    printf("Free Memory: %lld bytes\n", freeMemory);
}

// Process tasks
void processTasks() {
    printf("Processing tasks...\n");
    for (int i = 0; i < taskCount; i++) {
        if (strcmp(taskQueue[i].operation, "create") == 0) {
            printf("Processing task: create %s\n", taskQueue[i].filename);
        } else if (strcmp(taskQueue[i].operation, "append") == 0) {
            printf("Processing task: append %s\n", taskQueue[i].filename);
        } else if (strcmp(taskQueue[i].operation, "delete") == 0) {
            printf("Processing task: delete %s\n", taskQueue[i].filename);
        }
    }
    taskCount = 0;  // Clear task queue after processing
}

// Search a file
void searchFile() {
    char filename[50];
    printf("Enter the filename to search: ");
    scanf("%s", filename);

    FileNode *current = root;
    while (current != NULL) {
        if (strcmp(current->filename, filename) == 0) {
            printf("File '%s' found. Size: %d bytes, Content: %s\n", current->filename, current->size, current->data);
            return;
        } else if (strcmp(filename, current->filename) < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    printf("File '%s' not found.\n", filename);
}

int main() {
    int choice;
    while (1) {
        printf("\nOptions:\n");
        printf("1. Create file\n");
        printf("2. Append to file\n");
        printf("3. Delete file\n");
        printf("4. Display all files in the directory\n");
        printf("5. Display memory usage\n");
        printf("6. Process tasks\n");
        printf("7. Display files with contents\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createFile();
                break;
            case 2:
                appendToFile();
                break;
            case 3:
                deleteFile();
                break;
            case 4:
                displayDirectoryTree();
                break;
            case 5:
                displayMemoryUsage();
                break;
            case 6:
                processTasks();
                break;
            case 7:
                displayFiles();
                break;
            case 8:
                exit(0);
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
