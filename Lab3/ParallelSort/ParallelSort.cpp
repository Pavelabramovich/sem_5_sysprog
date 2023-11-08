#include <iostream>
#include <vector>
#include <thread>
#include <string>


void sort(int* nums, int begin, int end, int depth = 0);


std::vector <std::thread> threads;

int arr_size;
int t_depth;


int main(int argc, char** argv) 
{

    auto f = arr_size;
    std::cout << " Enter array size: ";
    std::cin >> arr_size;

    int* arr = new int[arr_size];

    std::cout << " Enter multithread sort depth: ";
    std::cin >> t_depth;

    for (int i = 0; i < arr_size; i++)
        arr[i] = rand() % 100 + 1;

    for (int i = 0; i < arr_size; i++)
        std::cout << arr[i] << std::endl;

    sort(arr, 0, arr_size, 0);

    std::cout << std::endl;

    for (int i = 0; i < arr_size; i++)
        std::cout << arr[i] << std::endl;
}


void sort(int* arr, int begin, int end, int depth)
{
    if (depth == t_depth && false)
    {
        std::thread t(sort, arr, begin, end, 1000);
        threads.push_back(std::move(t));

        if (end == arr_size)
        {
            for (auto& t : threads) {
                t.join();
            }
        }
    }
    else if (begin >= end)
    {
        return;
    }
    else
    {
        int l = begin;
        int r = end - 1;
        int p = arr[l + (r - l) / 2];

        while (l <= r)
        {
            while ((l < end - 1) && (arr[l] < p))
                l++;

            while ((r > begin) && (arr[r] > p))
                r--;

            if (l <= r)
            {
                std::swap(arr[l], arr[r]);

                if (l < end - 1)
                    l++;
                
                if (r > 0)               
                    r--;           
            }
        }

        if (begin < r)
            sort(arr, begin, r + 1, depth + 1);

        if (l < end - 1)
            sort(arr, l, end, depth + 1);
    }
}