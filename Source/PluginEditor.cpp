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
    gainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    dimensionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    dimensionSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    addAndMakeVisible(minFreqSlider);
    addAndMakeVisible(maxFreqSlider);
    addAndMakeVisible(xSlider);
    addAndMakeVisible(ySlider);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(dimensionSlider);

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

    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(gainLabel);

    dimensionLabel.setText("Size of the box (square meters)", juce::dontSendNotification);
    dimensionLabel.attachToComponent(&dimensionSlider, false);
    dimensionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(dimensionLabel);

    // Vincular sliders a etiquetas
    minFreqAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "minFreq", minFreqSlider));

    maxFreqAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "maxFreq", maxFreqSlider));

    lfoSpeedAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "lfoSpeed", lfoSpeedSlider));

    xAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "x", xSlider));

    yAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "y", ySlider));

    zAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "gain", gainSlider));

    dimensionAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getState(), "dimension", dimensionSlider));

    setSize (width, heigth);
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
    const int xMargin = 20, yMargin = 60;
    const int sliderWidth = getWidth() / 4;
    const int sliderHeight = getHeight() / 4; 

    // First row
    minFreqSlider.setBounds(
        xMargin, 
        yMargin, 
        sliderWidth, 
        sliderHeight);

    lfoSpeedSlider.setBounds(
        getWidth() / 2 - sliderWidth / 2, 
        yMargin,
        sliderWidth, 
        sliderHeight);

    maxFreqSlider.setBounds(
        getWidth() - sliderWidth - xMargin, 
        yMargin,
        sliderWidth, 
        sliderHeight);

    // Second row
    xSlider.setBounds(
        xMargin, 
        yMargin*2 + sliderHeight, 
        sliderWidth, 
        sliderHeight);
    ySlider.setBounds(
        getWidth() / 2 - sliderWidth / 2,
        yMargin*2 + sliderHeight,
        sliderWidth, 
        sliderHeight);
    gainSlider.setBounds(
        getWidth() - sliderWidth - xMargin, 
        yMargin*2 + sliderHeight, 
        sliderWidth, 
        sliderHeight);

    // Third row
    dimensionSlider.setBounds(
        xMargin,
        yMargin * 3 + sliderHeight * 2,
        getWidth() - xMargin * 2,
        sliderHeight);
}
