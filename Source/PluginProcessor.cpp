/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthParameters.h"

//==============================================================================
TapSynthAudioProcessor::TapSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), false)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    synth.addSound(new SynthSound());
    synth.addVoice(new SynthVoice());

    apvts.reset(new juce::AudioProcessorValueTreeState(*this, nullptr, "Parameters", createParameters()));
}

TapSynthAudioProcessor::~TapSynthAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout TapSynthAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Partro para la velocidad (en Hz)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfoSpeed", "LFO Speed",
        juce::NormalisableRange<float>(0.1f, 5.0f, 0.01f),  // Rango: 0.1Hz a 5Hz
        0.5f));  // Valor por defecto: 0.5Hz (ciclo cada 2 segundos)

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "minFreq", "Minimum Frequency",
        juce::NormalisableRange<float>(50.0f, 4000.0f, 1.0f), 500.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "maxFreq", "Maximum Frequency",
        juce::NormalisableRange<float>(50.0f, 4000.0f, 1.0f), 500.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "x", "X",
        juce::NormalisableRange<float>(1.0f, 100.0f, 1.0f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "y", "Y",
        juce::NormalisableRange<float>(1.0f, 100.0f, 1.0f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "z", "Z",
        juce::NormalisableRange<float>(1.0f, 100.0f, 1.0f), 0.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String TapSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapSynthAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool TapSynthAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool TapSynthAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double TapSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int TapSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapSynthAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String TapSynthAudioProcessor::getProgramName(int index)
{
    return {};
}

void TapSynthAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void TapSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    }

    // Play C4 on start:
    synth.noteOn(midiChannel, midiNoteNumber, velocity);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;

    // Delay code:
    delayLineL.reset(); // left channel
    delayLineR.reset(); // right channel
    delayLineL.prepare(spec);
    delayLineR.prepare(spec);

    // Resize 
    delayLineL.setMaximumDelayInSamples((int)(sampleRate * 2.0)); // delay max seconds 
    delayLineR.setMaximumDelayInSamples((int)(sampleRate * 2.0));

    delayLineL.setDelay(0.01);
    delayLineR.setDelay(0.01);

    // Psychoacoustic gain
    gainL.prepare(spec);
    gainL.setGainLinear(1.0f);
}


void TapSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void TapSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    minFreq = apvts->getRawParameterValue("minFreq")->load();
    maxFreq = apvts->getRawParameterValue("maxFreq")->load();
    lfoSpeed = apvts->getRawParameterValue("lfoSpeed")->load();
    horizontalPosition = apvts->getRawParameterValue("x")->load();
    verticalPosition = apvts->getRawParameterValue("y")->load();

    lDistance = sqrt(juce::square(horizontalPosition - leftEarX) + juce::square(leftEarY - verticalPosition));
    rDistance = sqrt(juce::square(horizontalPosition - rightEarX) + juce::square(rightEarY - verticalPosition));

    float leftD = (currentSampleRate / 1000.0f)* lDistance;
    float rightD = (currentSampleRate / 1000.0f)* rDistance;
    
    delayLineL.setDelay(leftD);
    delayLineR.setDelay(rightD);
    float gainLValue = (maxDistance - lDistance) / maxDistance;
    float gainRValue = (maxDistance - rDistance) / maxDistance;
    gainL.setGainLinear(gainLValue);
    gainR.setGainLinear(gainRValue);

    // Primitive sequencer
    msCount = time.getMillisecondCounterHiRes();
    msCount = ((msCount % 1000) / 100);
    if (msCount % 2 == 0) {
        synth.noteOff(midiChannel, midiNoteNumber, velocity, true);
    }
    else if (msCount % 3 == 0) {
        synth.noteOn(midiChannel, midiNoteNumber, velocity);
    }

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const int numSamples = buffer.getNumSamples();
    float currentFreq;

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, numSamples);
    }

    // Send parameters to the Synth's voice:
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->updateParams(*apvts);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        float gain = (channel == 0) ? gainLValue : gainRValue;

        buffer.applyGain(channel, 0, buffer.getNumSamples(), gain);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float inputSample = channelData[sample];

            float delayedSample = (channel == 0)
                ? delayLineL.popSample(0)
                : delayLineR.popSample(0);

            //channelData[sample] = inputSample + 0.5f * delayedSample;
            channelData[sample] =  delayedSample;

            // Push new sample into delay buffer
            if (channel == 0) {
                delayLineL.pushSample(0, inputSample);
            } else {
                delayLineR.pushSample(0, inputSample);
            }
        }
    }
}


//==============================================================================
bool TapSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapSynthAudioProcessor::createEditor()
{
    return new TapSynthAudioProcessorEditor(*this);
}

//==============================================================================
void TapSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TapSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapSynthAudioProcessor();
}