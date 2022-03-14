#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>

#define PASSWD_LEN 5

/**
 * @brief Generate initial character based on the rank of the executor-node
 *
 * @param rank current node's rank
 * @return char
 */
char generate_character_for_node(const int rank)
{
    return 'a' + rank;
}

void generate_string(const int rank, int len, char *string, const char *salt, const char *target)
{
    /**
     * @brief Current position in the `string`.
     * 
     * This is the position in the array of chars that a new character will be inserted into.
     */
    int current_position = PASSWD_LEN - len;

    if (len == 0)
    {
        char hash[20] = {0};
        /**
         * @brief Encrypt 
         * 
         */
        strcpy(hash, crypt(string, salt));
        if (strcmp(hash, target) == 0)
        {
            printf("[%d] %d -> %s => %s vs. %s\n", rank, current_position, string, hash, target);
        }

        return;
    }

    /**
     * @brief Set the first char based on the rank
     * @see generate_character_for_node
     */
    if (current_position == 0)
    {
        string[current_position] = generate_character_for_node(rank);
        return generate_string(rank, len - 1, string, salt, target);
    }

    /**
     * @brief
     *
     */
    for (int i = 0; i < 26; i++)
    {
        char letter = 'a' + i;
        string[current_position] = letter;
        generate_string(rank, len - 1, string, salt, target);
    }
}

int main(int argc, char **argv)
{
    const char *SALT = "aa";
    const char *TARGET = "aa7uckMQPpN46";

    int rank, size;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int namelen;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);
    // printf("Jestem %d z %d na %s\n", rank, size, processor_name);

    char text[PASSWD_LEN + 1] = {0};
    // generate_character_for_node(rank);
    generate_string(rank, PASSWD_LEN, text, SALT, TARGET);
    // printf("Jestem %d, mam znak: %s\n", rank, sign);

    MPI_Finalize();
}