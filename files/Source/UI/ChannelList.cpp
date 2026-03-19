/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2014 Open Ephys

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "ChannelList.h"

#include "ChannelComponent.h"

#include "../DeviceEditor.h"
#include "../DeviceThread.h"
#include "../Headstage.h"

using namespace RhythmNode;

ChannelList::ChannelList (DeviceThread* board_, DeviceEditor* editor_) : board (board_), editor (editor_), maxChannels (0)
{
    channelComponents.clear();

    //label for stim output selection (Pulse or Pattern).
    outputTypeLabel = std::make_unique<Label> ("Output Select", "Output Select");
    outputTypeLabel->setEditable (false);
    outputTypeLabel->setBounds (10, 10, 150, 25);
    addAndMakeVisible (outputTypeSelect.get());

    //Output Selection Button
    outputTypeSelect = std::make_unique<ComboBox> ("Output Selection");
    outputTypeSelect->addItem ("Pulse Output", 1);
    outputTypeSelect->addItem ("Pattern Output", 2);
    outputTypeSelect->setBounds (125, 10, 140, 25);
    outputTypeSelect->addListener (this);
    outputTypeSelect->setSelectedId (1, dontSendNotification);
    addAndMakeVisible (outputTypeSelect.get());

    //upload stim parameters
    uploadStimParamButton = std::make_unique<ColourButton> ("Upload Stim Parameters", FontOptions (14.0f));
    uploadStimParamButton->setBounds (270, 10, 170, 25);
    uploadStimParamButton->addListener (this);
    addAndMakeVisible (uploadStimParamButton.get());

    
    selectPatternFileButton = std::make_unique<ColourButton> ("Select Pattern Stim File", FontOptions (14.0f));
    selectPatternFileButton->setBounds (445, 10, 170, 25);
    selectPatternFileButton->addListener (this);
    addAndMakeVisible (selectPatternFileButton.get());

    //pulse count
    patternRepeatCount = std::make_unique<TextEditor> ("Pattern Repeat Count");
    patternRepeatCount->setText ("1");
    patternRepeatCount->setBounds (625, 10, 40, 25);
    //patternRepeatCount->addListener(this);
    addAndMakeVisible (patternRepeatCount.get());

    //count label
    //waveform input labels
    repeatLabel = std::make_unique<Label> ("Repeat Label", "Count");
    repeatLabel->setEditable (false);
    repeatLabel->setBounds (675, 10, 200, 25);
    addAndMakeVisible (repeatLabel.get());

    //waveform input labels
    waveformInputLabel = std::make_unique<Label> ("Waveform Labels", "Amplitude(uA)    Period(ms)     Pulse Width(ms)    Rise/Fall Time    #Pulses");
    waveformInputLabel->setEditable (false);
    waveformInputLabel->setBounds (10, 50, 800, 25);
    addAndMakeVisible (waveformInputLabel.get());

    //amplitude
    amplitudeInput = std::make_unique<TextEditor> ("Amplitude");
    amplitudeInput->setText ("0");
    amplitudeInput->setBounds (30, 75, 50, 25);
    addAndMakeVisible (amplitudeInput.get());

    //period
    periodInput = std::make_unique<TextEditor> ("Period");
    periodInput->setText ("0");
    periodInput->setBounds (150, 75, 50, 25);
    addAndMakeVisible (periodInput.get());

    //pulse width
    pulseWidthInput = std::make_unique<TextEditor> ("Pulse Width");
    pulseWidthInput->setText ("0");
    pulseWidthInput->setBounds (280, 75, 50, 25);
    addAndMakeVisible (pulseWidthInput.get());

    //edgeSelect
    edgeSelectBox = std::make_unique<ComboBox> ("Edge Select");
    edgeSelectBox->addItem ("0 ms", 1);
    edgeSelectBox->addItem ("0.1 ms", 2);
    edgeSelectBox->addItem ("0.5 ms", 3);
    edgeSelectBox->addItem ("1 ms", 4);
    edgeSelectBox->addItem ("2 ms", 5);
    edgeSelectBox->setBounds (400, 75, 80, 25);
    edgeSelectBox->addListener (this);
    edgeSelectBox->setSelectedId (1, dontSendNotification);
    addAndMakeVisible (edgeSelectBox.get());

    //pulse count
    pulseCountInput = std::make_unique<TextEditor> ("Pulse Count");
    pulseCountInput->setText ("0");
    pulseCountInput->setBounds (530, 75, 50, 25);
    addAndMakeVisible (pulseCountInput.get());

    //trigger select (PC, EXTERNAL)
    //triggerSelectBox = new ComboBox("Trigger Type");
    // triggerSelectBox->addItem("PC", 1);
    // triggerSelectBox->addItem("TTL IN 0", 2);
    // triggerSelectBox->addItem("TTL IN 1", 3);
    // triggerSelectBox->addItem("TTL IN 2", 4);
    // triggerSelectBox->addItem("TTL IN 3", 5);
    // triggerSelectBox->setBounds(590, 75, 80, 25);
    // triggerSelectBox->addListener(this);
    // triggerSelectBox->setSelectedId(1, dontSendNotification);
    // triggerSelectBox(edgeSelectBox);

    //Initialize the led toggle buttons
    for (int i = 0; i < 64; ++i)
    {
        toggleButtons[i] = std::make_unique<ToggleButton> ("");
        toggleButtons[i]->setSize(100, 100);
        toggleButtons[i]->addListener (this);
        addAndMakeVisible (toggleButtons[i].get());
    }

    patternOutputLabel = std::make_unique<Label> ("Output Type Alert");
    patternOutputLabel->setText ("Pattern Output is Selected!!", juce::dontSendNotification);
    patternOutputLabel->setJustificationType (juce::Justification::centred);
    patternOutputLabel->setBounds (112, 300, 200, 30);
    addAndMakeVisible(patternOutputLabel.get()); 
    patternOutputLabel->setVisible (false); // Starts hidden!

    //measures impedance (on acute configurations only).
    impedanceButton = std::make_unique<UtilityButton> ("Measure Impedances");
    impedanceButton->setRadius (3);
    impedanceButton->setBounds (280, 10, 145, 25);
    impedanceButton->setFont (FontOptions (14.0f));
    impedanceButton->addListener (this);
    //addAndMakeVisible (impedanceButton.get());

    saveImpedanceButton = std::make_unique<UtilityButton> ("Save Impedances");
    saveImpedanceButton->setRadius (3);
    saveImpedanceButton->setBounds (430, 10, 145, 25);
    saveImpedanceButton->setFont (FontOptions (14.0f));
    saveImpedanceButton->addListener (this);
    saveImpedanceButton->setEnabled (false);
    //addAndMakeVisible (saveImpedanceButton.get());

    update();
}

