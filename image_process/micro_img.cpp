#include "micro_img.h"
#include <iostream>
#include <limits.h>

/*
Dynamically allocate n x m array of uint8_t type. 
Args:
    n : number of rows
    m : number of cols
    value : initialized value

Returns:
    arr : pointer to dynamically allocated  2d array
*/
uint8_t** microimg::micro_img::initialize_array_uint8_t(int n, int m, int value)
{

    // Dynamically allocate the memory
    uint8_t** arr = new uint8_t* [n];
    for(int i = 0; i < n; i++)
    {
        arr[i] = new uint8_t[m];
    }
    
    // Initialize the array
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            arr[i][j] = value;
        }
    }
    
    return arr;
}

/*
Dynamically allocate n x m array of int16_t type. 
Args:
    n : number of rows
    m : number of cols
    value : initialized value

Returns:
    arr : pointer to dynamically allocated  2d array
*/
int16_t** microimg::micro_img::initialize_array_int16_t(int n, int m, int value)
{

    // Dynamically allocate the memory
    int16_t** arr = new int16_t* [n];
    for(int i = 0; i < n; i++)
    {
        arr[i] = new int16_t[m];
    }
    
    // Initialize the array
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<m; j++)
        {
            arr[i][j] = value;
        }
    }
    
    return arr;
}


/*
Pad the array so that the size of the image
remains the same after convolution.
The function frees the arr memory and returns
a pointer to newly allocated 2d array of the 
appropriate dimensions

Args:
   uint8_t** arr : pointer to 2d array
   int n_arr : number of rows
   int m_arr : number of cols
   int kernel_size : size of the kernel
   int stride : stride of convolution

Returns:
    padded_arr : pointer to dynamically allocated  2d array
*/
uint8_t** microimg::micro_img::padding(uint8_t** arr, int n_arr, int m_arr, int kernel_size, int stride)
{
    // Calculate padding size
    int padding_size = ((n_arr - 1) * stride + kernel_size - n_arr);

    // Allocate 2d array of appropriate dimensions
    uint8_t** padded_arr = initialize_array_uint8_t(n_arr + padding_size, m_arr + padding_size, 0);
    
    // Move the image from arr to padded_arr
    for(int i=0; i<n_arr; i++)
    {
        for(int j=0; j<m_arr; j++)
        {
            padded_arr[i + padding_size/2][j + padding_size/2] = arr[i][j];
        }
    }
    // Free the memory
    delete_arr(arr, n_arr);

    return padded_arr;
}

/*
Free the dynamically allocated 2d array

Args:
   uint8_t** arr : dynamically allocated 2d array
   int n : number of rows
*/
void microimg::micro_img::delete_arr(uint8_t** arr, int n)
{
    for(int i=0; i<n; i++)
    {
        delete arr[i];
    }
    return;
}

/*
Free the dynamically allocated 2d array

Args:
   int16_t** arr : dynamically allocated 2d array
   int n : number of rows
*/
void microimg::micro_img::delete_arr(int16_t** arr, int n)
{
    for(int i=0; i<n; i++)
    {
        delete arr[i];
    }
    return;
}

/*
Applies convolution operation.
Frees the source image memory
Returns a pointer to dynamically allocated 2d array

Args:
   uint8_t** img : pointer to 2d array 
   int n_img : number of rows in image
   int m_img : number of cols in image
   int** kernel : pointer to 2d array
   int n_kernel : number of rows in kernel
   int m_kernel : number of cols in kernel

Returns:
    final_result : pointer to dynamically allocated 2d array
*/
uint8_t** microimg::micro_img::conv2d(uint8_t** img, int n_img, int m_img, int** kernel, int n_kernel, int m_kernel)
{
    int n_result = calc_img_size(n_img, n_kernel, 1);
    int m_result = calc_img_size(m_img, m_kernel, 1);

    // Store temp result in int16_t
    int16_t** result = initialize_array_int16_t(n_result, m_result, 0);
    
    // Initialize
    for(int n = 0; n < n_result ; n++)
    {
        for(int m = 0; m < m_result; m++)
        {
            result[n][m] = 0;
        }
    }
    

    // Store result in uint8_t
    uint8_t** final_result = initialize_array_uint8_t(n_result, m_result, 0);
    int16_t max_val = SCHAR_MIN;
    int16_t min_val = SCHAR_MAX;
   
    
    // Convolution operation
    for(int i = 0; i < n_result ; i++)
    {
        for(int j = 0; j < m_result; j++)
        {
            for(int n = 0; n < n_kernel ; n++)
            {
                for(int m = 0; m < m_kernel; m++)
                {
                    result[i][j] += (img[i+m][j+n] * kernel[m][n]) / (n_kernel * m_kernel);
                    max_val= std::max(max_val, result[i][j]);
                    min_val = std::min(min_val, result[i][j]);
                }
            }
        }
    }
    
    // Convert from int16_t to uint8_t by normalization
    for(int n = 0; n < n_result ; n++)
    {
        for(int m = 0; m < m_result; m++)
        {
            final_result[n][m] = ((result[n][m] - (float)min_val) / ((float)max_val - (float)min_val)) * 255;
        }
    }

    // Free the temp result
    delete_arr(result,n_result);

    // Free the source image
    delete_arr(img, n_img);

    return final_result;
}

/*
Calculates image size after convolution 

Args:
    int n_img : dimension size
    int n_kernel : size of kernel
    int stride  : stride of convolution

Returns:
   int n_result : size of the image after convolution
*/
int microimg::micro_img::calc_img_size(int n_img, int n_kernel, int stride)
{
    int n_result = (n_img - n_kernel) / stride + 1;
    return n_result;
}


/*
Dynamically allocate average filter. 

Returns:
   int** arr : dynamically allocated 2d array
               of size 3 x 3
*/
int** microimg::average()
{
    int** arr = new int* [3];
    for(int i = 0; i < 3; i++)
    {
        arr[i] = new int[3];
    }
    

    arr[0][0] = 1;
    arr[0][1] = 1;
    arr[0][2] = 1;
    
    arr[1][0] = 1;
    arr[1][1] = 1;
    arr[1][2] = 1;
    
    arr[2][0] = 1;
    arr[2][1] = 1;
    arr[2][2] = 1;
    
    return arr;
}


/*
Dynamically allocate sobel filter. 

Returns:
   int** arr : dynamically allocated 2d array
               of size 3 x 3
*/
int** microimg::sobel_filter_x()
{
    int** arr = new int* [3];
    for(int i = 0; i < 3; i++)
    {
        arr[i] = new int[3];
    }
    

    arr[0][0] = -1;
    arr[0][1] = 0;
    arr[0][2] = 1;
    
    arr[1][0] = -2;
    arr[1][1] = 0;
    arr[1][2] = 2;
    
    arr[2][0] = -1;
    arr[2][1] = 0;
    arr[2][2] = 1;
    
    return arr;
}
