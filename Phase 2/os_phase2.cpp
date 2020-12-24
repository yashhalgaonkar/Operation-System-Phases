//Operating System Phase 2
// Coded by Yash Halgaonkar

#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>
#include <stdlib.h>

using namespace std;

/**
 * Memory Ccomposition :=
 * 300 words (0 to 299)
 * 30 blocks and each block has 10 words
 * valid physical address - 0 to 299
 * valid virtual address - 0 to 99
 * */

//* Global variables
ifstream infile("input1.txt");
ofstream outfile;
std::string::size_type sz; // alias of size_t

/**
 * TTL - Total Time Limit
 * TLL - Total Line Limit
 * JOB_ID - job id
 * */

typedef struct PCB
{
    int TTL, TLL, JOB_ID;
} PCB;

class OS
{
public:
    void load();
    OS()
    {
        //default constructor
    }

private:
    string next_to_amj = "cdss", data;

    /**
     * VA - virtual address
     * RA - Real Address
     * PTR - Page table register - contains the base address of the page table
     * TTC - Total Time Counter
     * TLC - Total Line Counter
     * PI - Program Interrupt
     * Si - Service Interrupt
     * TI - Timeer Interrupt
    */

    int blocks = 0, IC = 0, m = 0, VA, data_counter, address, row_no, col_no, SI = 0, PTR, TTC, TLC, PI, TI, RA, flag_for_end;
    bool occupied[30] = {false};
    string instruction;
    PCB pcb;

    char memory[300][4] = {'*', '*'}, IR[4], C, R[4];

    //functions

    void startexecution();

    void executeuserprogram();

    void MOS();

    // Function to terminate the execution using proper error message
    void TERMINATE(int);

    int calculate_address();

    void go_to_end();

    void reset_memory();

    PCB init_PCB(string file_content);

    //Function to allocate frame number
    // ALLOCATE generates a random number between 0 to 29 because we have only 30 block
    int ALLOCATE();

    //Function to check if timer has exceeded
    void SIMULATION();

    void update_page_table(int);

    //Fucntion to convert virtual address to physical address
    int ADDRESS_MAP(int VA1);

    void set_pt();

    bool isvalid(char num);

    //Function to display the content of enitre main memory
    void show_memory()
    {
        //cout << "Memory :\n";
        // for (int i = 0; i < 300; i++)
        // {
        //     cout << i << ": ";
        //     for (int j = 0; j < 4; j++)
        //         cout << memory[i][j] << " ";
        //     cout << "\n";
        // }
    }
};

void OS::go_to_end()
{
    string temp, file_content, end_check;
    end_check = data.substr(0, 4);
    if (end_check != "$END")
    {
        while (1)
        {
            if (!getline(infile, file_content))
                break;                        //Accept a string
                                              //from file
            temp = file_content.substr(0, 4); //compare temp to check
                                              // the actions to be perform
            if (temp == "$END")               //break the while loop
            {
                break;
            }
        }
    }
    flag_for_end = 1;
    reset_memory();
    return;
}

void OS::reset_memory()
{
    for (int i = 0; i < 300; i++)
        for (int j = 0; j < 4; j++)
            memory[i][j] = '*';
    occupied[0] = true;
    for (int i = 1; i < 30; i++)
        occupied[i] = false;
    IC = 0;
    PI = 0;
    TI = 0;
    TTC = 0;
    TLC = 0;
}

PCB OS::init_PCB(string file_content)
{
    string
        job_id = file_content.substr(4, 4);
    int
        job_id1 = std::stoi(job_id, &sz);
    pcb.JOB_ID = job_id1;
    string
        TTL = file_content.substr(8, 4);
    int
        TTL1 = std::stoi(TTL, &sz);
    pcb.TTL = TTL1;
    string
        TLL = file_content.substr(12, 4);
    int
        TLL1 = std::stoi(TLL, &sz);
    pcb.TLL = TLL1;
    return pcb;
}

//Generates a random number between 0 to 29
int OS::ALLOCATE()
{
    int page_no;
    while (1)
    {
        page_no = rand() % 30;
        //keep generating frame numbers untill we get a number that is not occupied
        if (!occupied[page_no])
        {
            occupied[page_no] = true;
            break;
        }
    }
    return page_no;
}

