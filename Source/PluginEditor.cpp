/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapSynthAudioProcessorEditor::TapSynthAudioProcessorEditor(TapSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {

    // Configuracion del slider de velocidad
    lfoSpeedSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfoSpeedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    //lfoSpeedSlider.setRange(0.1, 5.0, 0.01);  // Coincide con el rango del parametro
    addAndMakeVisible(lfoSpeedSlider);

    lfoSpeedLabel.setText("Speed", juce::dontSendNotification);
    lfoSpeedLabel.attachToComponent(&lfoSpeedSlider, false);
    lfoSpeedLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(lfoSpeedLabel);


    // Configurar sliders
    minFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    minFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    maxFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    maxFreqSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    xSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    xSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    ySlider.setSliderStyle(juce::Slider::LinearVertical);
    ySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    zSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    zSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(minFreqSlider);
    addAndMakeVisible(maxFreqSlider);
    addAndMakeVisible(xSlider);
    addAndMakeVisible(ySlider);
    addAndMakeVisible(zSlider);

    // Configure labels
    minLabel.setText("Minimum Pitch (Hz)", juce::dontSendNotification);
    minLabel.attachToComponent(&minFreqSlider, false);
    minLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(minLabel);

    maxLabel.setText("Maximum Pitch (Hz)", juce::dontSendNotification);
    maxLabel.attachToComponent(&maxFreqSlider, false);
    maxLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(maxLabel);

    xLabel.setText("X", juce::dontSendNotification);
    xLabel.attachToComponent(&xSlider, false);
    xLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(xLabel);

    yLabel.setText("Y", juce::dontSendNotification);
    yLabel.attachToComponent(&ySlider, false);
    yLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(yLabel);

    zLabel.setText("Gain", juce::dontSendNotification);
    zLabel.attachToComponent(&zSlider, false);
    zLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(zLabel);

    // Vincular sliders a etiquetas
    minFreqAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "minFreq", minFreqSlider));

    maxFreqAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "maxFreq", maxFreqSlider));

    lfoSpeedAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(),
        "lfoSpeed", lfoSpeedSlider));

    xAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(),
        "x", xSlider));

    yAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(),
        "y", ySlider));

    zAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(),
        "z", zSlider));

    setSize (400, 500);
}

TapSynthAudioProcessorEditor::~TapSynthAudioProcessorEditor()
{
}

//==============================================================================
void TapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void TapSynthAudioProcessorEditor::resized()
{
    const int margin = 20;
    const int sliderWidth = 150;
    const int sliderHeight = 150;

    minFreqSlider.setBounds(margin, 40, sliderWidth * 2 / 3, sliderHeight * 2 / 3);
    lfoSpeedSlider.setBounds(getWidth() / 2 - sliderWidth / 2, 40, sliderWidth * 2 / 3, sliderWidth * 2 / 3);
    maxFreqSlider.setBounds(getWidth() - sliderWidth - margin, 40, sliderWidth * 2 / 3, sliderHeight * 2 / 3);

    xSlider.setBounds(margin, getHeight()/2, sliderWidth * 2 / 3, sliderHeight * 2 / 3);
    ySlider.setBounds(getWidth() / 2 - sliderWidth / 2, getHeight()/2, sliderWidth * 2 / 3, sliderWidth * 2 / 3);
    zSlider.setBounds(getWidth() - sliderWidth - margin, getHeight()/2, sliderWidth * 2 / 3, sliderHeight * 2 / 3);
}
