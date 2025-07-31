/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TapSynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TapSynthAudioProcessorEditor (TapSynthAudioProcessor&);
    ~TapSynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TapSynthAudioProcessor& audioProcessor;

    float heigth = 500, width = 400;

    // Min Slider
    juce::Slider minFreqSlider;
    juce::Label minLabel;
    // Max Slider
    juce::Slider maxFreqSlider;
    juce::Label maxLabel;
    // LFO Slider
    juce::Slider lfoSpeedSlider;
    juce::Label lfoSpeedLabel;
    // X Slider
    juce::Slider xSlider;
    juce::Label xLabel;
    // Y Slider
    juce::Slider ySlider;
    juce::Label yLabel;
    // Gain Slider
    juce::Slider gainSlider;
    juce::Label gainLabel;
    // Dimension Slider
    juce::Slider dimensionSlider;
    juce::Label dimensionLabel;



    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoSpeedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> minFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> maxFreqAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> yAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> zAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dimensionAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapSynthAudioProcessorEditor)
};
