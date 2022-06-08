#include "PluginProcessor.h"
#include "PluginEditor.h"

// === ROTARY SLIDER ===================================================================================

void RotaryLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{
    using namespace juce;
    auto bounds = Rectangle<float>(x, y, width, height);
    auto centre = bounds.getCentre();

    if (auto* rotarySlider = dynamic_cast<RotarySliderWithLabels*>(&slider)) {

        g.setColour(rotarySlider->getPrimaryColour());
        g.fillEllipse(bounds);

        // Creating the arc that the tick is bound to
        rotarySlider->createInnerArc(g, bounds, 6.0f, 4, rotaryStartAngle, rotaryEndAngle);
        // Creating the tick that indicates the angle of the slider
        rotarySlider->createTick(g, bounds, 8, sliderPosProportional, rotaryStartAngle, rotaryEndAngle);

        // Generating text that displays the current value of the parameter
        Rectangle<float> textBounds;
        g.setFont(rotarySlider->getTextHeight());
        auto text = rotarySlider->getDisplayValue();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        textBounds.setSize(strWidth + 4, rotarySlider->getTextHeight() + 2);
        textBounds.setCentre(centre);
        g.drawFittedText(text, textBounds.toNearestInt(), juce::Justification::centred, 1);
    }
}

void RotarySliderWithLabels::createInnerArc(juce::Graphics& g, const juce::Rectangle<float> &bounds, float arcWidth, float arcRadius, float startAngle, float endAngle)
{
    juce::Path innerArc;
    auto centre = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    arcRadius = radius - arcWidth * 0.5f - arcRadius;
    innerArc.addCentredArc(centre.getX(), centre.getY(), arcRadius, arcRadius, 0.0f, startAngle, endAngle, true);
    g.setColour(this->getSecondaryColour());
    g.strokePath(innerArc, juce::PathStrokeType(arcWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void RotarySliderWithLabels::createTick(juce::Graphics &g, const juce::Rectangle<float> &bounds, int tickWidth, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle)
{
    juce::Path tick;
    juce::Rectangle<float> tickBounds;
    auto centre = bounds.getCentre();

    tickBounds.setLeft(centre.getX() - tickWidth / 2);
    tickBounds.setRight(centre.getX() + tickWidth / 2);
    tickBounds.setTop(bounds.getY() + tickWidth / 2);
    tickBounds.setBottom(bounds.getY() + 4 + tickWidth);
    tick.addEllipse(tickBounds);

    auto sliderAngleRad = juce::jmap(sliderPosProportional, 0.0f, 1.0f,
        rotaryStartAngle, rotaryEndAngle);

    tick.applyTransform(juce::AffineTransform().rotated(sliderAngleRad, centre.getX(),
        centre.getY()));

    g.setColour(juce::Colours::white);
    g.fillPath(tick);
}

void RotarySliderWithLabels::paint(juce::Graphics & g)
{
    using namespace juce;

    auto startAngle = degreesToRadians(180.0f + 45.0f);
    auto endAngle = degreesToRadians(180.0f - 45.0f) + MathConstants<float>::twoPi;
    auto range = getRange();
    auto sliderBounds = getSliderBounds();
    
    getLookAndFeel().drawRotarySlider(
        g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(),
        sliderBounds.getHeight(),
        jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), startAngle,
        endAngle, *this);

    drawSliderText(g);
    
};

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    auto sideLen = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.8;
    juce::Rectangle<int> sliderBounds;
    sliderBounds.setSize(sideLen, sideLen);
    sliderBounds.setCentre(bounds.getCentreX(), bounds.getBottom() - sideLen/2);
    return sliderBounds;
};

void RotarySliderWithLabels::drawSliderText(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    juce::Rectangle<int> textBox = bounds.removeFromTop(bounds.getHeight() * 0.2);
    auto text = getComponentID();
    g.setColour(juce::Colours::white);
    g.drawFittedText(text, textBox, juce::Justification::centred, 1);
}

juce::String RotarySliderWithLabels::getDisplayValue() const
{
    juce::String str;
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
        float val = getValue();
        str = juce::String(val);
    }
    return str;
};

// === LINEAR SLIDER =================================================================================

void LinearSliderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    const juce::Slider::SliderStyle, juce::Slider& slider)
{
    using namespace juce;
    
    auto sliderBounds = Rectangle<float>(x, y, width, height);

    if (auto* verticalSlider = dynamic_cast<VerticalSlider*>(&slider)) {     
        
        g.setColour(verticalSlider->getPrimaryColour());
        g.fillRoundedRectangle(sliderBounds, 15.0f);
        int padding = 16;
        Rectangle<float> innerBounds(x + padding, y + padding, width - padding*2, height - padding*4);
        g.setColour(verticalSlider->getSecondaryColour());
        g.fillRoundedRectangle(innerBounds, 10.0f);

        Path tick;
        Rectangle<float> tickBounds;
        tickBounds.setLeft(sliderBounds.getCentreX() - 12);
        tickBounds.setRight(sliderBounds.getCentreX() + 12);
        tickBounds.setTop(sliderBounds.getY() + 4);
        tickBounds.setBottom(sliderBounds.getY() + 28);
        tick.addEllipse(tickBounds);
 
        auto tickMovement = jmap(sliderPos, 0.0f, 1.0f,
            minSliderPos, maxSliderPos);

        tick.applyTransform(AffineTransform().translated(0, tickMovement));

        g.setColour(juce::Colours::white);
        g.fillPath(tick);

        juce::Rectangle<int> textBounds;
        auto text = verticalSlider->getDisplayValue();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        textBounds.setSize(strWidth + 4, verticalSlider->getTextHeight() + 2);
        textBounds.setCentre(sliderBounds.getCentreX(), sliderBounds.getBottom() - 24);
        g.drawFittedText(text, textBounds.toNearestInt(), juce::Justification::centred, 1);
    }
};

