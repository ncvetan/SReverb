/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SReverbAudioProcessor::SReverbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SReverbAudioProcessor::~SReverbAudioProcessor()
{
}

//==============================================================================
const juce::String SReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void SReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
    leftChain.get<0>().prepare(spec);
    rightChain.get<0>().prepare(spec);

    auto reverbParams = getReverbParams(apvts);

    updateReverb(reverbParams);
}

void SReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void SReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto reverbParams = getReverbParams(apvts);

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    updateReverb(reverbParams);

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool SReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SReverbAudioProcessor::createEditor()
{
    return new SReverbAudioProcessorEditor (*this);
    // return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream outputStream(destData, true);
    apvts.state.writeToStream(outputStream);
}

void SReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto input = juce::ValueTree::readFromData(data, sizeInBytes);
    auto reverbParams = getReverbParams(apvts);

    if (input.isValid()) {
        updateReverb(reverbParams);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SReverbAudioProcessor();
}

void SReverbAudioProcessor::updateReverb(const juce::dsp::Reverb::Parameters& reverbParams)
{
    leftChain.get<0>().setParameters(reverbParams);
    rightChain.get<0>().setParameters(reverbParams);
};

void SReverbAudioProcessor::toggleReverb(MonoChain leftChain, MonoChain rightChain)
{
    if ((leftChain.get<0>().isEnabled()) && (rightChain.get<0>().isEnabled()))
    {
        leftChain.get<0>().setEnabled(false);
        rightChain.get<0>().setEnabled(false);
    }
    else
    {
        leftChain.get<0>().setEnabled(true);
        rightChain.get<0>().setEnabled(true);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout SReverbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("Dry", "Dry",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Wet", "Wet",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.33f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Size", "Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Damping", "Damping",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Width", "Width",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.5f), 1.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Freeze", "Freeze",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f, 0.5f), 0.0f));

    return layout;
}

juce::dsp::Reverb::Parameters SReverbAudioProcessor::getReverbParams(const APVTS& avpts)
{
    juce::dsp::Reverb::Parameters reverbParams;

    reverbParams.dryLevel = avpts.getRawParameterValue("Dry")->load();
    reverbParams.wetLevel = avpts.getRawParameterValue("Wet")->load();
    reverbParams.damping = avpts.getRawParameterValue("Damping")->load();
    reverbParams.roomSize = avpts.getRawParameterValue("Size")->load();
    reverbParams.width = avpts.getRawParameterValue("Width")->load();
    reverbParams.freezeMode = avpts.getRawParameterValue("Freeze")->load();

    return reverbParams;
}