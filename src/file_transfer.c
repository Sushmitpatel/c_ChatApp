#include "common.h"
#define Buffer_size 1024

int file_exists(const char *file_name)
{
    struct stat buffer;
    return (stat(file_name, &buffer) == 0);
}

int is_file_accessible(char *file_path)
{
    file_path[strcspn(file_path, "\n")] = 0;
    if (access(file_path, F_OK) != 0)
    {
        // printf("Error: File '%s' does not exist!\n", file_path);
        return 0;
    }

    if (access(file_path, R_OK) != 0)
    {
        printf("Error: File '%s' is not readable!\n", file_path);
        return 0;
    }
    return 1;
}
void send_file(int sockfd, char *file_path)
{
    // Open the file for reading
    FILE *file = fopen(file_path, "rb");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Extract the file name
    char *file_name = basename(file_path);
    uint32_t file_name_len = strlen(file_name);

    // Step 1: Send the file name length
    if (send(sockfd, &file_name_len, sizeof(file_name_len), 0) <= 0)
    {
        perror("Error sending file name length");
        fclose(file);
        return;
    }

    // Step 2: Send the file name
    if (send(sockfd, file_name, file_name_len, 0) <= 0)
    {
        perror("Error sending file name");
        fclose(file);
        return;
    }

    // Step 3: Send the file size
    if (send(sockfd, &file_size, sizeof(file_size), 0) <= 0)
    {
        perror("Error sending file size");
        fclose(file);
        return;
    }

    // Step 4: Send the file content
    char buffer[Buffer_size];
    long bytes_sent = 0;
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        ssize_t sent = send(sockfd, buffer, bytes_read, 0);
        if (sent <= 0)
        {
            perror("Error sending file data");
            break;
        }
        bytes_sent += sent;

        // Display progress
        float progress = (float)bytes_sent / file_size * 100;
        printf("\rSending file... %.2f%%", progress);
        fflush(stdout);
    }

    // Step 5: Check if the file was fully sent
    if (bytes_sent == file_size)
    {
        printf("\nFile '%s' sent successfully.\n", file_name);
    }
    else
    {
        fprintf(stderr, "\nError: File transfer incomplete. Sent %ld/%ld bytes.\n", bytes_sent, file_size);
        should_terminate = 1;
    }

    // Close the file
    fclose(file);
}

void receive_file(int sockfd)
{
    // Step 1: Receive the file name length
    uint32_t name_len;
    if (recv(sockfd, &name_len, sizeof(name_len), 0) <= 0)
    {
        perror("Failed to receive file name length. Connection closed or error occurred.");
        should_terminate = 1;
        return;
    }

    // Step 2: Receive the file name
    char file_name[name_len + 1];
    if (recv(sockfd, file_name, name_len, 0) <= 0)
    {
        perror("Failed to receive file name. Connection closed or error occurred.");
        should_terminate = 1;
        return;
    }
    file_name[name_len] = '\0';

    
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", download_dir, file_name);
    int count = 1;
    char temp_path[1024];
    while (file_exists(full_path))
    {
        snprintf(temp_path, sizeof(temp_path), "%s%s_%d", download_dir, file_name, count++);
        strncpy(full_path, temp_path, sizeof(full_path));
    }
    printf("%s :\n",full_path);
    fflush(stdout);

    // Step 4: Receive the file size
    long file_size;
    if (recv(sockfd, &file_size, sizeof(file_size), 0) <= 0)
    {
        perror("Failed to receive file size. Connection closed or error occurred.");
        return;
    }

    // Step 5: Open the file for writing
    FILE *file = fopen(full_path, "wb");
    if (file == NULL)
    {
        perror("Failed to open file for writing");
        return;
    }

    // Step 6: Receive the file content
    char buffer[1024];
    long bytes_received = 0;
    ssize_t bytes;
    while (bytes_received < file_size)
    {
        bytes = recv(sockfd, buffer, sizeof(buffer), 0);

        if (bytes == 0)
        { // Connection closed by the client
            fprintf(stderr, "\nConnection closed by the client during file transfer.\n");
            break;
        }
        else if (bytes < 0)
        { // Error occurred
            perror("\nFailed to receive file data");
            break;
        }

        fwrite(buffer, 1, bytes, file);
        bytes_received += bytes;

        // Display progress
        float progress = (float)bytes_received / file_size * 100;
        printf("\rReceiving file... %.2f%%", progress);
        fflush(stdout);
    }

    // Step 7: Close the file and log the result
    fclose(file);

    if (bytes_received == file_size)
    {
        printf("\nFile '%s' received successfully.\n", file_name);
    }
    else
    {
        fprintf(stderr, "\nFile transfer incomplete. Received %ld/%ld bytes.\n", bytes_received, file_size);
    }
}
