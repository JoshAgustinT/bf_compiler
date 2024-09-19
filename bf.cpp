/*
Joshua Tlatelpa-Agustin
9/18/24
bf language compiler
written for adv compilers course
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stack>
using namespace std;

vector<char> program_file;

ofstream *output_file;
int loop_num = -1;
std::stack<int> myStack;

int bf_interpreter(char token, int i)
{

    switch (token)
    {
    case '>':
        // Calculate the address 50,000 bytes into the allocated memory
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax
        *output_file << "addq    $1, %rax" << std::endl;       // Add the offset to %rax

        // Store the adjusted pointer in the stack space at -8(%rbp)
        *output_file << "movq    %rax, -8(%rbp)" << std::endl;
        // tape_position++;
        break;
    case '<':

        // Calculate the address 50,000 bytes into the allocated memory
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax
        *output_file << "subq    $1, %rax" << std::endl;       // Add the offset to %rax

        // Store the adjusted pointer in the stack space at -8(%rbp)
        *output_file << "movq    %rax, -8(%rbp)" << std::endl;
        // tape_position--;
        break;
    case '+':

        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax

        // Load the byte at the address in %rax
        *output_file << "movb    (%rax), %cl" << std::endl; // Load byte into %cl (lower 8 bits of %rcx)

        // Add 1 to the byte
        *output_file << "addb    $1, %cl" << std::endl; // Increment byte in %cl

        // Store the modified byte back to the address in %rax
        *output_file << "movb    %cl, (%rax)" << std::endl; // Store byte from %cl back to memory

        // tape[tape_position] += 1;
        break;
    case '-':

        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax

        // Load the byte at the address in %rax
        *output_file << "movb    (%rax), %cl" << std::endl; // Load byte into %cl (lower 8 bits of %rcx)

        // Add 1 to the byte
        *output_file << "subb    $1, %cl" << std::endl; // Increment byte in %cl

        // Store the modified byte back to the address in %rax
        *output_file << "movb    %cl, (%rax)" << std::endl; // Store byte from %cl back to memory

        // tape[tape_position] -= 1;
        break;
    case '.':

        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax

        // Load the byte from the address into %al (to use with putc)
        *output_file << "movb    (%rax), %al" << std::endl; // Load byte into %al

        // Prepare for putc
        *output_file << "movq    stdout(%rip), %rsi" << std::endl; // Load file descriptor for stdout into %rsi
        *output_file << "movsbl  %al, %edi" << std::endl;          // Move and sign-extend byte in %al to %edi

        // Call putc to print the character
        *output_file << "call    putc@PLT" << std::endl;

        // cout << tape[tape_position];
        break;
    case ',':

        // Move the file pointer for stdin into the %rdi register
        *output_file << "movq    stdin(%rip), %rdi" << std::endl;

        // Call the getc function to read a character from stdin (returned in %al)
        *output_file << "call    getc@PLT" << std::endl;
        *output_file << "movb    %al, %bl" << std::endl; // Move the byte from %al into %bl
        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load the pointer at -8(%rbp) into %rax

        // Store the byte from %al into the memory pointed to by %rax
        *output_file << "movb    %bl, (%rax)" << std::endl; // Store byte from %al into the memory pointed to by %rax

        // char nextByte;
        // cin.get(nextByte);
        // tape[tape_position] = nextByte;
        break;
    case '[':
    {
        loop_num++;
        myStack.push(loop_num);

        string start_label = "start_loop_";
        start_label += to_string(loop_num);
        string end_label = "end_loop_";
        end_label += to_string(loop_num);

        *output_file << start_label << ":" << std::endl;
        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax

        // Load the byte at the address in %rax
        *output_file << "movb    (%rax), %cl" << std::endl; // Load byte into %cl (lower 8 bits of %rcx)

        *output_file << "cmpb    $0, %cl" << std::endl;
        *output_file << "je      " << end_label << std::endl;
    }

    break;
    case ']':
    {
        int match_loop = myStack.top();
        myStack.pop();
        string start_label = "start_loop_";
        start_label += to_string(match_loop);

        string end_label = "end_loop_";
        end_label += to_string(match_loop);

        *output_file << end_label << ":" << std::endl;
        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax

        // Load the byte at the address in %rax
        *output_file << "movb    (%rax), %cl" << std::endl; // Load byte into %cl (lower 8 bits of %rcx)

        *output_file << "cmpb    $0, %cl" << std::endl;
        *output_file << "jne      " << start_label << std::endl;
    }

    break;
    default:
        // non bf instruction, so we ignore
        break;
    } // end switch

    return i;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "No input file?" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]); // Open file
    if (!inputFile)
    {
        cout << "Couldn't open file: " << argv[1] << endl;
        return 1;
    }
    // Read the file into a vector of chars
    program_file.assign((istreambuf_iterator<char>(inputFile)),
                        istreambuf_iterator<char>());
    // Close the file
    inputFile.close();

    std::ofstream outFile("bf.s");

    output_file = &outFile;
    if (!outFile)
    {
        std::cerr << "File could not be opened!" << std::endl;
        return 1; // Return with an error code
    }
    *output_file << ".file	\"bf compiler\"" << endl;
    *output_file << ".text" << endl;
    *output_file << ".section	.text" << endl;
    *output_file << ".globl	main" << endl;
    *output_file << ".type	main, @function" << endl;
    *output_file << endl;

    *output_file << "main:" << endl;

    *output_file << "pushq	%rbp" << std::endl;
    *output_file << "movq	%rsp, %rbp" << std::endl;
    // Allocate 16 bytes of stack space for local variables
    *output_file << "subq	$16, %rsp" << std::endl;
    *output_file << "movl	$100000, %edi" << std::endl;
    *output_file << "call	malloc@PLT" << std::endl;
    // Store the pointer returned by malloc in the local variable at -8(%rbp)
    *output_file << "movq	%rax, -8(%rbp)" << std::endl;

    // Calculate the address 50,000 bytes into the allocated memory
    *output_file << "movq    -8(%rbp), %rax" << std::endl; // Load base address into %rax
    *output_file << "addq    $50000, %rax" << std::endl;   // Add the offset to %rax

    // Store the adjusted pointer in the stack space at -8(%rbp)
    *output_file << "movq    %rax, -8(%rbp)" << std::endl;

    // // begin our program compiler loop
    for (int i = 0; i < program_file.size(); i++)
    {
        char ch = program_file[i];
        i = bf_interpreter(ch, i);
    }
    // Set the return value to 0 (successful completion)
    *output_file << "movl    $0, %eax" << endl;
    // Proper stack cleanup
    *output_file << "movq    %rbp, %rsp" << std::endl;
    // Restore the old base pointer
    *output_file << "popq    %rbp" << std::endl;
    // Return from the function
    *output_file << "ret" << std::endl;
    // Close the file
    outFile.close();

    std::cout << "bf program compiled successfully." << std::endl;

} // end main()
