#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crypt.h>

#define PASSWD_LEN 5

/**
 * @brief Generate the initial character based on the rank of the node
 *
 * This character is unique for each node (making the node search only the 1/26 of all the combinations).
 *
 * @param rank current node's rank
 * @return char
 */
char generate_character_for_node(const int rank)
{
    return 'a' + rank;
}

/**
 * @brief Recursively generate a string of PASSWD_LEN characters and compare it with the encrypted target.
 *
 * @param rank rank of the MPI node that's running this code
 * @param len how many characters are left to generate
 * @param string the string we are generating
 * @param salt the salt used by the `crypt` function to create the target
 * @param target the result of the `crypt` function to compare against
 */
void generate_string(const int rank, int len, char *string, const char *salt, const char *target)
{
    /**
     * @brief Current position in the `string`.
     *
     * This is the position in the array of chars that a new character will be inserted into.
     */
    int current_position = PASSWD_LEN - len;

    // When there's no more characters to generate and add to the string...
    if (len == 0)
    {
        // ... use the system function—crypt—to hash the string...
        char hash[20] = {0};               // placeholder
        strcpy(hash, crypt(string, salt)); // move the encrypted text into the `hash` variable

        // ... and compare it against the target
        if (strcmp(hash, target) == 0)
            // if we've got a hit, print it
            printf("%s -> %s = (expected) %s\t[in node no. %d]\n", string, hash, target, rank);

        // finish the recursion
        return;
    }

    /**
     * @brief Set the first char based on the rank
     * @see generate_character_for_node
     */
    if (current_position == 0)
    {
        string[current_position] = generate_character_for_node(rank);
        generate_string(rank, len - 1, string, salt, target);
    }
    else
        /**
         * @brief Generate 26 different strings with 26 different characters in the current_position
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
    /**
     * @brief Salt used to hash the target
     */
    const char *SALT = "aa";
    /**
     * @brief The target for which we want to find the original text
     */
    const char *TARGET = "aa7uckMQPpN46";
    /**
     * @brief The rank (number) of the current MPI node (process).
     *
     * This identifies the process that is running the code in the distributed network.
     */
    int rank;
    /**
     * @brief Ths size of the NPI network.
     */
    int size;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int namelen;

    // Start the distributed processing
    MPI_Init(&argc, &argv);

    // Get data about the current node and the whole network
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);

    /**
     * @brief A placeholder for the PASSWD_LEN long strings we will be generating.
     * @see generate_string
     */
    char text[PASSWD_LEN + 1] = {0};
    generate_string(rank, PASSWD_LEN, text, SALT, TARGET);

    // Finish the distributed processing
    MPI_Finalize();
}