//Converts the virtual address to physical/ real address
int OS::ADDRESS_MAP(int VA1)
{
    int loc, fno, RA, flag1 = 1;
    if (VA1 >= 100)
    {
        //virtual address is greater than 99
        PI = 2; //* Operand Error
        MOS();
        return 0;
    }
    loc = PTR + (VA1 / 10);
    if (memory[loc][0] == '0')
    {
        PI = 3; //* Page Fault Error
        MOS();
        if (flag_for_end)
            return -1;
    }
    fno = (memory[loc][2] - '0') * 10 + (memory[loc][3] - '0');
    RA = (fno * 10) + (VA1 % 10);
    return RA;
}

// Function to initialize the page table
void OS::set_pt()
{
    int counter = PTR;
    while (counter < (PTR + 9))
    {
        for (int i = 0; i < 4; i++)
            memory[counter][i] = '0';
        counter++;
    }
}

void OS::update_page_table(int frame_no)
{
    for (int i = PTR; i < (PTR + 10); i++)
    {
        if (memory[i][0] == '0')
        {
            memory[i][0] = '1';
            int dec = (frame_no / 10);
            int unit = (frame_no % 10);
            memory[i][2] = (dec + '0');
            memory[i][3] = (unit + '0');
            break;
        }
    }
}

void OS::load()
{
    int flag = 0;
    reset_memory();
    while (infile)
    {
        int frame_no, fm;
        string file_content, sub_string, temp, trimmed_string;
        if (!getline(infile, file_content))
            break;                        //Accept a string
                                          //from file
        temp = file_content.substr(0, 4); //compare temp to check
                                          // the actions to be perform
        if (temp == "$END")               //break the while loop
        {
            //Reset all values here
            reset_memory();
            IC = m = data_counter = address = 0;
            outfile << "\n\n";
            continue;
            //break;
        }
        else if (temp == "$AMJ") //for $AMJ go on accepting
                                 //from file till you get H

        {
            flag_for_end = 0;
            PCB pcb_obj = init_PCB(file_content);

            cout << "Execution log for JobID : " << pcb.JOB_ID << endl;
            cout << "TTL: " << pcb.TTL << "\nTLL: " << pcb.TLL << endl
                 << endl;

            //ALLOCATE memory for page table
            PTR = ALLOCATE(); //generated the frame number
            PTR *= 10;
            //PTR += 1;
            //Initialize the page table
            set_pt();
            flag = 0;
            while (getline(infile, next_to_amj)) //Read File till we get H
            {
                frame_no = ALLOCATE();

                //put the generated frame number into the page table
                update_page_table(frame_no);

                frame_no *= 10;
                fm = frame_no;
                while (next_to_amj != "") //Read lines next to amj
                {
                    trimmed_string = next_to_amj.substr(0, 4);
                    if (!trimmed_string.find('H'))
                    {
                        memory[frame_no][0] = 'H';
                        // memory[frame_no][1]=' ';
                        // memory[frame_no][2]=' ';
                        // memory[frame_no][3]=' ';
                        flag = 1;
                        break;
                    }
                    next_to_amj.erase(0, 4);
                    memory[frame_no][0] = trimmed_string[0]; //Store this instructions
                    memory[frame_no][1] = trimmed_string[1]; //in memory which is available
                    memory[frame_no][2] = trimmed_string[2];
                    memory[frame_no][3] = trimmed_string[3];
                    frame_no++;
                }
                if (flag == 1) //We got h as substring in inner loop so break
                    break;
            }
            cout << "Memory after loading the code: \n";
            show_memory();
        }
        else if (temp == "$DTA")
        {
            // Start executing control cards
            startexecution();
        }
    }
}

void OS::startexecution()
{
    IC = 0;
    executeuserprogram();
}

