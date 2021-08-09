#include <stdint.h>

namespace microimg{

    int** average();
    int** sobel_filter_x();

    /*
    Class for onboard image processing
    */
    class micro_img
    {
        public:
        
            uint8_t** initialize_array_uint8_t(int n, int m, int value);
            int16_t** initialize_array_int16_t(int n, int m, int value);
            void delete_arr(uint8_t** arr, int n);
            void delete_arr(int16_t** arr, int n);
            uint8_t** padding(uint8_t** arr, int n_arr, int m_arr, int kernel_size, int stride);
            uint8_t** conv2d(uint8_t** img, int n_img, int m_img, int** kernel, int n_kernel, int m_kernel);
            int calc_img_size(int n_img, int n_kernel, int stride);
    };

}