void ChannelList::lookAndFeelChanged()
{   

    //probably put the color selections here, make the default colors changed upon the theme color changing from editior,   

    outputTypeLabel->setColour (Label::textColourId, findColour (ThemeColours::defaultText));
    uploadStimParamButton->setColours (findColour (ThemeColours::defaultText), findColour (ThemeColours::highlightedFill));  
    selectPatternFileButton->setColours (findColour (ThemeColours::defaultText), findColour (ThemeColours::defaultFill));
  

    for (int i = 0; i < 64; ++i)
    {
        toggleButtons[i]->setColour (ToggleButton::ColourIds::tickDisabledColourId, Colours::darkgrey);
        toggleButtons[i]->setColour (ToggleButton::ColourIds::tickColourId, Colours::royalblue);
    }

    //alert window
    patternOutputLabel->setColour (juce::Label::textColourId, juce::Colours::white);
    patternOutputLabel->setColour (juce::Label::backgroundColourId, juce::Colours::darkred);
    patternOutputLabel->setFont (juce::Font (16.0f, juce::Font::bold));

    //numberingSchemeLabel->setColour (Label::textColourId, findColour (ThemeColours::defaultText));



    update();
}

void ChannelList::buttonClicked (Button* btn)
{
  
    
    //UPLOAD ENABLED LEDS AND WAVEFORM PARAMETERS
    //update the LED toggle buttons

    for (int i = 0; i < 64; ++i)
    {
        if (btn == toggleButtons[i].get())
        {
            // Handle button click
            bool isToggled = toggleButtons[i]->getToggleState();
        }
    }

    if (btn == uploadStimParamButton.get())
    {
        //upload the stimulation parameters saved in the
        //This structure should be updated upon       //any change to the UI elements.
        LOGD ("UPLOAD STIM PARAMS PRESSED")

        //set which output type to use
        board->setStimOutputType(selectedStimOutputType);

        //pulse stim selected
        if (!selectedStimOutputType)
        {
            LOGD ("Pulse Output Selected")
            //get all text entries for waveform at stim
            amplitude = amplitudeInput->getTextValue().toString().getIntValue();
            period = periodInput->getTextValue().toString().getIntValue();
            pulse_width = pulseWidthInput->getTextValue().toString().getIntValue();
            edge_sel = edgeSelectBox->getComponentID().getIntValue();
            pulse_count = pulseCountInput->getTextValue().toString().getIntValue();

            //check the enabled buttons for which LEDs to output to

            int enabledLEDS[64];
            for (int i = 0; i < 64; ++i)
            {
                if (toggleButtons[i]->getToggleState())
                {
                    enabledLEDS[ledMapping[i]] = 1;
                }
                else
                {
                    enabledLEDS[ledMapping[i]] = 0;
                }
            }

            board->uploadStimParameters(amplitude, period, pulse_width, edge_sel, pulse_count, enabledLEDS);
        }
        //pattern stim is selected
        else
        {
            LOGD ("Pattern Output Selected")
            repeatCount = (patternRepeatCount->getTextValue().toString().getIntValue()) - 1;
            board->setPatternStimRepeatCount(repeatCount);
        }

        //TEMP BUTTON TO SET THE REPEAT COUNT
    }
    //PATTERN STIMULATION BUTTON
    if (btn == selectPatternFileButton.get())
    {
        LOGD ("SELECT FILE BUTTON CLICKED")

        FileChooser fileChooser ("Please select a stimulation pattern file...");

        //opens documents folder
        if (fileChooser.browseForFileToOpen())

        {
            File chosenFile = fileChooser.getResult();
            String filename = chosenFile.getFileName();
            int64_t fileLength = chosenFile.getSize();

            String message = "File Name: " + filename + "\n" + "File Length: " + juce::String (fileLength) + " bytes";
            AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon, "Uploading Please Wait...", message);

            // Do the upload
            int uploaded = board->uploadPatternFile(chosenFile);

            if (uploaded >= 0)
                message = "File upload completed!\nFile Name: " + filename + "\nFile Length: " + juce::String (fileLength) + " bytes";
            else
                message = "File upload FAILED!\nFile Name: " + filename;

            AlertWindow::showMessageBoxAsync (juce::AlertWindow::InfoIcon, (uploaded >= 0) ? "Upload Successful" : "Upload Failed", message);
        }
    }

    /*
    if (btn == impedanceButton.get())
    {
        editor->measureImpedance();
        saveImpedanceButton->setEnabled (true);
    }
    else if (btn == saveImpedanceButton.get())
    {
        FileChooser chooseOutputFile ("Please select the location to save...",
                                      File(),
                                      "*.xml");

        if (chooseOutputFile.browseForFileToSave (true))
        {
            // Use the selected file
            File impedenceFile = chooseOutputFile.getResult();
            editor->saveImpedance (impedenceFile);
        }
    }
    */
    
}

