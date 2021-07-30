#include "AudioLip.h"
#include <math.h>

#define SAMPLE_RATE 48000

static float Clamp(float val, float min, float max) {
	if (val < min) {
		return min;
	}
	else if (val > max) {
		return max;
	}
	return val;
}

AudioLip::AudioLip()
{
    _length = 0;
    _sqrt1 = 0;
    _sqrt2 = 0;
    _raw = nullptr;
    _pts = nullptr;
    _spectrum = nullptr;
}

AudioLip::~AudioLip()
{
    if (_raw) {
        delete[] _raw;
    }

    if (_pts) {
        delete[] _pts;
    }
}

void AudioLip::InitParamters(int length)
{
    _length = length;
    _sqrt1 = sqrtf(1.0f / _length);
    _sqrt2 = sqrtf(2.0f / _length);

    _raw = new float[_length];
    _pts = new float[_length * length];
    _spectrum = new float[_length];

    for (int i = 0; i < _length; i++) {
       _pts[i] = _sqrt1;
    }

    for (int m = 0; m < _length; m++) {
        for (int k = 0; k < _length; k++) {
            _pts[m * _length + k] = _sqrt2 * cosf((3.14f / _length) * m * (k + 0.5f));
        }
    }
}

void AudioLip::DCTSpectrum1D(float* src, float* result)
{
    result[0] = 0;
    for (int i = 0; i < _length; i++) {
        result[0] += src[i];
    }
    result[0] /= _length;
    if (result[0] < 0) {
        result[0] = -result[0];
    }
    for (int m = 1; m < _length; ++m) {
        result[m] = 0;
        for (int k = 0; k < _length; ++k) {
            result[m] += src[k] * _pts[m * _length + k];
        }
        result[m] *= _sqrt2;
        if (result[m] < 0) {
            result[m] = -result[m];
        }
    }
}

void AudioLip::AnalyzeSpectrum(float* spectrum, LipWeight& w)
{
    float sensitivityThreshold = 0.4f;
	float* samples = spectrum;
	for (int i = 0; i < _length; i++) {
		samples[i] = 20.0f * log10f(samples[i]);
		samples[i] = sensitivityThreshold + (samples[i] + 20) / 140.0f;
	}

	//// Calculate energy for each bin.
	int voiceBoundingFrequencies[] = { 0, 500, 700, 3000, 6000 };
	float vocalTractLengthFactor = 1.0f;
	int frequencyDataIndices[5];
	for (int i = 0; i < 5; i++) {
		voiceBoundingFrequencies[i] = (int)(voiceBoundingFrequencies[i] * vocalTractLengthFactor);
		frequencyDataIndices[i] = (int)(2.0f * _length / SAMPLE_RATE * voiceBoundingFrequencies[i]);
	}

	float binEnergy[5] = {};
	for (int i = 0; i < 4; i++) {
		int indexStart = frequencyDataIndices[i];
		int indexEnd = frequencyDataIndices[i + 1];
		float binLength = indexEnd - indexStart;

		for (int j = indexStart; j < indexEnd; j++) {
			binEnergy[i] += (samples[j] > 0 ? samples[j] : 0);
		}

		binEnergy[i] /= binLength;
	}
	float blendKissShape, blendLipsPressedShape, blendMouthOpenShape, blendLipsClosedShape;
	// Calculate blend shape weight from bin energy.
	// Kiss shape.
	if (binEnergy[1] >= 0.2f) {
		blendKissShape = 1.0f - 2 * binEnergy[2];
		blendKissShape = Clamp(blendKissShape, 0, 1);
	} else {
		blendKissShape = (1 - 2 * binEnergy[2]) * 5 * binEnergy[1];
		blendKissShape = Clamp(blendKissShape, 0, 1);
	}

	// Lips press shape.
	blendLipsPressedShape = 3 * binEnergy[3];
	blendLipsPressedShape = Clamp(blendLipsPressedShape, 0, 1);

	// Mouth open shape
	blendMouthOpenShape = 0.8f * (binEnergy[1] - binEnergy[3]);
	blendMouthOpenShape = Clamp(blendMouthOpenShape, 0, 1);


    /// for filter
	static float lastOpen0 = 0, lastOpen1 = 0;
	static float lastPressed0 = 0, lastPressed1 = 0;
	static float lastKiss0 = 0, lastKiss1 = 0;

	w._open = 0.4f * blendMouthOpenShape + 0.3f * lastOpen0 + 0.3f * lastOpen1;
	w._press = 0.4f * blendLipsPressedShape + 0.3f * lastPressed0 + 0.3f * lastPressed1;
	w._kiss = 0.4f * blendKissShape + 0.3f * lastKiss0 + 0.3f * lastKiss1;


	lastOpen1 = lastOpen0;
	lastOpen0 = blendMouthOpenShape;

	lastPressed1 = lastPressed0;
	lastPressed0 = blendLipsPressedShape;

	lastKiss1 = lastKiss0;
	lastKiss0 = blendKissShape;
}

LipWeight AudioLip::ComputeLipWeights(int16_t *raw, int length)
{
    LipWeight weight = {0.0f, 0.0f, 0.0f};
    if (length < _length) {
        return weight;
    }

    for (int i = 0; i < _length; i++) {
        _raw[i] = raw[i] / 32768.0f;
    }
    DCTSpectrum1D(_raw, _spectrum);

    AnalyzeSpectrum(_spectrum, weight);

    return weight;
}