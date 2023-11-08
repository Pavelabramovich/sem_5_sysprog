#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <string>
#include <strstream>
#include <functional>


using ThreadData = std::vector<float>;

using namespace std;


const int MAX_THREADS = 50;

mutex inputMtx;
mutex outputMtx;
mutex coutMtx;

const char input_file_name[] = "input.txt";
const char output_file_name[] = "output.txt";


int arrays_count;
int array_size;




std::vector<float> parse_floats(std::string str)
{
    std::vector<float> ans{};

    auto ptr = new char;

    char* char_array = new char[str.length() + 1];
    strcpy_s(char_array, str.length() + 1, str.c_str());

    char* token = strtok_s(char_array, " ", &ptr);

    while (token != NULL)
    {
        try
        {
            float num = stof(token);

            if (num > 0)
                ans.push_back(num);
        }
        catch (const std::invalid_argument& e)
        { }
        catch (const std::out_of_range& e)
        { }

        token = strtok_s(NULL, " ", &ptr);
    }

    delete[] char_array;

    return ans;
}


void init_file(ofstream& input_file)
{
    for (int i = 0; i < arrays_count; i++)
    {
        for (int j = 0; j < array_size; ++j)
            input_file << (rand() % 1000) / (float)10 << ' ';

        input_file << endl;
    }

    input_file.close();
}



void ReadFile(std::ifstream& input,ThreadData& threadData)
{
    std::string line;
    getline(input, line);

    auto floats = parse_floats(line);

    for (float& f : floats)
        threadData.push_back(f);   
}

void SortThread(ThreadData& threadData)
{
    int dataSize = array_size;

    while (dataSize--)
    {
        bool swapped = false;

        for (int i = 0; i < dataSize; i++)
        {
            if (threadData[i] > threadData[i + 1])
            {
                std::swap(threadData[i], threadData[i + 1]);
                swapped = true;
            }
        }

        if (swapped == false)
            break;
    }
}

void WriteFile(ofstream& output, ThreadData& threadData)
{
    for (float number : threadData)
        output << number << ' ';

    output << endl;
}

void ThreadFunction(ifstream& input, ofstream& output, ThreadData&& threadData)
{
    unique_lock<mutex> coutLock1(coutMtx);
    cout << "Thread " << std::this_thread::get_id() << " is not ready." << endl;
    coutLock1.unlock();

    auto start_time = chrono::high_resolution_clock::now();

 
    unique_lock<mutex> inputLock(inputMtx);
    ReadFile(input, threadData);
    inputLock.unlock();

    SortThread(threadData);

    unique_lock<mutex> outputLock(outputMtx);
    WriteFile(output, threadData);
    outputLock.unlock();
    

    auto end_time = chrono::high_resolution_clock::now();

    unique_lock<mutex> coutLock2(coutMtx);
    std::cout << "Thread " << std::this_thread::get_id() << " is ready." << std::endl;
    coutLock2.unlock();

    chrono::duration<double> elapsed_time = end_time - start_time;

    unique_lock<mutex> coutLock3(coutMtx);
    std::cout << "Thread " << std::this_thread::get_id() << " completed in " << elapsed_time.count() << " seconds." << std::endl;
    coutLock3.unlock();
}

int main()
{
    std::cout << "Enter the number of arrays: ";
    std::cin >> arrays_count;


    std::cout << "Enter the size of each array: ";
    std::cin >> array_size;


    ofstream input_file(input_file_name);
    init_file(input_file);


    ifstream input(input_file_name);
    ofstream output(output_file_name);


    vector<thread> threads(arrays_count);
    vector<ThreadData> threadDataArray(arrays_count);


    for (int i = 0; i < arrays_count; i++)
    {
        threads[i] = thread(ThreadFunction, ref(input), ref(output), threadDataArray[i]);
    }

    std::cout << "Sorting array fragments with " << arrays_count << " threads started." << endl;

    for (std::thread& thread : threads)
        thread.join();

    input.close();
    output.close();

    std::cout << "Sorting completed." << std::endl;

    return 0;
}