void OS::executeuserprogram()
{
    int address, row;
    string end = "H";
    while (1)
    {
        SI = 0;

        // Convert the virtual address in IC to physical(Real) address
        RA = ADDRESS_MAP(IC);
        IC++;
        string temp, temp1;
        for (int j = 0; j < 4; j++)
            IR[j] = memory[RA][j];
        temp = IR[0];
        temp1 = IR[1];
        instruction = temp + temp1;
        if (instruction[0] == 'H' && instruction[1] == '*')
        {
            TTC++;
            SI = 3; // hault
            MOS();
            break;
        }
        VA = calculate_address();
        if (flag_for_end)
        {
            flag_for_end = 0;
            break;
        }
        //convert the virtual address in form of operand to real address
        RA = ADDRESS_MAP(VA);
        //cout << "RA : " << RA << endl;
        if (instruction == "GD")
        {
            //TTC++;
            cout << "Get data from RA: " << RA << endl;
            SI = 1; //For GD System interupt is set to 1
            MOS();
        }
        else if (instruction == "PD")
        {
            SI = 2; //For PD System interupt is set to 2
            cout << "Data printed to output file at RA: " << RA << endl;
            MOS();
        }
        else if (instruction == "LR")
        {
            //Check for the page fault error before loading the register
            if (PI == 3)
                TERMINATE(6);
            for (int i = 0; i < 4; i++)
                R[i] = memory[RA][i];
            cout << "Loaded general register with RA : " << RA << endl;
        }
        else if (instruction == "SR")
        {
            //TTC++;
            PI = 0;
            for (int i = 0; i < 4; i++)
                memory[RA][i] = R[i];
            cout << "Content of general register stored at RA : " << RA << endl;
        }
        else if (instruction == "CR")
        {
            int flag = 0;
            for (int i = 0; i < 4; i++)
                if (memory[RA][i] != R[i])
                    flag = 1;
            if (flag == 0)
                C = 'T';
            else
                C = 'F';
            flag = 0;
            cout << "C register has been set to: " << C << endl;
        }
        else if (instruction == "BT")
        {
            if (C == 'T')
                IC = ((IR[2] - 48) * 10) + (IR[3] - 48);
            C = 'F';
            cout << "IC register has been set to: " << IC << endl;
        }
        else if (instruction == "H")
        {
            SI = 3; // for halt, set the system interrupt to 3
            MOS();
        }
        else
        {
            //If non of the above condition is satisfied... then the opcode is wrong
            PI = 1; // For opcode error, set the program interrupt to 1
        }
        if (flag_for_end)
        {
            flag_for_end = 0;
            break;
        }

        //check for the time limit exceeded error
        SIMULATION();

        if (PI != 0 || TI != 0)
            MOS();
        if (flag_for_end)
        {
            flag_for_end = 0;
            break;
        }

        //incerment the timer after succesfull execution of a single instruction
        if (instruction == "GD" || instruction == "SR")
            TTC += 2;
        else
            TTC++;
    }

    reset_memory();
}

void OS::MOS()
{
    int row_no1;

    //* If not a timer interrupt but system interrupt
    if (TI == 0)
    {
        switch (SI)
        {
        case 1: //GD Operation will take place here
        {
            string temp;
            data = "";
            PI = 0;
            getline(infile, data);
            temp = data.substr(0, 4);
            if (temp == "$END" || temp == "")
            {
                //* Out of data Error
                TERMINATE(1);
            }
            int counter = 0;
            for (int i = RA; counter < (data.length()); i++)
                for (int j = 0; j < 4 && counter < (data.length()); j++)
                {
                    memory[i][j] = data[counter];
                    counter += 1;

                    if (counter > 40)
                    {
                        //cout << "Data card cannot be bigger than 40bytes";
                        //break;
                    }
                }
            break;
        }
        case 2: // In PD Data is written to output file
                // from ex 30 to 39 mem location (10 words)
        {
            TLC++;

            if (TLC > pcb.TLL)
            {
                //* Line limit exceeded error
                TERMINATE(2);
                return;
            }
            if (PI == 3)
            {
                //* Page fault error
                TERMINATE(6);
                return;
            }
            int row;
            string space = " ";
            int count = 0;
            for (int row = RA; count < 10; row++)
            {
                count++;
                for (int col = 0; col < 4; col++)
                {
                    if (memory[row][col] != '*')
                    {
                        outfile << memory[row][col];
                    }
                }
            }
            outfile << "\n";
            break;
        }
        case 3:
        {
            //Terminate with no error
            TERMINATE(0);
            return;
        }
        }
    }

    //* incase of a timer interrupt and system interrupt
    //timer and systerm interrupt combined are handled here
    else if (TI == 2)
    {

        switch (SI)
        {
        case 1: //incase of GD:= TERMINATE (3)
        {
            TERMINATE(3);
            break;
        }
        case 2: //incase of PD:=  WRITE, THEN TERMINATE (3)
        {
            TERMINATE(3);
            break;
        }
        case 3: //incase of halt:= TERMINATE (0)
        {
            TERMINATE(0);
            break;
        }
        }
    }

    //*if not a timer interrupt but a program interrupt
    if (TI == 0)
    {
        switch (PI)
        {
        case 1: //incase of Opcode error
            TERMINATE(4);
            break;
        case 2: //incase of operand error
            TERMINATE(5);
            break;
        case 3:
            //* Incase of PAGE FAULT
            if (instruction == "GD" || instruction == "SR")
            {
                //this is a valid page fault
                int allocator = ALLOCATE();
                address = PTR + VA / 10;
                memory[address][0] = 0;
                memory[address][2] = ((allocator / 10) + '0');
                memory[address][3] = ((allocator % 10) + '0');
                allocator *= 10;
                RA = allocator;
                flag_for_end = 0;
                //IC--;

                cout << "Valid page falut at IC: " << IC << endl;
            }
            else
            {
                //invalid page fault
                TERMINATE(6); // Terminate with PageFault error
                cout << "Invalid page falut at IC: " << IC << endl;
            }
            break;
        }
    }

    //*Timer and Program error combine are handled here
    else if (TI == 2)
    {
        switch (PI)
        {
        case 1: //incase of opcode error := TLE with Opcode Error
            TERMINATE(7);
            break;
        case 2: // incase of operand error := TLE with Operand error
            TERMINATE(8);
            break;
        case 3: // incase of pafe fault := TLE
            TERMINATE(3);
            break;
        default:
            // TLE
            TERMINATE(3);
            break;
        }
    }
    SI = 0;
}