void ChannelList::update()
{
    if (! board->foundInputSource())
    {
        disableAll();
        return;
    }

    staticLabels.clear();
    channelComponents.clear();
    uploadStimParamButton->setEnabled (true); //g3 change here

    const int columnWidth = 250;

    Array<const Headstage*> headstages = board->getConnectedHeadstages();

    int column = -1;

    maxChannels = 16; //this is the number of LEDs

    const float shankThickness = 2.0f;
    const float gap = shankThickness * 8.0f;
    const float startX = 55.0f;
    const float shankHeight = 500.0f;
    const float startY = 130.0f;
    const float shankWidth = 80.0f;
    const float buttonWidth = (shankWidth / 2.0f) + 10; // Use the same width as shankWidth for simplicity
    const float buttonHeight = (shankHeight - shankWidth * 0.5f) / buttonCountPerShank;

    // Position the toggle buttons
    for (int i = 0; i < shankCount; ++i)
    {
        float xPosition = startX + i * (shankWidth + gap);

        for (int j = 0; j < buttonCountPerShank; ++j)
        {
            // Reverse the button placement so CH1 is bottom-most
            int index = i * buttonCountPerShank + j;
            int reversedJ = buttonCountPerShank - 1 - j; // This makes j=0 bottom, j=15 top

            toggleButtons[index]->setBounds(
                (int) (xPosition),
                (int) (startY + reversedJ * buttonHeight),
                (int) (buttonWidth / 2),
                (int) (buttonHeight));
        }
    }
}


void ChannelList::disableAll()
{

    uploadStimParamButton->setEnabled (false);

    //Initialize the led toggle buttons
    for (int i = 0; i < 64; ++i)
    {
        toggleButtons[i]->setEnabled (false);
    }
}

void ChannelList::enableAll()
{
    uploadStimParamButton->setEnabled (true);
    //Initialize the led toggle buttons
    for (int i = 0; i < 64; ++i)
    {
        toggleButtons[i]->setEnabled (true);
    }
}


void ChannelList::comboBoxChanged (ComboBox* b)
{
    if (b == outputTypeSelect.get())
    {
        selectedStimOutputType = (b->getSelectedId() == 2); //pulse out is 1, pattern out is 2
        board->setStimOutputType(selectedStimOutputType);

        //pulse stim
        if (! selectedStimOutputType)
        {
            patternOutputLabel->setVisible(false);
            //Initialize the led toggle buttons
            for (int i = 0; i < 64; ++i)
            {
                toggleButtons[i]->setEnabled(true);
            }
        }
        //pattern stim
        else
        {
            patternOutputLabel->setVisible(true);
            for (int i = 0; i < 64; ++i)
            {
                toggleButtons[i]->setEnabled(false);
                patternOutputLabel->setVisible(true);
            }
        }
    }
}