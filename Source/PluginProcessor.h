#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/

using MonoChain = juce::dsp::ProcessorChain<juce::dsp::Reverb>;
using APVTS = juce::AudioProcessorValueTreeState;

class SReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SReverbAudioProcessor();
    ~SReverbAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    juce::dsp::Reverb::Parameters getReverbParams(const APVTS& avpts);
    void updateReverb(const juce::dsp::Reverb::Parameters& reverbParams);
    void toggleReverb(MonoChain leftChain, MonoChain rightChain);


    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:
    //==============================================================================
    
    MonoChain leftChain, rightChain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SReverbAudioProcessor)
};