//Fucntion to calculate the real address from the operand
int OS::calculate_address()
{
    //check if the operand is a valid number
    if (isvalid(IR[2]) && isvalid(IR[3]))
        VA = (IR[2] - '0') * 10 + (IR[3] - '0');
    else
    {
        //operand is not a valid number
        PI = 2; // operand error
        SIMULATION();
        MOS();
        return 0;
    }
    return VA;
}

// Fucntion to terminate by printing appropriate error message
void OS::TERMINATE(int ERR_CODE)
{
    switch (ERR_CODE)
    {
    case 0:
        outfile << "cpu_em : NO ERROR\n";
        break;
    case 1:
        outfile << "cpu_em : OUT OF DATA\n";
        break;
    case 2:
        outfile << "cpu_em : LINE LIMIT EXCEEDED\n";
        break;
    case 3:
        outfile << "cpu_em : TIME LIMIT EXCEEDED\n";
        break;
    case 4:
        outfile << "cpu_em : OPERATION CODE ERROR\n";
        break;
    case 5:
        outfile << "cpu_em : OPERAND ERROR\n";
        break;
    case 6:
        outfile << "cpu_em : INVALID PAGE FAULT\n";
        break;
    case 7:
        outfile << "cpu_em : TIME LIMIT EXCEEDED\ncpu_em : OPERATION CODE ERROR\n";
        break;
    case 8:
        outfile << "cpu_em : TIME LIMIT EXCEEDED\ncpu_em : OPERAND ERROR\n";
        break;
    }
    cout << "Memory after execution:\n";
    show_memory();
    cout << "End of Execution of JobID: " << pcb.JOB_ID << "\n\n\n";
    outfile << "mos_execute : Job Details \n"
            << "JID\t"
            << "IC\t"
            << "IR\t"
            << "TTL\t"
            << "TLL\t"
            << "TTC\t"
            << "TLC\n";
    outfile << pcb.JOB_ID << "\t" << IC << "\t" << IR[0] << IR[1] << IR[2] << IR[3] << "\t" << pcb.TTL << "\t" << pcb.TLL << "\t" << TTC << "\t" << TLC << "\n\n\n";
    go_to_end();
}

void OS::SIMULATION()
{
    if (TTC >= pcb.TTL)
    {
        TI = 2;
        MOS();
    }
}

//*Helper functions
bool OS::isvalid(char num)
{
    if (num >= '0' && num <= '9')
        return true;
    return false;
}

int main()
{
    OS myos;
    outfile.open("output.txt");
    myos.load();
    infile.close();
    outfile.close();
}