void VerticalSlider::paint(juce::Graphics& g)
{
    using namespace juce;
    auto sliderBounds = getSliderBounds();

    drawSliderText(g);

    getLookAndFeel().drawLinearSlider(
        g, sliderBounds.getX(), sliderBounds.getY(), sliderBounds.getWidth(),
        sliderBounds.getHeight(), getValue(), sliderBounds.getBottom() - 96, sliderBounds.getY() - 24, getSliderStyle(), *this);
};

void VerticalSlider::drawSliderText(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto textBox = bounds.removeFromTop(bounds.getHeight() * 0.066);
    auto text = getComponentID();
    g.setColour(juce::Colours::white);
    g.setFont(this->getTextHeight());
    g.drawText(text, textBox, juce::Justification::centred);
}

juce::Rectangle<int> VerticalSlider::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    juce::Rectangle<int> sliderBounds;
    sliderBounds.setCentre(bounds.getCentreX(), bounds.getCentreY());
    sliderBounds.setTop(bounds.getY() + (bounds.getHeight()*0.066));
    sliderBounds.setBottom(bounds.getBottom()); 
    sliderBounds.setLeft(bounds.getX() + (bounds.getWidth() / 6));
    sliderBounds.setRight(bounds.getWidth() - (bounds.getWidth() / 6));
    
    return sliderBounds;
};

juce::String VerticalSlider::getDisplayValue() const
{
    juce::String str;
    if (auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
        float val = getValue();
        str = juce::String(val);
    }
    return str;
};

//=== EDITOR ===========================================================================

SReverbAudioProcessorEditor::SReverbAudioProcessorEditor (SReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    wetLevelSlider(audioProcessor.apvts.getParameter("Wet")),
    dryLevelSlider(audioProcessor.apvts.getParameter("Dry")),
    sizeSlider(audioProcessor.apvts.getParameter("Size")),
    dampingSlider(audioProcessor.apvts.getParameter("Damping")),
    widthSlider(audioProcessor.apvts.getParameter("Width")),
    wetLevelSliderAttachment(audioProcessor.apvts, "Wet",
        wetLevelSlider),
    dryLevelSliderAttachment(audioProcessor.apvts, "Dry",
        dryLevelSlider),
    sizeSliderAttachment(audioProcessor.apvts, "Size",
        sizeSlider),
    dampingSliderAttachment(audioProcessor.apvts, "Damping",
        dampingSlider),
    widthSliderAttachment(audioProcessor.apvts, "Width",
        widthSlider)
{
    setParemeterIDs();

    for (auto* comp : getComponents()) {
        addAndMakeVisible(comp);
    }
    
    setSize (300, 600);
}

SReverbAudioProcessorEditor::~SReverbAudioProcessorEditor()
{
}

void SReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.setColour (juce::Colour(33u, 33u, 33u));
    g.fillAll();

    auto bounds = getLocalBounds();
    
    auto logo = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);
    auto logoBounds = bounds.removeFromTop(bounds.getHeight() * 0.2);
    g.drawImageWithin(logo, logoBounds.getX(), logoBounds.getY(), logoBounds.getWidth(), logoBounds.getHeight(), juce::RectanglePlacement::centred);

    auto footerBounds = bounds.removeFromBottom(bounds.getHeight() * 0.12);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    juce::String gitHub = juce::String("github.com/ncvetan");
    g.drawFittedText(gitHub, footerBounds.toNearestInt(), juce::Justification::centred, 1);
}

void SReverbAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    auto textArea = bounds.removeFromTop(bounds.getHeight() * 0.2);
    auto footer = bounds.removeFromBottom(bounds.getHeight() * 0.12);

    auto verticalSliderArea = bounds.removeFromLeft(bounds.getWidth() * 0.5);
    wetLevelSlider.setBounds(verticalSliderArea.removeFromLeft(verticalSliderArea.getWidth() * 0.5));
    dryLevelSlider.setBounds(verticalSliderArea);
    
    auto rotarySliderArea = bounds;
    sizeSlider.setBounds(rotarySliderArea.removeFromTop(rotarySliderArea.getHeight() * 0.33));
    dampingSlider.setBounds(rotarySliderArea.removeFromTop(rotarySliderArea.getHeight() * 0.5));
    widthSlider.setBounds(rotarySliderArea);
}

std::vector<juce::Component*> SReverbAudioProcessorEditor::getComponents()
{
    return {&wetLevelSlider, &dryLevelSlider, &sizeSlider, &dampingSlider, &widthSlider };
}

void SReverbAudioProcessorEditor::setParemeterIDs() {
    wetLevelSlider.setComponentID("Wet");
    dryLevelSlider.setComponentID("Dry");
    sizeSlider.setComponentID("Size");
    dampingSlider.setComponentID("Damping");
    widthSlider.setComponentID("Width");
}
