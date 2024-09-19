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
stack<int> myStack;

void bf_assembler(char token)
{

    switch (token)
    {
    case '>':
        // Load base address into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl; 
        // add one to pointer address
        *output_file << "addq    $1, %rax" << endl;       
        // Store the adjusted pointer back at -8(%rbp)
        *output_file << "movq    %rax, -8(%rbp)" << endl;
        // tape_position++;
        break;
    case '<':

        // Load base address into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl; 
        // remove one from pointer address
        *output_file << "subq    $1, %rax" << endl;       
        // Store the adjusted pointer back at -8(%rbp)
        *output_file << "movq    %rax, -8(%rbp)" << endl;
        // tape_position--;
        break;
    case '+':

        // Load base address into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl;
        // Load byte into %cl (lower 8 bits)
        *output_file << "movb    (%rax), %cl" << endl; 

        // Add 1 to the byte
        *output_file << "addb    $1, %cl" << endl;

        // Store the modified byte back to the address in %rax
        *output_file << "movb    %cl, (%rax)" << endl; 

        // tape[tape_position] += 1;
        break;
    case '-':

        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl;

         // Load byte into %cl (lower 8 bits)
        *output_file << "movb    (%rax), %cl" << endl;

        // Decrrement byte in %cl
        *output_file << "subb    $1, %cl" << endl; 

        // Store the modified byte back to the address in %rax
        *output_file << "movb    %cl, (%rax)" << endl; 

        // tape[tape_position] -= 1;
        break;
    case '.':

        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl; 

        // Load the byte from the address into %al (to use with putc)
        *output_file << "movb    (%rax), %al" << endl; 

        // Prepare for putc
        // Load file descriptor for stdout into %rsi
        *output_file << "movq    stdout(%rip), %rsi" << endl; 
        // Move and sign-extend byte in %al to %edi
        *output_file << "movsbl  %al, %edi" << endl;          

        // Call putc to print the character
        *output_file << "call    putc@PLT" << endl;

        // cout << tape[tape_position];
        break;
    case ',':

        // Move the file pointer for stdin into the %rdi register
        *output_file << "movq    stdin(%rip), %rdi" << endl;

        // Call the getc function to read a character from stdin (returned in %al)
        *output_file << "call    getc@PLT" << endl;
        // Move the byte from %al into %bl
        *output_file << "movb    %al, %bl" << endl; 
        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl; 

        // Store the byte from %bl into the memory pointed to by %rax
        *output_file << "movb    %bl, (%rax)" << endl; 

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

        *output_file << start_label << ":" << endl;
        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl; 

        // Load byte into %cl (lower 8 bits)
        *output_file << "movb    (%rax), %cl" << endl; 
        // jump to matching end label if 0
        *output_file << "cmpb    $0, %cl" << endl;
        *output_file << "je      " << end_label << endl;
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

        *output_file << end_label << ":" << endl;
        // Load the pointer from -8(%rbp) into %rax
        *output_file << "movq    -8(%rbp), %rax" << endl;

        // Load byte into %cl (lower 8 bits)
        *output_file << "movb    (%rax), %cl" << endl; 

        //jump to matching start label if not 0
        *output_file << "cmpb    $0, %cl" << endl;
        *output_file << "jne      " << start_label << endl;
    }

    break;
    default:
        // non bf instruction, so we ignore
        break;
    } // end switch

 
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "No input file?" << endl;
        return 1;
    }
    // Open bf file
    ifstream inputFile(argv[1]); 
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

    //create our output file
    ofstream outFile("bf.s");
    output_file = &outFile;

    if (!outFile)
    {
       cout << "could not create output file"<<endl;
        return 2; // Return with an error code
    }

    //Assembly setup
    *output_file << ".file	\"bf compiler\"" << endl;
    *output_file << ".text" << endl;
    *output_file << ".section	.text" << endl;
    *output_file << ".globl	main" << endl;
    *output_file << ".type	main, @function" << endl;
    *output_file << endl;

    *output_file << "main:" << endl;

    *output_file << "pushq	%rbp" << endl;
    *output_file << "movq	%rsp, %rbp" << endl;
    // Allocate 16 bytes of stack space for local variables
    *output_file << "subq	$16, %rsp" << endl;
    // Allocate 100,000 bytes with malloc
    *output_file << "movl	$100000, %edi" << endl;
    *output_file << "call	malloc@PLT" << endl;
    // Store the pointer returned by malloc in the local variable at -8(%rbp)
    *output_file << "movq	%rax, -8(%rbp)" << endl;

    // Calculate the address 50,000 bytes into the allocated memory
    *output_file << "movq    -8(%rbp), %rax" << endl; // Load base address into %rax
    *output_file << "addq    $50000, %rax" << endl;   // Add the offset to %rax

    // Store the adjusted pointer back at -8(%rbp)
    *output_file << "movq    %rax, -8(%rbp)" << endl;

    // // begin our program compiler loop
    for (int i = 0; i < program_file.size(); i++)
    {
        char ch = program_file[i];
        bf_assembler(ch);
    }
    // Set the return value to 0 (successful completion)
    *output_file << "movl    $0, %eax" << endl;
    // Proper stack cleanup
    *output_file << "movq    %rbp, %rsp" << endl;
    // Restore the old base pointer
    *output_file << "popq    %rbp" << endl;
    // Return from the function
    *output_file << "ret" << endl;
    // Close the file
    outFile.close();

    cout << "bf program compiled successfully." << endl;

} // end main()
