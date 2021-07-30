#pragma once
#include <stdint.h>

struct LipWeight {
    float _kiss;
    float _press;
    float _open;
};

class AudioLip
{    
public:
    AudioLip();
    ~AudioLip();
    void InitParamters(int length);
    LipWeight ComputeLipWeights(int16_t *raw, int length);
private:
    void DCTSpectrum1D(float* src, float* result);
    void AnalyzeSpectrum(float* spectrum, LipWeight& w);
private:
    int _length;        //process sample count one frame
    float _sqrt1;       //sqrtf(1.0/length)
    float _sqrt2;       //sqrtf(2.0/length)
    float *_raw;        //int16_t--->float[-1, 1]
    float *_pts;        //dct paramters
    float *_spectrum;   //spectrum data after dct transform
};
