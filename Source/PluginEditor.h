#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

struct RotaryLookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
};

struct RotarySliderWithLabels : juce::Slider {

    RotarySliderWithLabels(juce::RangedAudioParameter* rangedAudioParam) :
        juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(rangedAudioParam)
    {
        setLookAndFeel(&lookAndFeel);
    }

    ~RotarySliderWithLabels()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;

    void createInnerArc(juce::Graphics& g, const juce::Rectangle<float> &bounds, float arcWidth, float arcRadius, float startAngle, float endAngle);

    void createTick(juce::Graphics& g, const juce::Rectangle<float>& bounds, int tickWidth, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle);

    void drawSliderText(juce::Graphics& g);

    juce::Rectangle<int> getSliderBounds() const;

    int getTextHeight() const { return 14; };

    juce::Colour getPrimaryColour() { return juce::Colour(209u, 69u, 69u); }
    
    juce::Colour getSecondaryColour() { return juce::Colour(143u, 47u, 47u); }

    juce::String getDisplayValue() const;

private:
    RotaryLookAndFeel lookAndFeel;
    juce::RangedAudioParameter* param;
};

struct LinearSliderLookAndFeel : juce::LookAndFeel_V4
{
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        const juce::Slider::SliderStyle, juce::Slider& slider) override;
};

struct VerticalSlider : juce::Slider {

    VerticalSlider(juce::RangedAudioParameter* rangedAudioParam) :
        juce::Slider(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextEntryBoxPosition::NoTextBox),
        param(rangedAudioParam)
    {
        setLookAndFeel(&lookAndFeel);
    }

    ~VerticalSlider()
    {
        setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override;

    void drawSliderText(juce::Graphics& g);

    juce::Rectangle<int> getSliderBounds() const;

    int getTextHeight() const { return 18; }

    juce::Colour getPrimaryColour() { return juce::Colour(209u, 69u, 69u); }

    juce::Colour getSecondaryColour() { return juce::Colour(143u, 47u, 47u); }

    juce::String getDisplayValue() const;

private:
    LinearSliderLookAndFeel lookAndFeel;
    juce::RangedAudioParameter* param;
};

class SReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SReverbAudioProcessorEditor (SReverbAudioProcessor&);
    ~SReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void setParemeterIDs();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SReverbAudioProcessor& audioProcessor;

    VerticalSlider wetLevelSlider, dryLevelSlider;
    RotarySliderWithLabels sizeSlider, dampingSlider, widthSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    SliderAttachment wetLevelSliderAttachment, dryLevelSliderAttachment, sizeSliderAttachment, dampingSliderAttachment, widthSliderAttachment;

    juce::Image logo = juce::ImageFileFormat::loadFrom(juce::File("Source/Assets/logo.png"));

    std::vector<juce::Component*> getComponents();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SReverbAudioProcessorEditor)
};
