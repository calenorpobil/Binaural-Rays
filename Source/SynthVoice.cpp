/*
  ==============================================================================

    SynthVoice.cpp
    Created: 27 Jul 2025 9:49:41am
    Author:  Carlos

  ==============================================================================
*/

#include "SynthVoice.h"


bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    osc.setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
    adsr.noteOn();
    delayLeft.reset();
    delayRight.reset();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    adsr.noteOff();
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{

}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{

}

void SynthVoice::updateParams(const juce::AudioProcessorValueTreeState& apvts)
{
    lfoSpeed = apvts.getRawParameterValue("lfoSpeed");
    minFreq = apvts.getRawParameterValue("minFreq");
    maxFreq = apvts.getRawParameterValue("maxFreq");
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    adsr.setSampleRate(sampleRate);

    adsr.setParameters(adsrParams);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = outputChannels;

    osc.prepare(spec);
    gain.prepare(spec);
    gain.setGainLinear(0.3f);
    // Prepare delay lines
    delayLeft.prepare(spec);
    delayRight.prepare(spec);

    synthBuffer.setSize(outputChannels, samplesPerBlock);

    isPrepared = true;
}

void SynthVoice::renderNextBlock(juce::AudioBuffer< float >& outputBuffer, int startSample, int numSamples)
{
    jassert(isPrepared);


    if (!isVoiceActive())
        return;

    // Create a temporary mono buffer for oscillator output
    juce::AudioBuffer<float> monoBuffer(1, numSamples);
    monoBuffer.clear();

    // Clear our internal buffer (stereo)
    synthBuffer.clear();

    // Pitch Oscillator
    float currentFreq = 400;
    float lfoSpeedFloat = lfoSpeed->load();
    float minFreqFloat = minFreq->load();
    float maxFreqFloat = maxFreq->load();
    if (lfoSpeed != nullptr && minFreq != nullptr) {

        // Velocidad del cambio de pitch
        lfoPhase += 2000*lfoSpeedFloat / currentSampleRate;
        if (lfoPhase >= 1.0f)
            lfoPhase -= 1.0f;

        // 1. Actualizar LFO (sinusoidal)
        lfoPhase += lfoFreq / currentSampleRate;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;

        // 2. Calcular frecuencia actual
        const float lfoValue = std::sin(2.0f * juce::MathConstants<float>::pi * lfoPhase);
        currentFreq = minFreqFloat + (maxFreqFloat - minFreqFloat) * (0.5f + 0.5f * lfoValue);


        osc.setFrequency(currentFreq);

    }


    juce::dsp::AudioBlock<float> audioBlock{ synthBuffer };
    delayLeft.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    delayRight.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    osc.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    gain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    adsr.applyEnvelopeToBuffer(synthBuffer, startSample, synthBuffer.getNumSamples());
    // Handles stereo
    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
    {


        outputBuffer.addFrom(channel, startSample,
            synthBuffer, channel % synthBuffer.getNumChannels(),
            0, numSamples);
    }



    if (!adsr.isActive())
        clearCurrentNote();

}




















